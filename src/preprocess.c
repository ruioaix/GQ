#include "preprocess.h"
#include "dataset.h"
#include "random.h"

NET *preprocess_NET(struct OPTION *op) {
	struct DSATTR dsa;
	if (op->ds_crossover) dsa.crossover = CROSSOVER;
	else dsa.crossover = NONCROSSOVER;	
	if (op->ds_direct) dsa.direct = DIRECTED;
	else dsa.direct = UNDIRECTED;

	struct LineFile * lf;
	if (op->ds_lattice) lf = lattice2dDS(op->num_lattice_side, dsa);
	if (op->ds_line) lf = lineDS(op->num_line_node, dsa);


	NET *net = createNET(lf);

	freeLF(lf);
		
	return net;
}

// 0:S, 1:I, 2:R
int *preprocess_STATUS(NET *net, struct OPTION *op) {
	int *status = scalloc(net->maxId + 1, sizeof(int));
	
	if (op->ds_line) {
		if (op->init_single) {
			status[net->maxId/2] = 1;
		}
		else if (op->init_random) {
			if (op->init_random_frequency != -1) {
				int i;
				for (i = 0; i < net->maxId / op->init_random_frequency; ++i) {
					status[random()%net->maxId] = 1;
				}
			}
			if (op->init_random_num != -1) {
				int i;
				for (i = 0; i < op->init_random_num; ++i) {
					status[random()%net->maxId] = 1;
				}
			}
		}
	}
	else if (op->ds_lattice) {
		if (op->init_single) {
			status[net->maxId/2 + op->num_lattice_side/2] = 1;
		}
		else if (op->init_random) {
			if (op->init_random_frequency != -1) {
				int i;
				for (i = 0; i < net->maxId / op->init_random_frequency; ++i) {
					status[random()%net->maxId] = 1;
				}
			}
			if (op->init_random_num != -1) {
				int i;
				for (i = 0; i < op->init_random_num; ++i) {
					status[random()%net->maxId] = 1;
				}
			}
		}
	}
	return status;
}
