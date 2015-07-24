/**
 * this file is used to generate all kinds of data set.
 * the data set will be represented with struct LineFile or just print into file.
 */
#ifndef DATASET_H
#define DATASET_H

#include "linefile.h"
#include "attr.h"

struct LineFile * lattice2dDS(int L, struct DSATTR dsa);
struct LineFile * lineDS(int N, struct DSATTR dsa);

#endif
