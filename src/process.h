#ifndef PROCESS_H
#define PROCESS_H 

#include "net.h"
void process(NET *net, double rate_infect, double rate_recover, double deltat, int STEP, int *status);

#endif
