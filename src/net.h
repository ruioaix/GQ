#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "core.h"
#include "attr.h"
#include "utils.h"
#include "linefile.h"

typedef struct CORE NETCORE;

struct NET{
	int size;
	NETCORE **core;
	struct NETATTR attr;
}; 

typedef struct NET NET;

void freeNET(NET *net);
NET *createNET(const struct LineFile * const file, struct NETATTR na);
void printNET(NET *net, char *filename);

#endif
