#pragma once
#include "../lib.h"
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
  str name = str_def;
  float Ns = 0;
  vec4 Ka{0, 0, 0, 0};
  vec4 Kd{0, 0, 0, 0};
  vec4 Ks{0, 0, 0, 0};
  vec4 Ke{0, 0, 0, 0};
  float Ni = 0;
  float d = 0;
  str map_Kd = str_def;
  id texture_id = 0;
  unsigned texture_size_bytes = 0;
};
#define objmtl_def                                                             \
  (objmtl) {                                                                   \
    str_def, 0, vec4_def, vec4_def, vec4_def, vec4_def, 0, 0, str_def, 0, 0    \
  }

inline static void objmtl_free(objmtl *o) {
  str_free(&o->name);
  str_free(&o->map_Kd);
}

#include "objmtls.h"

static objmtls materials;

inline static objmtl *objmtl_alloc() {
  objmtl *o = (objmtl *)malloc(sizeof(objmtl));
  if (!o) {
    perror("\nout of memory while allocating a objmtl\n");
    fprintf(stderr, "\n     %s %d\n", __FILE__, __LINE__);
    exit(-1);
  }
  *o = objmtl_def;
  return o;
}

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
      o = objmtl_alloc();
      token t = token_next(&p);
      unsigned n = token_size(&t);
      str_add_list(&o->name, t.content, n);
      str_add(&o->name, 0);
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
      token t = token_next(&p);
      unsigned n = token_size(&t);
      str_add_list(&o->map_Kd, t.content, n);
      str_add(&o->map_Kd, 0);
      continue;
    }
  }
  str_free(&file);
}
