#pragma once
#include <stdlib.h>

//------------------------------------------------------------------ library

static inline double random() {
	return rand() / RAND_MAX;
}

static inline double random_range(double low, double high) {
	return random() * (high - low) + low;
}

//------------------------------------------------------------------ library
