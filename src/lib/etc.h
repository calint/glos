#pragma once

#include <glm/glm.hpp>
#include <math.h>
#include <stdlib.h>
//----------------------------------------------------------------------------

inline static bool is_bit_set(unsigned *bits, int bit_number_starting_at_zero) {
  return *bits & (1 << bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static void set_bit(unsigned *bits, int bit_number_starting_at_zero) {
  *bits |= (1 << bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static void clear_bit(unsigned *bits, int bit_number_starting_at_zero) {
  *bits &= ~(1 << bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static float rnd() { return (float)rand() / (float)RAND_MAX; }

//----------------------------------------------------------------------------

inline static float random_range(float low, float high) {
  return (float)rnd() * (high - low) + low;
}

//----------------------------------------------------------------------------

inline static float bounding_radius_for_scale(glm::vec3 *s) {
  return sqrtf(s->x * s->x + s->y * s->y + s->z * s->z); //?
}

//----------------------------------------------------------------------------
