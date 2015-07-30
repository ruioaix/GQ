#include "dataset.h"
#include "log.h"
#include <stdlib.h>

/**
 * for Lattice 2d, L is the size of Height and Width of the lattice, N = L*L, L is at least 2.
 * L = 2 is a special case. total linesNum:
 * 		crossover,		direct: 8
 * 		crossvoer,	non_direct: 4
 * 	non_crossover,		direct: 8
 * 	non_crossover, 	non_direct: 4
 * 	
 * when L > 2, total linesNum:
 * 		crossover, 		direct: L*L*4
 * 		crossover, 	non_direct: L*L*2
 * 	non_crossover, 		direct: L*(L-1)*4
 * 	non_crossover,	non_direct: L*(L-1)*2
 */

struct LineFile * lattice2dDS(int L, struct DSATTR dsa) {
	int N = L * L;

	struct LineFile *file = createLF(NULL);
	long linesNum = (L-1)*L*2;
	if (dsa.crossover == CROSSOVER && L != 2) {
		linesNum += 2*L;
	}
	if (dsa.direct == DIRECTED) {
		linesNum *= 2;
	}

	LOG(LOG_INFO, "Generate 2D Lattice, %s, %s, L: %d, N: %d, linesNum: %ld.", whatnetattr(dsa.crossover), whatnetattr(dsa.direct), L, N, linesNum);

	int *i1 = smalloc(linesNum * sizeof(int));
	int *i2 = smalloc(linesNum * sizeof(int));

	int i, j;
	long k = 0;
	for (i = 1; i < L; ++i) {
		for (j = 1; j < L; ++j) {
			int id = i*L + j;
			i1[k]=id;
			i2[k]=id-1;	
			++k;
			if (dsa.direct == DIRECTED) {
				i2[k]=id;
				i1[k]=id-1;	
				++k;
			}
			i1[k]=id;
			i2[k]=id-L;
			++k;
			if (dsa.direct == DIRECTED) {
				i2[k]=id;
				i1[k]=id-L;
				++k;
			}
		}
	}

	for (i = 1; i < L; ++i) {
		i1[k] = i;
		i2[k] = i-1;
		++k;
		if (dsa.direct == DIRECTED) {
			i2[k] = i;
			i1[k] = i-1;
			++k;
		}
		i1[k] = i*L;
		i2[k] = i*L-L;
		++k;
		if (dsa.direct == DIRECTED) {
			i2[k] = i*L;
			i1[k] = i*L-L;
			++k;
		}
	}
	if (dsa.crossover == CROSSOVER && L != 2) {
		for (i = 0; i < L; ++i) {
			i1[k] = i;
			i2[k] = i+(L-1)*L;
			++k;
			if (dsa.direct == DIRECTED) {
				i2[k] = i;
				i1[k] = i+(L-1)*L;
				++k;
			}
			i1[k] = i*L;
			i2[k] = i*L+L-1;
			++k;
			if (dsa.direct == DIRECTED) {
				i2[k] = i*L;
				i1[k] = i*L+L-1;
				++k;
			}
		}
	}
	if (linesNum != k) {
		LOG(LOG_FATAL, "linesNum is %ld, k is %ld, not equal.", linesNum, k);
	}
	LOG(LOG_INFO, "linesNum is %ld", linesNum);

	file->i1 = i1;
	file->i2 = i2;
	file->linesNum = linesNum;
	file->memNum = linesNum;
	return file;
}

/**
 * for 1D Line, N is number of nodes in the line, N must > 1
 *
 * N = 2 is a special case. total linesNum:
 * 		crossover,		direct: 2
 * 		crossover,	non_direct: 1
 * 	non_crossover,		direct: 2
 * 	non_crossover, 	non_direct: 1
 * 	
 * when N > 2, total linesNum:
 * 		crossover, 		direct: N*2
 * 		crossover, 	non_direct: N
 * 	non_crossover, 		direct: (N-1)*2
 * 	non_crossover,	non_direct: N-1
 * 
 */
struct LineFile * lineDS(int N, struct DSATTR dsa) {
	if (N<2) LOG(LOG_FATAL, "N is %d, too small.", N);
	
	struct LineFile *file = createLF(NULL);

	long linesNum = N-1;
	if (dsa.crossover == CROSSOVER && N != 2) {
		linesNum += 1;
	}
	if (dsa.direct == DIRECTED) {
		linesNum *= 2;
	}

	int *i1 = smalloc(linesNum * sizeof(int));
	int *i2 = smalloc(linesNum * sizeof(int));

	int i;
	long k=0;
	for (i = 1; i < N; ++i) {
		i1[k] = i;
		i2[k] = i-1;
		++k;
		if (dsa.direct == DIRECTED) {
			i2[k] = i;
			i1[k] = i-1;
			++k;
		}
	}
	if (dsa.crossover == CROSSOVER && N != 2) {
		i1[k] = 0;
		i2[k] = N - 1;
		++k;
		if (dsa.direct == DIRECTED) {
			i2[k] = 0;
			i1[k] = N - 1;
			++k;
		}
	}

	if (linesNum != k) {
		LOG(LOG_FATAL, "linesNum is %ld, k is %ld, not equal.", linesNum, k);
	}
	LOG(LOG_INFO, "linesNum is %ld", linesNum);

	file->i1 = i1;
	file->i2 = i2;
	file->linesNum = linesNum;
	file->memNum = linesNum;
	return file;
}
