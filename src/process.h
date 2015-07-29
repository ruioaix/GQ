#ifndef PROCESS_H
#define PROCESS_H 

#include "net.h"
void process(NET *net, double rate_infect, double deltat, double TI, double TR, int STEP, int *status);

#endif
