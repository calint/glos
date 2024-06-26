#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10
// reviewed: 2024-01-16

namespace glos {

class glob final {
  class range final {
  public:
    // index in vertex buffer where triangle data starts
    uint32_t vertex_begin = 0;
    // number of triangle vertices in the range
    uint32_t vertex_count = 0;
    // index in 'materials'
    uint32_t material_ix = 0;
  };

  GLuint vertex_array_id = 0;
  GLuint vertex_buffer_id = 0;
  std::vector<range> ranges{};
  size_t size_B = 0;

public:
  std::string name{};
  float bounding_radius = 0;
  // planes
  std::vector<glm::vec4> planes_points{}; // x, y, z, 1
  std::vector<glm::vec3> planes_normals{};

  inline glob(char const *obj_path, char const *bounding_planes_path) {
    load_object(obj_path);
    if (bounding_planes_path) {
      load_planes(bounding_planes_path);
    }
  }

  inline auto render(glm::mat4 const &Mmw) const -> void {
    glUniformMatrix4fv(shaders::umtx_mw, 1, GL_FALSE, glm::value_ptr(Mmw));
    glBindVertexArray(vertex_array_id);
    for (range const &rng : ranges) {
      material const &mtl = materials.at(rng.material_ix);
      glActiveTexture(GL_TEXTURE0);
      if (mtl.texture_id) {
        glUniform1i(shaders::utex, 0);
        glBindTexture(GL_TEXTURE_2D, mtl.texture_id);
      } else {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      glDrawArrays(GL_TRIANGLES, GLint(rng.vertex_begin),
                   GLint(rng.vertex_count));
      metrics.rendered_triangles += rng.vertex_count / 3;
      if (mtl.texture_id) {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
    glBindVertexArray(0); //? can be removed?
    ++metrics.rendered_globs;
  }

  inline auto free() const -> void {
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    metrics.buffered_vertex_data -= size_B;
    --metrics.allocated_globs;
  }

private:
  // loads definition and optional bounding planes from 'obj' files
  inline auto load_object(char const *obj_path) -> void {
    printf(" * loading glob from '%s'\n", obj_path);

    std::ifstream const file{obj_path};
    if (not file) {
      throw glos_exception{std::format("cannot open file '{}'", obj_path)};
    }
    std::stringstream buffer{};
    buffer << file.rdbuf();
    std::string const content = buffer.str();
    char const *p = content.c_str();

    std::string base_dir_path{};
    {
      std::string const file_path = obj_path;
      size_t const found = file_path.find_last_of('/');
      if (found != std::string::npos) {
        base_dir_path = file_path.substr(0, found + 1);
        // note: +1 to include '/'
      }
    }

    std::vector<float> vertices{};
    std::vector<glm::vec3> positions{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texture_uv{};

    std::string mtl_path{};
    uint32_t current_material_ix = 0;
    uint32_t vertex_ix = 0;
    uint32_t vertex_ix_prv = 0;
    bool is_first_obj = true;

    while (*p) {
      token const tk = token_next(&p);
      if (token_equals(&tk, "mtllib")) {
        token const t = token_next(&p);
        unsigned const n = token_size(&t);
        std::string const file_name{t.content, t.content + n};
        mtl_path = base_dir_path + file_name;
        materials.load(mtl_path.c_str());
        continue;
      }
      if (token_equals(&tk, "o") and is_first_obj) {
        is_first_obj = false;
        token const t = token_next(&p);
        unsigned const n = token_size(&t);
        name = std::string{t.content, t.content + n};
        printf("   %s\n", name.c_str());
        continue;
      }
      if (token_equals(&tk, "usemtl")) {
        token const t = token_next(&p);
        unsigned const n = token_size(&t);
        std::string const mtl_name = {t.content, t.content + n};
        current_material_ix =
            materials.find_material_ix_or_break(mtl_path, mtl_name);
        if (vertex_ix_prv != vertex_ix) {
          // is not the first 'usemtl' directive
          ranges.emplace_back(vertex_ix_prv, vertex_ix - vertex_ix_prv,
                              current_material_ix);
          vertex_ix_prv = vertex_ix;
        }
        continue;
      }
      if (token_equals(&tk, "v")) {
        token const tx = token_next(&p);
        float const x = token_get_float(&tx);
        token const ty = token_next(&p);
        float const y = token_get_float(&ty);
        token const tz = token_next(&p);
        float const z = token_get_float(&tz);
        positions.emplace_back(x, y, z);

        float const r = glm::length(positions.back());
        if (r > bounding_radius) {
          bounding_radius = r;
        }
        continue;
      }
      if (token_equals(&tk, "vt")) {
        token const tu = token_next(&p);
        float const u = token_get_float(&tu);
        token const tv = token_next(&p);
        float const v = token_get_float(&tv);
        texture_uv.emplace_back(u, v);
        continue;
      }
      if (token_equals(&tk, "vn")) {
        token const tx = token_next(&p);
        float const x = token_get_float(&tx);
        token const ty = token_next(&p);
        float const y = token_get_float(&ty);
        token const tz = token_next(&p);
        float const z = token_get_float(&tz);
        normals.emplace_back(x, y, z);
        continue;
      }
      if (token_equals(&tk, "f")) {
        material const &current_material = materials.at(current_material_ix);
        for (unsigned i = 0; i < 3; ++i) {
          // position
          token const ix1_tkn = token_from_string_additional_delim(p, '/');
          p = ix1_tkn.end;
          unsigned const ix1 = token_get_uint(&ix1_tkn);
          glm::vec3 const &position = positions.at(ix1 - 1);

          // texture
          token const ix2_tkn = token_from_string_additional_delim(p, '/');
          p = ix2_tkn.end;
          unsigned const ix2 = token_get_uint(&ix2_tkn);
          glm::vec2 const &texture =
              ix2 ? texture_uv.at(ix2 - 1) : glm::vec2{0, 0};

          // normal
          token const ix3_tkn = token_from_string_additional_delim(p, '/');
          p = ix3_tkn.end;
          unsigned const ix3 = token_get_uint(&ix3_tkn);
          glm::vec3 const &normal = normals.at(ix3 - 1);

          // add to buffer
          // position
          vertices.push_back(position.x);
          vertices.push_back(position.y);
          vertices.push_back(position.z);
          // color
          vertices.push_back(current_material.Kd.r);
          vertices.push_back(current_material.Kd.g);
          vertices.push_back(current_material.Kd.b);
          vertices.push_back(current_material.d); // opacity
          // normal
          vertices.push_back(normal.x);
          vertices.push_back(normal.y);
          vertices.push_back(normal.z);
          // texture
          vertices.push_back(texture.x);
          vertices.push_back(texture.y);

          ++vertex_ix;
        }
        continue;
      }

      // unknown token

      if (*p != '\0' and *(p - 1) != '\n') {
        p = token_to_including_newline(p);
      }
    }
    // add the last material range
    ranges.emplace_back(vertex_ix_prv, vertex_ix - vertex_ix_prv,
                        current_material_ix);

    unsigned triangles_count = 0;
    for (range const &rng : ranges) {
      triangles_count += rng.vertex_count / 3;
    }

    printf("   %zu range%c   %zu vertices   %u triangles   %zu B   radius: "
           "%0.2f\n",
           ranges.size(), ranges.size() == 1 ? ' ' : 's',
           vertices.size() * sizeof(float) / sizeof(vertex), triangles_count,
           vertices.size() * sizeof(float), bounding_radius);

    ++metrics.allocated_globs;

    //
    // upload to opengl
    //
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);

    // describe the data format
    glEnableVertexAttribArray(shaders::apos);
    glVertexAttribPointer(shaders::apos, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)offsetof(vertex, position));

    glEnableVertexAttribArray(shaders::argba);
    glVertexAttribPointer(shaders::argba, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)offsetof(vertex, color));

    glEnableVertexAttribArray(shaders::anorm);
    glVertexAttribPointer(shaders::anorm, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)offsetof(vertex, normal));

    glEnableVertexAttribArray(shaders::atex);
    glVertexAttribPointer(shaders::atex, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (GLvoid *)offsetof(vertex, texture));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    size_B = vertices.size() * sizeof(float);

    metrics.buffered_vertex_data += size_B;
  }

