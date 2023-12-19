#pragma once
typedef struct physics {
  position position;
  velocity velocity;
  angle angle;
  angular_velocity angular_velocity;
} physics;
#define physics_def                                                            \
  { vec4_def, vec4_def, vec4_def, vec4_def }
