#pragma once
#include "../lib.h"
#include <string>
// newmtl texture
// Ns 96.078431
// Ka 1.000000 1.000000 1.000000
// Kd 0.640000 0.640000 0.640000
// Ks 0.500000 0.500000 0.500000
// Ke 0.000000 0.000000 0.000000
// Ni 1.000000
// d 1.000000
// illum 2
// map_Kd /home/c/w/glos/logo.jpeg

class objmtl {
public:
  std::string name = "";
  float Ns = 0;
  vec4 Ka{0, 0, 0, 0};
  vec4 Kd{0, 0, 0, 0};
  vec4 Ks{0, 0, 0, 0};
  vec4 Ke{0, 0, 0, 0};
  float Ni = 0;
  float d = 0;
  std::string map_Kd = "";
  id texture_id = 0;
  unsigned texture_size_bytes = 0;
};

#include "objmtls.h"

static objmtls materials;

inline static void objmtls_load_from_file(const char *path) {
  str file = /*takes*/ str_from_file(path);
  objmtl *o = NULL;
  const char *p = file.data;
  while (*p) {
    token t = token_next(&p);
    if (token_starts_with(&t, "#")) {
      p = scan_to_including_newline(p);
      continue;
    }
    if (token_starts_with(&t, "newmtl")) {
      o = new objmtl{};
      token t = token_next(&p);
      const unsigned n = token_size(&t);
      o->name = std::string{t.content, t.content + n};

      objmtls_add(&materials, o);
      continue;
    }
    if (token_equals(&t, "Ns")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      o->Ns = f;
      continue;
    }
    if (token_equals(&t, "Ka") || token_equals(&t, "Kd") ||
        token_equals(&t, "Ks") || token_equals(&t, "Ke")) {
      vec4 v;

      token x = token_next(&p);
      v.x = token_get_float(&x);

      token y = token_next(&p);
      v.y = token_get_float(&y);

      token z = token_next(&p);
      v.z = token_get_float(&z);

      v.w = 0;

      if (token_equals(&t, "Ka")) {
        o->Ka = v;
      } else if (token_equals(&t, "Kd")) {
        o->Kd = v;
      } else if (token_equals(&t, "Ks")) {
        o->Ks = v;
      } else if (token_equals(&t, "Ke")) {
        o->Ke = v;
      }
      continue;
    }

    if (token_equals(&t, "Ni")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      o->Ni = f;
      continue;
    }

    if (token_equals(&t, "d")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      o->d = f;
      continue;
    }

    if (token_equals(&t, "map_Kd")) {
      const token t = token_next(&p);
      const unsigned n = token_size(&t);
      o->map_Kd = std::string{t.content, t.content + n};
      continue;
    }
  }
  str_free(&file);
}
