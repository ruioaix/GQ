#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

struct OPTION {
	char *logfilename;
	unsigned long num_randomseed;

	bool ds_line;
	bool ds_lattice;
	bool ds_crossover;
	bool ds_direct;

	int num_line_node;
	int num_lattice_side;

	double rate_infect;
	double rate_recover;
	double deltat;
	double TI;
	double TR;
	int STEP;
};

struct OPTION *setOPTION(int argc, char **argv);
void freeOPTION(struct OPTION *op);

#endif
