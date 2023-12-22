#pragma once
#include "material.hpp"
#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string>
#include <vector>

class range {
public:
  unsigned begin = 0;
  unsigned count = 0;
  unsigned material_ix = 0;
};

class glo {
public:
  std::vector<float> vtxbuf{};
  std::vector<range> ranges{};
  unsigned vtxbuf_id = 0;
  std::string name = "";

  inline void free() {
    glDeleteBuffers(1, &vtxbuf_id);
    metrics.buffered_vertex_data -= vtxbuf.size() * sizeof(float);
    metrics.glos_allocated--;
  }

  inline void upload_to_opengl() {
    // upload vertex buffer
    glGenBuffers(1, &vtxbuf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vtxbuf_id);
    glBufferData(GL_ARRAY_BUFFER, (signed)(vtxbuf.size() * sizeof(float)),
                 vtxbuf.data(), GL_STATIC_DRAW);
    metrics.buffered_vertex_data += vtxbuf.size() * sizeof(float);
    for (const range &mr : ranges) {
      material &m = materials.store.at(mr.material_ix);
      if (not m.map_Kd.empty()) { // load texture
        glGenTextures(1, &m.texture_id);
        printf(" * loading texture %u from '%s'\n", m.texture_id,
               m.map_Kd.c_str());
        glBindTexture(GL_TEXTURE_2D, m.texture_id);
        SDL_Surface *surface = IMG_Load(m.map_Kd.c_str());
        if (!surface) {
          printf("!!! could not load image from '%s'\n", m.map_Kd.c_str());
          exit(-1);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        m.texture_size_bytes =
            (unsigned)(surface->w * surface->h * sizeof(uint32_t));
        SDL_FreeSurface(surface);
        metrics.buffered_texture_data += m.texture_size_bytes;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      }
    }
  }

  inline void render(const glm::mat4 &mtx_mw) {
    glUniformMatrix4fv(shaders::umtx_mw, 1, 0, glm::value_ptr(mtx_mw));
    glBindBuffer(GL_ARRAY_BUFFER, vtxbuf_id);
    glVertexAttribPointer(shaders::apos, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          0);
    glVertexAttribPointer(shaders::argba, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)(3 * sizeof(float)));
    glVertexAttribPointer(shaders::anorm, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4) * sizeof(float)));
    glVertexAttribPointer(shaders::atex, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)((3 + 4 + 3) * sizeof(float)));
    for (const range &mr : ranges) {
      const material &m = materials.store.at(mr.material_ix);
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
      metrics.triangles_rendered_prv_frame += mr.count / 3;
      if (m.texture_id) {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
  }

  static /*gives*/ glo *make_from_string(const char **ptr_p) {
    const char *p = *ptr_p;

    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texuv{};

    std::vector<float> vertex_buffer{};
    std::vector<range> mtlrngs{};

    unsigned current_objmtl_ix = 0;
    const char *basedir = "obj/";
    unsigned vtxbufix = 0;
    unsigned prev_vtxbufix = 0;
    int first_o = 1;
    std::string object_name = "";
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
      if (token_equals(&t, "o")) {
        if (first_o) {
          first_o = 0;
          token t = token_next(&p);
          const unsigned n = token_size(&t);
          object_name = std::string{t.content, t.content + n};
          puts(object_name.c_str());
          continue;
        }
        p = t.begin;
        break;
      }
      if (token_equals(&t, "usemtl")) {
        token t = token_next(&p);
        std::string name{t.content, t.content + token_size(&t)};
        bool found = false;
        for (unsigned i = 0; i < materials.store.size(); i++) {
          material &mm = materials.store.at(i);
          if (mm.name == name) {
            current_objmtl_ix = i;
            found = true;
            break;
          }
        }
        if (not found) {
          fprintf(stderr, "\n%s:%u: could not find material\n", __FILE__,
                  __LINE__);
          fprintf(stderr, "        name: '%s'\n\n", name.c_str());
          fprintf(stderr, "\n\n");
          exit(-1);
        }
        if (prev_vtxbufix != vtxbufix) {
          mtlrngs.emplace_back(prev_vtxbufix, vtxbufix, current_objmtl_ix);
          prev_vtxbufix = vtxbufix;
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
        continue;
      }
      if (token_equals(&t, "vt")) {
        token tu = token_next(&p);
        float u = token_get_float(&tu);

        token tv = token_next(&p);
        float v = token_get_float(&tv);

        texuv.emplace_back(u, v);
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
        const material &current_objmtl = materials.store.at(current_objmtl_ix);
        for (int i = 0; i < 3; i++) {
          // position
          token vert1 = token_from_string_additional_delim(p, '/');
          p = vert1.end;
          unsigned ix1 = token_get_uint(&vert1);
          const glm::vec3 &vtx = vertices.at(ix1 - 1);

          // texture index
          token vert2 = token_from_string_additional_delim(p, '/');
          p = vert2.end;
          const unsigned ix2 = token_get_uint(&vert2);
          const glm::vec2 tex = ix2 ? texuv.at(ix2 - 1) : glm::vec2{0, 0};
          // normal
          token vert3 = token_from_string_additional_delim(p, '/');
          p = vert3.end;
          unsigned ix3 = token_get_uint(&vert3);
          const glm::vec3 &norm = normals.at(ix3 - 1);

          // buffer
          vertex_buffer.push_back(vtx.x);
          vertex_buffer.push_back(vtx.y);
          vertex_buffer.push_back(vtx.z);

          vertex_buffer.push_back(current_objmtl.Kd.x);
          vertex_buffer.push_back(current_objmtl.Kd.y);
          vertex_buffer.push_back(current_objmtl.Kd.z);
          vertex_buffer.push_back(1);

          vertex_buffer.push_back(norm.x);
          vertex_buffer.push_back(norm.y);
          vertex_buffer.push_back(norm.z);

          vertex_buffer.push_back(tex.x);
          vertex_buffer.push_back(tex.y);

          vtxbufix++;
        }
        continue;
      }
    }

    mtlrngs.emplace_back(prev_vtxbufix, vtxbufix, current_objmtl_ix);

    printf("    %zu range%c   %lu vertices   %zu B\n", mtlrngs.size(),
           mtlrngs.size() == 1 ? ' ' : 's',
           vertex_buffer.size() / sizeof(vertex),
           vertex_buffer.size() * sizeof(float));

    glo *g = new glo();
    metrics.glos_allocated++;
    *g = (glo){std::move(vertex_buffer), std::move(mtlrngs), 0,
               std::move(object_name)};
    *ptr_p = p;

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
    printf(" * loading object from '%s'\n", path);
    std::ifstream file(path);
    if (!file) {
      printf("*** cannot open file '%s'\n", path);
      exit(1);
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
    exit(-1);
    return NULL;
  }
};

static glos glos{};
