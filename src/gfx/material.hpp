#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

class material {
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
  unsigned texture_id = 0;
  unsigned texture_size_bytes = 0;
};

static std::vector<material> materials{};

inline static void objmtls_load_from_file(const char *path) {
  std::ifstream file(path);
  if (!file) {
    printf("*** cannot open file '%s'\n", path);
    exit(1);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();
  const char *p = content.c_str();
  while (*p) {
    token t = token_next(&p);
    if (token_starts_with(&t, "#")) {
      p = scan_to_including_newline(p);
      continue;
    }
    if (token_starts_with(&t, "newmtl")) {
      materials.push_back({});
      token t = token_next(&p);
      const unsigned n = token_size(&t);
      materials.back().name = std::string{t.content, t.content + n};
      continue;
    }
    if (token_equals(&t, "Ns")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      materials.back().Ns = f;
      continue;
    }
    if (token_equals(&t, "Ka") || token_equals(&t, "Kd") ||
        token_equals(&t, "Ks") || token_equals(&t, "Ke")) {
      glm::vec4 v;

      token x = token_next(&p);
      v.x = token_get_float(&x);

      token y = token_next(&p);
      v.y = token_get_float(&y);

      token z = token_next(&p);
      v.z = token_get_float(&z);

      if (token_equals(&t, "Ka")) {
        materials.back().Ka = v;
      } else if (token_equals(&t, "Kd")) {
        materials.back().Kd = v;
      } else if (token_equals(&t, "Ks")) {
        materials.back().Ks = v;
      } else if (token_equals(&t, "Ke")) {
        materials.back().Ke = v;
      }
      continue;
    }

    if (token_equals(&t, "Ni")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      materials.back().Ni = f;
      continue;
    }

    if (token_equals(&t, "d")) {
      token t = token_next(&p);
      float f = token_get_float(&t);
      materials.back().d = f;
      continue;
    }

    if (token_equals(&t, "map_Kd")) {
      const token t = token_next(&p);
      const unsigned n = token_size(&t);
      materials.back().map_Kd = std::string{t.content, t.content + n};
      continue;
    }
  }
}

static inline void materials_free() {
  for (const material &m : materials) {
    if (m.texture_id) {
      glDeleteTextures(1, &m.texture_id);
      metrics.buffered_texture_data -= m.texture_size_bytes;
    }
  }
}