#ifndef ATTR_H
#define ATTR_H 

#include "utils.h"

struct NETATTR {
	enum ATTR direct;
	enum ATTR weight;
};

struct DSATTR {
	enum ATTR crossover;
	enum ATTR direct;
};

#endif
