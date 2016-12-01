#pragma once
#include <stdlib.h>

//------------------------------------------------------------------ library

static inline float random() {
	return rand() / (float) RAND_MAX;
}

static inline int randomRange(int low, int high) {
	int range = high - low;
	return (int) (random() * range) + low;
}

//------------------------------------------------------------------ library
