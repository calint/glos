#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06

#include "token.h"

namespace glos {

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

class material final {
public:
  std::string path = "";
  std::string name = "";
  float Ns = 0;
  glm::vec3 Ka{0};
  glm::vec3 Kd{0};
  glm::vec3 Ks{0};
  glm::vec3 Ke{0};
  float Ni = 0;
  float d = 0;
  std::string map_Kd = "";
  GLuint texture_id = 0;
};

class materials final {
public:
  std::vector<material> store{};

  inline void init() {}

  inline void free() { store.clear(); }

  inline void load(char const *path) {
    printf("   * loading materials from '%s'\n", path);
    std::ifstream file(path);
    if (not file) {
      fprintf(stderr, "\n%s:%d: cannot open file '%s'\n", __FILE__, __LINE__,
              path);
      fflush(stderr);
      std::abort();
    }
    std::stringstream buffer{};
    buffer << file.rdbuf();
    std::string const content = buffer.str();
    char const *p = content.c_str();
    while (*p) {
      token const t = token_next(&p);
      if (token_starts_with(&t, "#")) {
        // comment line
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_starts_with(&t, "newmtl")) {
        store.push_back({});
        token const tk = token_next(&p);
        unsigned const n = token_size(&tk);
        store.back().path = std::string{path};
        store.back().name = std::string{tk.content, tk.content + n};
        printf("     * %s\n", store.back().name.c_str());
        continue;
      }
      if (token_equals(&t, "Ns")) {
        token const tk = token_next(&p);
        store.back().Ns = token_get_float(&tk);
        continue;
      }
      if (token_equals(&t, "Ka") or token_equals(&t, "Kd") or
          token_equals(&t, "Ks") or token_equals(&t, "Ke")) {
        glm::vec3 v{};
        token const x = token_next(&p);
        v.x = token_get_float(&x);
        token const y = token_next(&p);
        v.y = token_get_float(&y);
        token const z = token_next(&p);
        v.z = token_get_float(&z);
        if (token_equals(&t, "Ka")) {
          store.back().Ka = v;
        } else if (token_equals(&t, "Kd")) {
          store.back().Kd = v;
        } else if (token_equals(&t, "Ks")) {
          store.back().Ks = v;
        } else if (token_equals(&t, "Ke")) {
          store.back().Ke = v;
        }
        continue;
      }

      if (token_equals(&t, "Ni")) {
        token const tk = token_next(&p);
        store.back().Ni = token_get_float(&tk);
        continue;
      }

      if (token_equals(&t, "d")) {
        token const tk = token_next(&p);
        store.back().d = token_get_float(&tk);
        continue;
      }

      if (token_equals(&t, "map_Kd")) {
        // texture path
        token const tk = token_next(&p);
        unsigned const n = token_size(&tk);
        store.back().map_Kd = std::string{tk.content, tk.content + n};
        continue;
      }
    }
  }
};

inline materials materials{};
} // namespace glos