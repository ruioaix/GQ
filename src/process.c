#include "process.h"
#include "random.h"
#include "log.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

void process_core(int maxId, int *degree, int **rela, double rate_infect, double rate_recover, int *numi, int *infected, int *status, int *sign, int*infected_new, int *recovered_new) {
	int i, j, k;
	int newi = 0, newr = 0;
	int oldi = *numi;
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
			double infect_rate = 1 - pow(1 - rate_infect, count);
			//LOG(LOG_DBG, "snode %d, count %d, rate_infect %f, infect_rate %f", snode, count, rate_infect, infect_rate);
			if (randomd() < infect_rate) {
				infected_new[newi++] = snode;
			}
			sign[snode] = 1;
		}
		if (randomd() < rate_recover) {
			recovered_new[newr++] = inode;
			infected[i] = infected[oldi - 1];
			--oldi;
		}
		else {
			++i;
		}
	}

	for (i = 0; i < newr; ++i) {
		status[recovered_new[i]] = 2;
	}
	for (i = 0; i < newi; ++i) {
		status[infected_new[i]] = 1;
		infected[oldi + i] = infected_new[i];
	}

	*numi = oldi + newi;
}

static void print_status(int maxId, int *status, int step) {
	char fn[100];
	sprintf(fn, "STEP_%d", step + 1);
	FILE *fo = fopen(fn, "w");
	int j;
	for (j = 0; j < maxId + 1; ++j) {
		fprintf(fo, "%d\t%d\n", j, status[j]);
	}
	fclose(fo);
}

void process(NET *net, double rate_infect, double rate_recover, int STEP, int *status) {
	int maxId = net->core[0]->maxId;
	int *degree = net->core[0]->degree;
	int **rela = net->core[0]->rela;

	int *infected = smalloc((maxId + 1) * sizeof(int));
	int numi = 0;

	int i;
	for (i = 0; i < maxId + 1; ++i) {
		if (status[i] == 1) infected[numi++] = i;	
	}

	int *infected_new = smalloc((maxId + 1) * sizeof(int));
	int *recovered_new = smalloc((maxId + 1) * sizeof(int));
	int *sign = smalloc((maxId + 1) * sizeof(int));

	for (i = 0; i < STEP; ++i) {
		process_core(maxId, degree, rela, rate_infect, rate_recover, &numi, infected, status, sign, infected_new, recovered_new);
		print_status(maxId, status, i);
	}

	free(infected);
	free(infected_new);
	free(recovered_new);
	free(sign);
}
