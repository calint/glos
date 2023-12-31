#pragma once
// reviewed: 2023-12-22

#include "material.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace glos {
class glo final {
  class range final {
  public:
    int vertex_begin = 0; // index in vertex buffer where triangle data starts
    int vertex_count = 0; // number of triangle vertices in the range
    int material_ix = 0;  // index in 'materials'
  };

public:
  std::string name = "";
  float bounding_radius = 0;
  GLuint vertex_array_id = 0;
  GLuint vertex_buffer_id = 0;
  std::vector<range> ranges{};
  int size_B = 0;
  // planes
  std::vector<glm::vec3> planes_points{};
  std::vector<glm::vec3> planes_normals{};

  inline void free() {
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    metrics.buffered_vertex_data -= size_t(size_B);
    metrics.allocated_glos--;
  }

  inline void render(glm::mat4 const &mtx_mw) const {
    glUniformMatrix4fv(shaders::umtx_mw, 1, GL_FALSE, glm::value_ptr(mtx_mw));
    glBindVertexArray(vertex_array_id);
    for (const range &mr : ranges) {
      const material &m = materials.store.at(unsigned(mr.material_ix));
      glActiveTexture(GL_TEXTURE0);
      if (m.texture_id) {
        glUniform1i(shaders::utex, 0);
        glBindTexture(GL_TEXTURE_2D, m.texture_id);
      } else {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      glDrawArrays(GL_TRIANGLES, mr.vertex_begin, mr.vertex_count);
      metrics.rendered_triangles += mr.vertex_count / 3;
      if (m.texture_id) {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
    glBindVertexArray(0);
    metrics.rendered_glos++;
  }

  // loads definition and optional bounding planes from 'obj' files
  inline void load(char const *obj_path, char const *bounding_planes_path) {
    printf(" * loading glo from '%s'\n", obj_path);
    std::ifstream file(obj_path);
    if (!file) {
      printf("!!! cannot open file '%s'\n", obj_path);
      std::abort();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    const char *p = content.c_str();

    std::vector<float> vertex_buffer{};
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texture_uv{};

    unsigned current_object_material_ix = 0;
    const char *basedir = "obj/";
    unsigned vertex_buffer_ix = 0;
    unsigned vertex_buffer_ix_prv = 0;
    int first_o = 1;
    std::string object_name = "";

    while (*p) {
      token tk = token_next(&p);
      if (token_starts_with(&tk, "#")) {
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_equals(&tk, "mtllib")) {
        token t = token_next(&p);
        std::string base{basedir};
        std::string file_name{t.content, t.content + token_size(&t)};
        std::string path = base + file_name;
        materials.load(path.c_str());
        continue;
      }
      if (token_equals(&tk, "o") and first_o) {
        first_o = false;
        token t = token_next(&p);
        const unsigned n = token_size(&t);
        object_name = std::string{t.content, t.content + n};
        printf("     %s\n", object_name.c_str());
        continue;
      }
      if (token_equals(&tk, "usemtl")) {
        token t = token_next(&p);
        name = {t.content, t.content + token_size(&t)};
        bool found = false;
        for (unsigned i = 0; i < materials.store.size(); i++) {
          material &mtl = materials.store.at(i);
          if (mtl.name == name) {
            current_object_material_ix = i;
            found = true;
            break;
          }
        }
        if (not found) {
          fprintf(stderr, "\n%s:%u: could not find material\n", __FILE__,
                  __LINE__);
          fprintf(stderr, "        name: '%s'\n\n", name.c_str());
          fprintf(stderr, "\n\n");
          std::abort();
        }
        if (vertex_buffer_ix_prv != vertex_buffer_ix) {
          ranges.emplace_back(vertex_buffer_ix_prv,
                              vertex_buffer_ix - vertex_buffer_ix_prv,
                              current_object_material_ix);
          vertex_buffer_ix_prv = vertex_buffer_ix;
        }
      }
      if (token_equals(&tk, "s")) {
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_equals(&tk, "v")) {
        token tx = token_next(&p);
        float x = token_get_float(&tx);
        token ty = token_next(&p);
        float y = token_get_float(&ty);
        token tz = token_next(&p);
        float z = token_get_float(&tz);
        vertices.emplace_back(x, y, z);

        const float r = glm::length(vertices.back());
        if (r > bounding_radius) {
          bounding_radius = r;
        }
        continue;
      }
      if (token_equals(&tk, "vt")) {
        token tu = token_next(&p);
        float u = token_get_float(&tu);
        token tv = token_next(&p);
        float v = token_get_float(&tv);
        texture_uv.emplace_back(u, v);
        continue;
      }
      if (token_equals(&tk, "vn")) {
        token tx = token_next(&p);
        float x = token_get_float(&tx);
        token ty = token_next(&p);
        float y = token_get_float(&ty);
        token tz = token_next(&p);
        float z = token_get_float(&tz);
        normals.emplace_back(x, y, z);
        continue;
      }

      if (token_equals(&tk, "f")) {
        const material &current_object_material =
            materials.store.at(current_object_material_ix);
        for (unsigned i = 0; i < 3; i++) {
          // position
          token ix1_tkn = token_from_string_additional_delim(p, '/');
          p = ix1_tkn.end;
          const unsigned ix1 = token_get_uint(&ix1_tkn);
          const glm::vec3 &vertex = vertices.at(ix1 - 1);

          // texture
          token ix2_tkn = token_from_string_additional_delim(p, '/');
          p = ix2_tkn.end;
          const unsigned ix2 = token_get_uint(&ix2_tkn);
          const glm::vec2 &texture =
              ix2 ? texture_uv.at(ix2 - 1) : glm::vec2{0, 0};

          // normal
          token ix3_tkn = token_from_string_additional_delim(p, '/');
          p = ix3_tkn.end;
          const unsigned ix3 = token_get_uint(&ix3_tkn);
          const glm::vec3 &normal = normals.at(ix3 - 1);

          // add to buffer
          // position
          vertex_buffer.push_back(vertex.x);
          vertex_buffer.push_back(vertex.y);
          vertex_buffer.push_back(vertex.z);
          // color
          vertex_buffer.push_back(current_object_material.Kd.x);
          vertex_buffer.push_back(current_object_material.Kd.y);
          vertex_buffer.push_back(current_object_material.Kd.z);
          vertex_buffer.push_back(1); // opacity
          // normal
          vertex_buffer.push_back(normal.x);
          vertex_buffer.push_back(normal.y);
          vertex_buffer.push_back(normal.z);
          // texture
          vertex_buffer.push_back(texture.x);
          vertex_buffer.push_back(texture.y);

          vertex_buffer_ix++;
        }
        continue;
      }
    }
    // add the last material range
    ranges.emplace_back(vertex_buffer_ix_prv,
                        vertex_buffer_ix - vertex_buffer_ix_prv,
                        current_object_material_ix);

    int ntriangles = 0;
    for (const range &r : ranges) {
      ntriangles += r.vertex_count / 3;
    }

    printf("     %zu range%c   %lu vertices   %d triangles   %zu B   radius: "
           "%0.2f\n",
           ranges.size(), ranges.size() == 1 ? ' ' : 's',
           vertex_buffer.size() / sizeof(vertex), ntriangles,
           vertex_buffer.size() * sizeof(float), bounding_radius);

    metrics.allocated_glos++;

    if (bounding_planes_path) {
      load_planes(bounding_planes_path);
    }

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER,
                 GLsizeiptr(vertex_buffer.size() * sizeof(float)),
                 vertex_buffer.data(), GL_STATIC_DRAW);

    // describe the data format
    glEnableVertexAttribArray(shaders::apos);
    glVertexAttribPointer(shaders::apos, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          0);

    glEnableVertexAttribArray(shaders::argba);
    glVertexAttribPointer(shaders::argba, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)(3 * sizeof(float)));

    glEnableVertexAttribArray(shaders::anorm);
    glVertexAttribPointer(shaders::anorm, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4) * sizeof(float)));

    glEnableVertexAttribArray(shaders::atex);
    glVertexAttribPointer(shaders::atex, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4 + 3) * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    size_B = int(vertex_buffer.size() * sizeof(float));

    metrics.buffered_vertex_data += size_t(size_B);

    for (range const &mtlrng : ranges) {
      material &mtl = materials.store.at(unsigned(mtlrng.material_ix));
      if (not mtl.map_Kd.empty()) {
        mtl.texture_id = textures.get_id_or_load(mtl.map_Kd);
        textures.get_id_or_load(mtl.map_Kd);
      }
    }
  }

private:
  inline void load_planes(char const *path) {
    // load from blender exported 'obj' file
    printf(" * loading planes from '%s'\n", path);
    std::ifstream file(path);
    if (!file) {
      printf("!!! cannot open file '%s'\n", path);
      std::abort();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    const char *p = content.c_str();

    std::vector<glm::vec3> positions{};
    std::vector<glm::vec3> normals{};

    while (*p) {
      token t = token_next(&p);
      if (token_equals(&t, "v")) {
        token tx = token_next(&p);
        float x = token_get_float(&tx);
        token ty = token_next(&p);
        float y = token_get_float(&ty);
        token tz = token_next(&p);
        float z = token_get_float(&tz);
        positions.emplace_back(x, y, z);
        continue;
      }
      if (token_equals(&t, "vn")) {
        token tx = token_next(&p);
        float x = token_get_float(&tx);
        token ty = token_next(&p);
        float y = token_get_float(&ty);
        token tz = token_next(&p);
        float z = token_get_float(&tz);
        normals.emplace_back(x, y, z);
        continue;
      }
      if (token_equals(&t, "f")) {
        // read first vertex in face and create position and normal
        // position
        token ix1_tkn = token_from_string_additional_delim(p, '/');
        p = ix1_tkn.end;
        const unsigned ix1 = token_get_uint(&ix1_tkn);
        const glm::vec3 &position = positions.at(ix1 - 1);

        // texture, skip
        token ix2_tkn = token_from_string_additional_delim(p, '/');
        p = ix2_tkn.end;

        // normal
        token ix3_tkn = token_from_string_additional_delim(p, '/');
        p = ix3_tkn.end;
        const unsigned ix3 = token_get_uint(&ix3_tkn);
        const glm::vec3 &normal = normals.at(ix3 - 1);

        planes_points.emplace_back(position);
        planes_normals.emplace_back(normal);

        // ignore the other vertices
        p = scan_to_including_newline(p);
        continue;
      }
      // unknown token
      p = scan_to_including_newline(p);
    }
    printf("     %zu planes\n", planes_normals.size());
    // const size_t n = planes_points.size();
    // for (unsigned i = 0; i < n; i++) {
    //   printf(" p: %s  n: %s\n", glm::to_string(planes_points[i]).c_str(),
    //          glm::to_string(planes_normals[i]).c_str());
    // }
  }
};

class glos final {
public:
  std::vector<glo> store{};

  inline void init() {}

  inline void free() {
    for (glo &g : store) {
      g.free();
    }
  }

  inline auto load(char const *obj_path, char const *bounding_planes_path)
      -> int {
    glo g{};
    g.load(obj_path, bounding_planes_path);
    store.push_back(std::move(g));
    return int(store.size() - 1);
  }

  inline auto get_by_index(int const ix) const -> glo const & {
    return store.at(size_t(ix));
  }
};

static glos glos{};
} // namespace glos