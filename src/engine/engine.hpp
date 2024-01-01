#pragma once

#include <iostream>
//
#include "metrics.hpp"
//
#include "net.hpp"
//
#include "sdl.hpp"
//
#include "window.hpp"
//
#include "shader.hpp"
//
#include "camera.hpp"
//
#include "material.hpp"
//
#include "glo.hpp"
//

namespace glos {
inline static void debug_render_wcs_line(const glm::vec3 &from_wcs,
                                         const glm::vec3 &to_wcs,
                                         const glm::vec3 &color);

class frame_context {
public:
  float dt = 0;
  unsigned tick = 0;
};
} // namespace glos

// application interface
void application_init();
void application_at_frame_end(glos::frame_context const &fc);
void application_free();

#include "object.hpp"
//
#include "grid.hpp"
//
#include "net.hpp"
//
#include "net_server.hpp"

namespace glos {
static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} background_color = {0, 0, 0};

inline glm::vec3 ambient_light = glm::normalize(glm::vec3{0, 1, 1});

inline object *camera_follow_object = nullptr;

inline bool net_enabled = false;

class engine final {
public:
  int shader_program_render_line = 0;
  int shader_program_ix = 0;
  int shader_program_ix_prev = shader_program_ix;

  inline void init() {
    if (net_enabled) {
      net.init();
    }
    metrics.init();
    sdl.init();
    window.init();
    shaders.init();
    textures.init();
    materials.init();
    glos.init();
    objects.init();
    grid.init();

    {
      const char *vtx = R"(
  #version 330 core
  uniform mat4 umtx_wvp; // world-to-view-to-projection
  layout(location = 0) in vec3 apos; // world coordinates
  void main() {
	  gl_Position = umtx_wvp * vec4(apos, 1);
  }
  )";

      const char *frag = R"(
  #version 330 core
  uniform vec3 ucolor;
  out vec4 rgba;
  void main() {
    rgba = vec4(ucolor, 1);
  }
)";
      shader_program_render_line = shaders.load_program_from_source(vtx, frag);
    }
  }

  inline void free() {
    grid.free();
    objects.free();
    glos.free();
    materials.free();
    textures.free();
    shaders.free();
    window.free();
    sdl.free();
    if (net_enabled) {
      net.free();
    }

    metrics.print(stderr);

    metrics.free();
  }
};

inline engine engine{};

// for debugging
inline static void debug_render_wcs_line(const glm::vec3 &from_wcs,
                                         const glm::vec3 &to_wcs,
                                         const glm::vec3 &color) {
  // std::cout << glm::to_string(from_wcs) << " to " << glm::to_string(to_wcs)
  //           << std::endl;

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  const glm::vec3 line_vertices[]{from_wcs, to_wcs};
  glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  camera.update_matrix_wvp();

  shaders.use_program(engine.shader_program_render_line);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(camera.Mvp));
  glUniform3fv(1, 1, glm::value_ptr(color));

  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  shaders.use_program(engine.shader_program_ix);
}
} // namespace glos