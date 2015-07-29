#include "log.h"
#include "option.h"
#include "attr.h"
#include "linefile.h"
#include "dataset.h"
#include "net.h"
#include "preprocess.h"
#include "process.h"

int main(int argc, char **argv) {
	setloglevel(LOG_DBG);
	struct OPTION *op = setOPTION(argc, argv);

	NET *net = preprocess_NET(op);	
	int *status = preprocess_STATUS(net);
	
	int width = 0, height = 0;
	if (op->ds_line) {
		width = op->num_line_node;
		height = 1;
	}
	if (op->ds_lattice) {
		height = width = op->num_lattice_side;
	}
	process(net, op->rate_infect, op->deltat, op->TI, op->TR, op->STEP, status, width, height);

	free(status);
	freeNET(net);
	freeOPTION(op);
	releaselog();
	return 0;
}
