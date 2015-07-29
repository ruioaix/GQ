#include "process.h"
#include "random.h"
#include "log.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "spng.h"

void process_core(int maxId, int *degree, int **rela, double rate_infect, double deltat, double TI, double TR, int *numi, int *infected, int *numr, int *recovered, int *status, int *sign, int*infected_new, int *recovered_new, int *sus_new, double *TIR) {
	int i, j, k;
	int newi = 0, newr = 0, news = 0;
	int oldi = *numi;
	int oldr = *numr;
	memset(sign, 0, (maxId + 1) * sizeof(int));
	for (i = 0; i < oldi; ) {
		int inode = infected[i];
		for (j = 0; j < degree[inode]; ++j) {
			int snode = rela[inode][j];
			if (status[snode] || sign[snode]) continue;
			int count = 0;
			for (k = 0; k < degree[snode]; ++k) {
				int neigh = rela[snode][k];
				if (status[neigh] == 1) ++count;
				//LOG(LOG_DBG, "neigh %d, status %d", neigh, status[neigh]);
			}
			double infect_rate = 1 - exp( - rate_infect * count * deltat);
			//LOG(LOG_DBG, "snode %d, count %d, rate_infect %f, infect_rate %f", snode, count, rate_infect, infect_rate);
			if (randomd() < infect_rate) {
				infected_new[newi++] = snode;
			}
			sign[snode] = 1;
		}
		TIR[inode] += deltat;
		if (TIR[inode] >= TI) {
			recovered_new[newr++] = inode;
			infected[i] = infected[oldi - 1];
			--oldi;
			TIR[inode] = 0;
		}
		else {
			++i;
		}
	}

	for (i = 0; i < oldr;) {
		int rnode = recovered[i];
		TIR[rnode] += deltat;
		if (TIR[rnode] >= TR) {
			sus_new[news++] = rnode;
			recovered[i] = recovered[oldr - 1];	
			--oldr;
			TIR[rnode] = 0;
		}
		else {
			++i;
		}
	}

	for (i = 0; i < newr; ++i) {
		status[recovered_new[i]] = 2;
		recovered[oldr + i] = recovered_new[i];
	}
	for (i = 0; i < newi; ++i) {
		status[infected_new[i]] = 1;
		infected[oldi + i] = infected_new[i];
	}
	for (i = 0; i < news; ++i) {
		status[sus_new[i]] = 0;
	}

	*numi = oldi + newi;
	*numr = oldr + newr;
}

static void save_status(int maxId, int *status, int step, int width, int height) {
	char fn[100];
	sprintf(fn, "result/STEP_%d.txt", step + 1);
	FILE *fo = sfopen(fn, "w");
	int j;
	for (j = 0; j < maxId + 1; ++j) {
		fprintf(fo, "%d\t%d\n", j, status[j]);
	}
	fclose(fo);

	BITMAP bp;
	bp.width = width;
	bp.height = height;
	bp.pixels = scalloc(bp.width * bp.height, sizeof(PIXEL));
	int i;
	for (i = 0; i < maxId + 1; ++i) {
		int x = i%width;
		int y = i/width;
		if (status[i] == 0) {
			setPIXEL(&bp, x, y, 255, 255, 255);
		}
		else if (status[i] == 1) {
			setPIXEL(&bp, x, y, 255, 0, 0);
		}
		else if (status[i] == 2) {
			setPIXEL(&bp, x, y, 0, 0, 255);
		}
		else {
			LOG(LOG_FATAL, "wrong status");
		}
	}
	sprintf(fn, "result/STEP_%05d.png", step + 1);
	savePNG(&bp, fn);
	free(bp.pixels);
}

void process(NET *net, double rate_infect, double deltat, double TI, double TR, int STEP, int *status, int width, int height) {
	int maxId = net->maxId;
	int *degree = net->degree;
	int **rela = net->rela;

	int *infected = smalloc((maxId + 1) * sizeof(int));
	int numi = 0;
	int *recovered = smalloc((maxId + 1) * sizeof(int));
	int numr = 0;
	double *TIR = smalloc((maxId + 1) * sizeof(double));

	int i;
	for (i = 0; i < maxId + 1; ++i) {
		TIR[i] = 0;
		if (status[i] == 1) {
			infected[numi++] = i;	
		}
		else if (status[i] == 2) {
			recovered[numr++] = i;
		}
	}

	int *infected_new = smalloc((maxId + 1) * sizeof(int));
	int *recovered_new = smalloc((maxId + 1) * sizeof(int));
	int *sus_new = smalloc((maxId + 1) * sizeof(int));
	int *sign = smalloc((maxId + 1) * sizeof(int));


	for (i = 0; i < STEP; ++i) {
		process_core(maxId, degree, rela, rate_infect, deltat, TI, TR, &numi, infected, &numr, recovered, status, sign, infected_new, recovered_new, sus_new, TIR);
		save_status(maxId, status, i, width, height);
	}

	free(TIR);
	free(infected);
	free(recovered);
	free(sus_new);
	free(infected_new);
	free(recovered_new);
	free(sign);
}
