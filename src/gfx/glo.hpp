#pragma once
// reviewed: 2023-12-22

#include "material.hpp"
#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string>
#include <vector>

class range final {
public:
  int begin = 0;
  int count = 0;
  int material_ix = 0;
};

class glo final {
public:
  std::string name = "";
  std::vector<float> vertex_buffer{};
  std::vector<range> ranges{};
  float bounding_radius = 0;
  GLuint vertex_buffer_id = 0;

  inline void free() {
    glDeleteBuffers(1, &vertex_buffer_id);
    metrics.buffered_vertex_data -= vertex_buffer.size() * sizeof(float);
    metrics.allocated_glos--;
  }

  inline void upload_to_opengl() {
    // upload vertex buffer
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER,
                 (signed)(vertex_buffer.size() * sizeof(float)),
                 vertex_buffer.data(), GL_STATIC_DRAW);
    metrics.buffered_vertex_data += vertex_buffer.size() * sizeof(float);
    for (const range &mtlrng : ranges) {
      material &mtl = materials.store.at(unsigned(mtlrng.material_ix));
      if (not mtl.map_Kd.empty()) {
        // load texture
        glGenTextures(1, &mtl.texture_id);
        printf(" * loading texture %u from '%s'\n", mtl.texture_id,
               mtl.map_Kd.c_str());
        glBindTexture(GL_TEXTURE_2D, mtl.texture_id);
        SDL_Surface *surface = IMG_Load(mtl.map_Kd.c_str());
        if (!surface) {
          printf("!!! could not load image from '%s'\n", mtl.map_Kd.c_str());
          std::abort();
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        mtl.texture_size_bytes =
            surface->w * surface->h * int(sizeof(uint32_t));
        SDL_FreeSurface(surface);
        metrics.buffered_texture_data += mtl.texture_size_bytes;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      }
    }
  }

  inline void render(const glm::mat4 &mtx_mw) {
    glUniformMatrix4fv(shaders::umtx_mw, 1, GL_FALSE, glm::value_ptr(mtx_mw));
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(shaders::apos, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          0);
    glVertexAttribPointer(shaders::argba, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)(3 * sizeof(float)));
    glVertexAttribPointer(shaders::anorm, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4) * sizeof(float)));
    glVertexAttribPointer(shaders::atex, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4 + 3) * sizeof(float)));
    for (const range &mr : ranges) {
      const material &m = materials.store.at(unsigned(mr.material_ix));
      if (m.texture_id) {
        glUniform1i(shaders::utex, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.texture_id);
        glEnableVertexAttribArray(shaders::atex);
      } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableVertexAttribArray(shaders::atex);
      }
      glDrawArrays(GL_TRIANGLES, (signed)mr.begin, (signed)mr.count);
      metrics.rendered_triangles += mr.count / 3;
      if (m.texture_id) {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
    metrics.rendered_glos++;
  }

  static /*gives*/ glo *make_from_string(const char **ptr_p) {
    const char *p = *ptr_p;

    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texture_uv{};

    std::vector<float> vertex_buffer{};
    std::vector<range> material_ranges{};

    unsigned current_object_material_ix = 0;
    const char *basedir = "obj/";
    unsigned vertex_buffer_ix = 0;
    unsigned prev_vertex_buffer_ix = 0;
    int first_o = 1;
    std::string object_name = "";
    float bounding_radius = 0;

    while (*p) {
      token t = token_next(&p);
      if (token_starts_with(&t, "#")) {
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_equals(&t, "mtllib")) {
        token t = token_next(&p);
        std::string base{basedir};
        std::string file_name{t.content, t.content + token_size(&t)};
        std::string path = base + file_name;
        materials.load_from_file(path.c_str());
        continue;
      }
      if (token_equals(&t, "o") and first_o) {
        first_o = false;
        token t = token_next(&p);
        const unsigned n = token_size(&t);
        object_name = std::string{t.content, t.content + n};
        printf("   %s\n", object_name.c_str());
        continue;
      }
      if (token_equals(&t, "usemtl")) {
        token t = token_next(&p);
        std::string name{t.content, t.content + token_size(&t)};
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
        if (prev_vertex_buffer_ix != vertex_buffer_ix) {
          material_ranges.emplace_back(prev_vertex_buffer_ix, vertex_buffer_ix,
                                       current_object_material_ix);
          prev_vertex_buffer_ix = vertex_buffer_ix;
        }
      }
      if (token_equals(&t, "s")) {
        p = scan_to_including_newline(p);
        continue;
      }
      if (token_equals(&t, "v")) {
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
      if (token_equals(&t, "vt")) {
        token tu = token_next(&p);
        float u = token_get_float(&tu);
        token tv = token_next(&p);
        float v = token_get_float(&tv);
        texture_uv.emplace_back(u, v);
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
          // vertex
          vertex_buffer.push_back(vertex.x);
          vertex_buffer.push_back(vertex.y);
          vertex_buffer.push_back(vertex.z);
          // color
          vertex_buffer.push_back(current_object_material.Kd.x);
          vertex_buffer.push_back(current_object_material.Kd.y);
          vertex_buffer.push_back(current_object_material.Kd.z);
          vertex_buffer.push_back(1);
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
    material_ranges.emplace_back(prev_vertex_buffer_ix, vertex_buffer_ix,
                                 current_object_material_ix);

    printf("   %zu range%c  %lu vertices   %zu B   radius: %0.3f\n",
           material_ranges.size(), material_ranges.size() == 1 ? ' ' : 's',
           vertex_buffer.size() / sizeof(vertex),
           vertex_buffer.size() * sizeof(float), bounding_radius);

    glo *g = new glo{std::move(object_name), std::move(vertex_buffer),
                     std::move(material_ranges), bounding_radius};
    metrics.allocated_glos++;
    return g;
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

  inline void load_from_file(const char *path) {
    printf(" * loading glo from '%s'\n", path);
    std::ifstream file(path);
    if (!file) {
      printf("!!! cannot open file '%s'\n", path);
      std::abort();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    const char *p = content.c_str();
    glo *g = /*takes*/ glo::make_from_string(&p);
    g->upload_to_opengl();
    store.push_back(std::move(*g));
    delete g;
  }

  inline glo *find_by_name(const char *name) {
    for (glo &g : store) {
      if (!strcmp(name, g.name.c_str())) {
        return &g;
      }
    }
    fprintf(stderr, "\n%s:%u: could not find glo '%s' \n", __FILE__, __LINE__,
            name);
    fprintf(stderr, "\n\n");
    std::abort();
  }
};

static glos glos{};
