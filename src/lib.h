#pragma once
#include <stdlib.h>

//------------------------------------------------------------------ library

static inline double random() {
	return rand() / RAND_MAX;
}

static inline double randomRange(double low, double high) {
	return random() * (high - low) + low;
}

//------------------------------------------------------------------ library
