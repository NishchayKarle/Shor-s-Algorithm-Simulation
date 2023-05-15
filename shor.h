#ifndef QALGORITHMS_INCLUDE
#define QALGORITHMS_INCLUDE

#include "quantum.h"

const double pi = acos(-1.0);

void QFT(Register *reg, unsigned int start = 0, unsigned int end = 0);

void IQFT(Register *reg, unsigned int start = 0, unsigned int end = 0);

unsigned int find_Shor_period(unsigned int a, unsigned int N, unsigned int depth_limit = 20);

unsigned int Shor(unsigned int N, unsigned int depth_limit = 200);

#endif