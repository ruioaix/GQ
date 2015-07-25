#include "log.h"
#include "option.h"
#include "attr.h"
#include "linefile.h"
#include "dataset.h"
#include "net.h"
#include "preprocess.h"
#include "process.h"

int main(int argc, char **argv) {
	setloglevel(LOG_INFO);
	struct OPTION *op = setOPTION(argc, argv);

	NET *net = preprocess_NET(op);	
	int *status = preprocess_STATUS(net);
	
	process(net, op->rate_infect, op->rate_recover, op->STEP, status);

	free(status);
	freeNET(net);
	freeOPTION(op);
	releaselog();
	return 0;
}
