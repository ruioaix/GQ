#include "option.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void display_usage(void) {
	puts("GQ. (May 2015)\n");
	puts("usage: gq [OPTION]\n");
	puts("OPTION common:");
	puts("  -h:  help");
	puts("  -o logfilename:");
	puts("       File the log system will output log to");
	puts("  -s unsignedlongValue: ");
	puts("       Random seed");
	puts("");
	puts("OPTION:");
	puts("  -l:  line dataset");
	puts("  -L:  lattice dataset");
	puts("  -c:  net crossover, if not set, the net will be non-crossover by default");
	puts("  -d:  net directed, if not set, the net will be undirected by default");
	puts("  --num-line-node intValue:  ");
	puts("       the number of the nodes in line dataset");
	puts("  --num-lattice-side intValue:  ");
	puts("       the number of the nodes in one lattice side");
	puts("       so the number of the nodes in lattice is L^2");
	puts("  --rate-infect doubleValue:  ");
	puts("  --rate-recover doubleValue:  ");
	puts("  --deltat doubleValue:  ");
	puts("  --TI doubleValue:  ");
	puts("  --TR doubleValue:  ");
	puts("  --STEP intValue:  ");
	puts("");
	exit(0);
}

static void init_OPTION(struct OPTION *op) {
	op->logfilename = NULL;
	op->num_randomseed= 1;

	op->ds_line = false;
	op->ds_lattice = false;
	op->ds_crossover = false;
	op->ds_direct = false;
	op->num_line_node = 100;
	op->num_lattice_side = 50;
	op->rate_infect = 0.5;
	op->rate_recover = 0.3;
	op->deltat = 0.01;
	op->TI = 0.3;
	op->TR = 1;	
	op->STEP = 10;
}

void freeOPTION(struct OPTION *op) {
	free(op);
}

static void verify_OPTION(struct OPTION *op);
static void info_OPTION(struct OPTION *op);

struct OPTION *setOPTION(int argc, char **argv) {
	if (argc == 1) display_usage();

	struct OPTION *op = smalloc(sizeof(struct OPTION));
	init_OPTION(op);

	static const char *short_options = "ho:s:lLcdC:A:";
	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"log-file", required_argument, NULL, 'o'},
		{"num-randomseed", required_argument, NULL, 's'},

		{"ds-line", no_argument, NULL, 'l'},
		{"num-line-node", required_argument, NULL, 300},
		{"ds-lattice", no_argument, NULL, 'L'},
		{"num-lattice-side", required_argument, NULL, 301},
		{"ds-crossover", no_argument, NULL, 'c'},
		{"ds-direct", no_argument, NULL, 'd'},

		{"rate-infect", required_argument, NULL, 302},
		{"rate-recover", required_argument, NULL, 303},
		{"deltat", required_argument, NULL, 305},
		{"TI", required_argument, NULL, 306},
		{"TR", required_argument, NULL, 307},
		{"STEP", required_argument, NULL, 304},

		{0, 0, 0, 0},
	};
	int longIndex = 0;
	int opt;
	do {
		opt = getopt_long(argc, argv, short_options, long_options, &longIndex);
		if (opt == -1) break;
		switch (opt) {
			case 'h':
				display_usage();
				break;
			case 'o':
				op->logfilename = optarg;
				break;
			case 's':
				op->num_randomseed = strtol(optarg, NULL, 10);
				break;

			case 'l':
				op->ds_line = true;
				break;
			case 300:
				op->num_line_node = strtol(optarg, NULL, 10);
				break;
			case 'L':
				op->ds_lattice = true;
				break;
			case 301:
				op->num_lattice_side = strtol(optarg, NULL, 10);
				break;
			case 'c':
				op->ds_crossover = true;
				break;
			case 'd':
				op->ds_direct = true;
				break;

			case 302:
				op->rate_infect = strtod(optarg, NULL);
				break;
			case 303:
				op->rate_recover = strtod(optarg, NULL);
				break;
			case 305:
				op->deltat = strtod(optarg, NULL);
				break;
			case 306:
				op->TI = strtod(optarg, NULL);
				break;
			case 307:
				op->TR = strtod(optarg, NULL);
				break;
			case 304:
				op->STEP = strtol(optarg, NULL, 10);
				break;

			case '?':
				break;
			default:
				abort();
		}
	} while (1);

	loginit(op->logfilename, getloglevel());
	setRGseed(op->num_randomseed);

	verify_OPTION(op);
	info_OPTION(op);

	return op;
}

static void verify_OPTION(struct OPTION *op) {
	//dataset
	if (!( op->ds_line || op->ds_lattice)) {
		LOG(LOG_FATAL, "no dataset selected, what do you want to calculate?");
	}
	if (op->ds_line && op->ds_lattice) {
		LOG(LOG_FATAL, "Sorry, for now you can only choose one kind of dataset: line or lattice.");
	}

	if (op->ds_line) {
		if (op->num_line_node < 2) {
			LOG(LOG_FATAL, "the nodes number of line dataset is set to %d, are you sure?", op->num_line_node);
		}
	}
	if (op->ds_lattice) {
		if (op->num_lattice_side < 2) {
			LOG(LOG_FATAL, "the side nodes number of lattice dataset is set to %d, are you sure?", op->num_lattice_side);
		}
	}

}

static void info_OPTION(struct OPTION *op) {
	if (op->ds_line) {
		LOG(LOG_INFO, "line dataset: used");
		LOG(LOG_INFO, "line ds, nodes number is %d", op->num_line_node);
	}
	if (op->ds_lattice) {
		LOG(LOG_INFO, "lattice dataset: used");
		LOG(LOG_INFO, "lattice ds, sides number is %d", op->num_lattice_side);
	}

	if (op->ds_crossover) LOG(LOG_INFO, "net will be crossover");
	else LOG(LOG_INFO, "net will be non-crossover");

	if (op->ds_direct) LOG(LOG_INFO, "net will be directed");
	else LOG(LOG_INFO, "net will be undirected");

	LOG(LOG_INFO, "Infect Rate is %f", op->rate_infect);
	LOG(LOG_INFO, "Recover Rate is %f", op->rate_recover);
	LOG(LOG_INFO, "Delta T is %f", op->deltat);
	LOG(LOG_INFO, "TI is %f", op->TI);
	LOG(LOG_INFO, "TR is %f", op->TR);
	LOG(LOG_INFO, "STEP is %d", op->STEP);

}
