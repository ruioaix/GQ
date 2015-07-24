#include "net.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <limits.h>
#include <string.h>


void freeNET(NET *net) {
	if (!net) return;
	int i;
	for (i = 0; i < net->size; ++i) {
		freeCORE(net->core[i]);
	}
	free(net->core);
	free(net);
}

static int set_size_NET(const struct LineFile * const lf, struct NETATTR na) {
	if (na.direct == UNDIRECTED) {
		return columnsNumLF(lf) - 1;
	}
	if (na.direct == DIRECTED) {
		return 2 * (columnsNumLF(lf) - 1);
	}
	LOG(LOG_FATAL, "You have to decide the NET's direct ATTR.");
}

static inline void set_maxid_minid_NET(int *i1, int *i2, long linesNum, int *maxId_retn, int *minId_retn) {
	long i;
	int maxId = i1[0];
	int minId = i1[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > i1[i] ? maxId : i1[i];	
		maxId = maxId > i2[i] ? maxId : i2[i];	
		minId = minId < i1[i] ? minId : i1[i];
		minId = minId < i2[i] ? minId : i2[i];
	}
	*maxId_retn = maxId;
	*minId_retn = minId;
}

static inline void set_degree_degreeMax_degreeMin_idNum_NET(int *i1, int *i2, long linesNum, int maxId, int *degree, int *degreeMax, int *degreeMin, int *idNum_retn) {
	int countMax = 0;
	int countMin = INT_MAX;
	int idNum=0;
	long i;
	for(i = 0; i < linesNum; ++i) {
		if (i1) ++degree[i1[i]];
		if (i2) ++degree[i2[i]];
	}
	int j;
	for(j=0; j<maxId+1; ++j) {
		countMax = countMax > degree[j] ? countMax : degree[j];
		countMin = countMin < degree[j] ? countMin : degree[j];
		if (degree[j]>0) {
			++idNum;
		}
	}
	
	*degreeMax = countMax;
	*degreeMin = countMin;
	*idNum_retn = idNum;
}

static inline void set_rela_aler_NET(int *i1, int *objtoi1, int *i2, int *objtoi2, double *dd, int *degree, int maxId, long linesNum, int ***rela_retn, double ***aler_retn) {
	int i;
	int **rela = NULL;
   	if (objtoi1 || objtoi2) rela = smalloc((maxId + 1) * sizeof(int *));
	double **aler = NULL;
	if (dd) aler = smalloc((maxId + 1) * sizeof(double *));

	for(i = 0; i < maxId + 1; ++i) {
		if (degree[i] != 0) {
			if (rela) rela[i] = smalloc(degree[i] * sizeof(int));
			if (aler) aler[i] = smalloc(degree[i] * sizeof(double));
		}
		else {
			if (rela) rela[i] = NULL;
			if (aler) aler[i] = NULL;
		}
	}

	int *temp_count = scalloc(maxId + 1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		if (objtoi1) {
			int ii1 = i1[i];
			if (rela) rela[ii1][temp_count[ii1]]=objtoi1[i];
			if (aler) aler[ii1][temp_count[ii1]]=dd[i];
			++temp_count[ii1];
		}
		if (objtoi2) {
			int ii2 = i2[i];
			if (rela) rela[ii2][temp_count[ii2]]=objtoi2[i];
			if (aler) aler[ii2][temp_count[ii2]]=dd[i];
			++temp_count[ii2];
		}
	}
	free(temp_count);

	*rela_retn = rela;
	*aler_retn = aler;
}

static inline NETCORE *assignNET(int maxId, int minId, long edgesNum, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, double **aler) {
	NETCORE *nc = smalloc(sizeof(NETCORE));
	nc->maxId=maxId;
	nc->minId=minId;
	nc->edgesNum=edgesNum;
	nc->idNum=idNum;
	nc->degreeMax = degreeMax;
	nc->degreeMin = degreeMin;
	nc->degree = degree;
	nc->rela = rela;
	nc->aler = aler;
	return nc;
}

NETCORE *oneNETCORE(int maxId, int minId, long linesNum, int *i1, int *objtoi1, int *i2, int *objtoi2, double *dd) {
		int *degree = scalloc(maxId+1, sizeof(int));
		int degreeMax, degreeMin, idNum;
		set_degree_degreeMax_degreeMin_idNum_NET(i1, i2, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);
		int **rela;
		double **aler;
		set_rela_aler_NET(i1, objtoi1, i2, objtoi2, dd, degree, maxId, linesNum, &rela, &aler);
		return assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
}

NET *createNET(const struct LineFile * const lf, struct NETATTR na) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	NET *net = smalloc(sizeof(NET));
	net->size = set_size_NET(lf, na);
	net->core = smalloc(net->size * sizeof(NETCORE *));

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int j = 0;
	if (na.direct == UNDIRECTED) {
		net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, i2, i2, i1, NULL);
	}
	else if (na.direct == DIRECTED) {
		net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, i2, NULL, NULL, NULL);
		net->core[j++] = oneNETCORE(maxId, minId, linesNum, NULL, NULL, i2, i1, NULL);
	}
	else {
		LOG(LOG_FATAL, "You have to decide the NET's direct ATTR.");
	}

	int i;
	for (i = 2; i < lf->iNum; ++i) {
		int *ii = *(lf->ilist[i]);
		if (ii) {
			if (na.direct == UNDIRECTED) {
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, ii, i2, ii, NULL);
			}
			else {
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, ii, NULL, NULL, NULL);
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, NULL, NULL, i2, ii, NULL);
			}
		}
	}
	for (i = 0; i < lf->dNum; ++i) {
		double *dd = *(lf->dlist[i]);
		if (dd) {
			if (na.direct == UNDIRECTED) {
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, NULL, i2, NULL, dd);
			}
			else {
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, i1, NULL, NULL, NULL, dd);
				net->core[j++] = oneNETCORE(maxId, minId, linesNum, NULL, NULL, i2, NULL, dd);
			}
		}
	}

	LOG(LOG_INFO, "NETS created, contains [%3d] NETs.", net->size);
	LOG(LOG_INFO, "  Max: %5d, Min: %5d, edgesNum: %5ld", maxId, minId, linesNum);
	return net;
}

void printNET(NET *net, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i, j, k;
	NETCORE *nc = net->core[0];

	for (i = 0; i < nc->maxId + 1; ++i) {
		for (j = 0; j < nc->degree[i]; ++j) {
			fprintf(fp, "%d\t", i);
			if (net->attr.direct == UNDIRECTED) {
				for (k = 0; k < net->size; ++k) {
					if (net->core[k]->rela) fprintf(fp, "\t%d", net->core[k]->rela[i][j]);
					if (net->core[k]->aler) fprintf(fp, "\t%f", net->core[k]->aler[i][j]);
				}
				fprintf(fp, "\n");
				continue;
			}
			if (net->attr.direct == DIRECTED) {
				for (k = 0; k < net->size; k += 2) {
					if (net->core[k]->rela) fprintf(fp, "\t%d", net->core[k]->rela[i][j]);
					if (net->core[k]->aler) fprintf(fp, "\t%f", net->core[k]->aler[i][j]);
				}
				fprintf(fp, "\n");
			}
		}
	}
	fclose(fp);
	LOG(LOG_INFO, "NET printed into \"%s\".", filename);
}
