#pragma once
// reviewed: 2023-12-22

#include "token.h"

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <vector>

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
  std::string name = "";
  float Ns = 0;
  glm::vec4 Ka{0, 0, 0, 0};
  glm::vec4 Kd{0, 0, 0, 0};
  glm::vec4 Ks{0, 0, 0, 0};
  glm::vec4 Ke{0, 0, 0, 0};
  float Ni = 0;
  float d = 0;
  std::string map_Kd = "";
  GLuint texture_id = 0;
  int texture_size_bytes = 0;
};

class materials final {
public:
  std::vector<material> store{};

  inline void init() {}
  inline void free() {
    for (const material &m : store) {
      if (m.texture_id) {
        glDeleteTextures(1, &m.texture_id);
        metrics.buffered_texture_data -= m.texture_size_bytes;
      }
    }
  }

  inline void load(char const *path) {
    std::ifstream file(path);
    if (!file) {
      printf("*** cannot open file '%s'\n", path);
      std::abort();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    const char *p = content.c_str();
    while (*p) {
      token tk = token_next(&p);
      if (token_starts_with(&tk, "#")) {
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_starts_with(&tk, "newmtl")) {
        store.push_back({});
        token t = token_next(&p);
        const unsigned n = token_size(&t);
        store.back().name = std::string{t.content, t.content + n};
        continue;
      }
      if (token_equals(&tk, "Ns")) {
        token t = token_next(&p);
        float f = token_get_float(&t);
        store.back().Ns = f;
        continue;
      }
      if (token_equals(&tk, "Ka") || token_equals(&tk, "Kd") ||
          token_equals(&tk, "Ks") || token_equals(&tk, "Ke")) {
        glm::vec4 v{};
        token x = token_next(&p);
        v.x = token_get_float(&x);
        token y = token_next(&p);
        v.y = token_get_float(&y);
        token z = token_next(&p);
        v.z = token_get_float(&z);
        if (token_equals(&tk, "Ka")) {
          store.back().Ka = v;
        } else if (token_equals(&tk, "Kd")) {
          store.back().Kd = v;
        } else if (token_equals(&tk, "Ks")) {
          store.back().Ks = v;
        } else if (token_equals(&tk, "Ke")) {
          store.back().Ke = v;
        }
        continue;
      }

      if (token_equals(&tk, "Ni")) {
        token t = token_next(&p);
        float f = token_get_float(&t);
        store.back().Ni = f;
        continue;
      }

      if (token_equals(&tk, "d")) {
        token t = token_next(&p);
        float f = token_get_float(&t);
        store.back().d = f;
        continue;
      }

      if (token_equals(&tk, "map_Kd")) {
        const token t = token_next(&p);
        const unsigned n = token_size(&t);
        store.back().map_Kd = std::string{t.content, t.content + n};
        continue;
      }
    }
  }
};

static materials materials{};
} // namespace glos