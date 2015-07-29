#ifndef PREPROCESS_H
#define PREPROCESS_H 

#include "net.h"
#include "option.h"

NET *preprocess_NET(struct OPTION *op);
int *preprocess_STATUS(NET *net, struct OPTION *op);

#endif