  inline auto load_planes(char const *path) -> void {
    // load from blender exported 'obj' file
    printf("   * loading planes from '%s'\n", path);
    std::ifstream const file{path};
    if (not file) {
      throw glos_exception{std::format("cannot open file '{}'", path)};
    }
    std::stringstream buffer{};
    buffer << file.rdbuf();
    std::string const content = buffer.str();
    char const *p = content.c_str();

    std::vector<glm::vec4> points{};
    std::vector<glm::vec3> normals{};

    while (*p) {
      token const t = token_next(&p);
      if (token_equals(&t, "v")) {
        token const tx = token_next(&p);
        float const x = token_get_float(&tx);
        token const ty = token_next(&p);
        float const y = token_get_float(&ty);
        token const tz = token_next(&p);
        float const z = token_get_float(&tz);
        points.emplace_back(x, y, z, 1.0f);
        continue;
      }
      if (token_equals(&t, "vn")) {
        token const tx = token_next(&p);
        float const x = token_get_float(&tx);
        token const ty = token_next(&p);
        float const y = token_get_float(&ty);
        token const tz = token_next(&p);
        float const z = token_get_float(&tz);
        normals.emplace_back(x, y, z);
        continue;
      }
      if (token_equals(&t, "f")) {
        // read first vertex in face and create position and normal

        // position
        token const ix1_tkn = token_from_string_additional_delim(p, '/');
        p = ix1_tkn.end;
        unsigned const ix1 = token_get_uint(&ix1_tkn);
        glm::vec4 const &point = points.at(ix1 - 1);

        // texture, skip
        token const ix2_tkn = token_from_string_additional_delim(p, '/');
        p = ix2_tkn.end;

        // normal
        token const ix3_tkn = token_from_string_additional_delim(p, '/');
        p = ix3_tkn.end;
        unsigned const ix3 = token_get_uint(&ix3_tkn);
        glm::vec3 const &normal = normals.at(ix3 - 1);

        planes_points.emplace_back(point);
        planes_normals.emplace_back(normal);

        // ignore the other vertices
        p = token_to_including_newline(p);
        continue;
      }
      // unknown token
      p = token_to_including_newline(p);
    }
    // add points not connected to normals to 'plane_points'
    for (glm::vec4 const &pt : points) {
      if (std::ranges::find(planes_points, pt) == planes_points.cend()) {
        planes_points.emplace_back(pt);
      }
    }
    printf("     %zu planes  %zu points\n", planes_normals.size(),
           planes_points.size());
  }
};

class globs final {
  std::vector<glob> store{};

public:
  inline auto init() -> void {}

  inline auto free() -> void {
    for (glob const &g : store) {
      g.free();
    }
  }

  inline auto load(char const *obj_path,
                   char const *bounding_planes_path) -> uint32_t {

    store.emplace_back(obj_path, bounding_planes_path);
    return uint32_t(store.size() - 1);
  }

  inline auto at(uint32_t const ix) const -> glob const & {
    return store.at(ix);
  }
};

static globs globs{};
} // namespace glos