#include "preprocess.h"
#include "dataset.h"

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
int *preprocess_STATUS(NET *net) {
	int *status = scalloc(net->maxId + 1, sizeof(int));
	status[0] = 1;
	return status;
}
