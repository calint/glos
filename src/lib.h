#pragma once
#include <stdlib.h>
//------------------------------------------------------------------ library

static inline double random() {
	double r = (double) rand() / RAND_MAX;
	return r;
}

static inline double random_range(double low, double high) {
	double r = random() * (high - low) + low;
	return r;
}

//------------------------------------------------------------------ library
