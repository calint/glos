#pragma once

class camera final {
public:
  position eye{0, .5f, 30, 0};
  float mxwvp[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  position lookat{0, 0, 0, 0};
  vec4 up{0, 1, 0, 0};
  float znear = -1;
  float zfar = -2;
  float wi = 1024;
  float hi = 1024;
  int type = 1;

  inline void update_matrix_wvp() {
    mat4 Mt;
    position Pt = eye;
    vec3_negate(&Pt);
    mat4_set_translation(Mt, &Pt);

    mat4 Ml;
    vec4 vec_up = (vec4){0, 1, 0, 0};
    mat4_set_look_at(Ml, &eye, &lookat, &vec_up);

    mat4 Mtl;
    mat4_multiply(Mtl, Ml, Mt);

    mat4 Mp;
    perspective_vertical(Mp, 40, wi / hi, .6f, 3);

    mat4 Mplt;
    mat4_multiply(Mplt, Mp, Mtl);

    mat4_assign(mxwvp, Mplt);
  }
};
