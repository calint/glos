#pragma once

#include <condition_variable>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <mutex>

// inclusion order relevant
#include "../application/configuration.hpp"
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
#include "glob.hpp"
//

namespace glos {
inline static void debug_render_wcs_line(const glm::vec3 &from_wcs,
                                         const glm::vec3 &to_wcs,
                                         const glm::vec3 &color);

// information about the current frame
class frame_context {
public:
  uint32_t frame_num = 0; // frame number (will rollover)
  uint32_t ms = 0;        // current time since start in milliseconds
  float dt = 0;           // frame delta time in seconds (time step)
};

inline frame_context frame_context{};

} // namespace glos

// application interface
void application_init();
void application_at_frame_end();
void application_free();

#include "object.hpp"
//
#include "grid.hpp"
//
#include "net.hpp"
//
#include "net_server.hpp"
//

namespace glos {
static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} background_color = {0, 0, 0};

inline glm::vec3 ambient_light = glm::normalize(glm::vec3{0, 1, 1});

inline object *camera_follow_object = nullptr;

class engine final {
public:
  int shader_program_render_line = 0;
  int shader_program_ix = 0;
  int shader_program_ix_prev = shader_program_ix;

  inline void init() {
    // set random number generator seed for deterministic behaviour
    srand(0);

    net.init();
    metrics.init();
    sdl.init();
    window.init();
    shaders.init();
    textures.init();
    materials.init();
    globs.init();
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

    printf("class sizes:\n");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9s :\n", "class", "bytes");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9ld :\n", "object", sizeof(object));
    printf(": %15s : %-9ld :\n", "glo", sizeof(glob));
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");

    if (grid_threaded) {
      printf("\nthreaded grid on %d cores\n\n",
             std::thread::hardware_concurrency());
    }

    application_init();
  }

  inline void free() {
    application_free();
    grid.free();
    objects.free();
    globs.free();
    materials.free();
    textures.free();
    shaders.free();
    window.free();
    sdl.free();
    net.free();
    metrics.print(stderr);
    metrics.free();
  }

  inline void render() {
    if (camera_follow_object) {
      camera.look_at = camera_follow_object->position;
    }

    camera.update_matrix_wvp();

    glUniformMatrix4fv(shaders::umtx_wvp, 1, GL_FALSE,
                       glm::value_ptr(camera.Mvp));
    glUniform3fv(shaders::ulht, 1, glm::value_ptr(ambient_light));
    glClearColor(background_color.red, background_color.green,
                 background_color.blue, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.render();

    window.swap_buffers();
  }

  inline void run() {
    const float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;
    float rad_over_mouse_pixels = rad_over_degree * .02f;
    float mouse_sensitivity = 1.5f;
    bool mouse_mode = false;
    bool do_render = true;
    bool print_grid = false;
    bool resolve_collisions = true;

    SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);

    metrics.fps.calculation_intervall_ms = 1000;
    metrics.reset_timer();
    metrics.print_headers(stderr);

    if (net.enabled) {
      // send initial signals
      net.begin();
    }

    // synchronization of update and render thread
    bool is_rendering = false;
    std::mutex is_rendering_mutex{};
    std::condition_variable is_rendering_cv{};

    // while application is running
    bool is_running = true;

    // note. needed after 'application_init'
    objects.apply_allocated_instances();

    // the update grid thread
    std::thread update_thread([&]() {
      uint32_t frame_num = 0;
      while (is_running) {
        frame_num++;
        {
          // wait until render thread is done before removing and adding objects
          // to grid
          std::unique_lock<std::mutex> lock{is_rendering_mutex};
          is_rendering_cv.wait(lock,
                               [&is_rendering] { return not is_rendering; });

          // render thread is done and waiting for grid to remove and add
          // objects

          grid.clear();

          // add all allocated objects to the grid
          object **const end = objects.allocated_list_end();
          for (object **it = objects.allocated_list(); it < end; ++it) {
            object *obj = *it;
            grid.add(obj);
          }

          // update frame context used throughout the frame
          // in multiplayer mode use dt from server else previous frame
          frame_context = {
              frame_num,
              SDL_GetTicks(),
              net.enabled ? net.dt : metrics.fps.dt,
          };

          // notify render thread to start rendering
          is_rendering = true;
          lock.unlock();
          is_rendering_cv.notify_one();
        }

        if (print_grid) {
          grid.print();
        }

        // note. data racing between render and update is ok

        grid.update();

        if (resolve_collisions) {
          grid.resolve_collisions();
        }

        // apply changes done at 'update' and 'resolve_collisions'
        objects.apply_freed_instances();
        objects.apply_allocated_instances();

        // callback
        application_at_frame_end();

        // apply changes done by application
        objects.apply_freed_instances();
        objects.apply_allocated_instances();

        // update signals state
        if (net.enabled) {
          // receive signals from previous frame and send signals of current
          // frame
          net.at_frame_end();
        } else {
          net.states[net.active_state_ix] = net.next_state;
        }
      }

      // in case render thread is waiting
      is_rendering = true;
      is_rendering_cv.notify_one();
    });

    // enter game loop
    while (is_running) {
      metrics.at_frame_begin();

      // poll events
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
          case SDL_WINDOWEVENT_SIZE_CHANGED: {
            auto [w, h] = window.get_width_and_height();
            camera.width = float(w);
            camera.height = float(h);
            glViewport(0, 0, w, h);
            printf(" * window resize to  %u x %u\n", w, h);
            break;
          }
          }
          break;
        }
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_MOUSEMOTION: {
          if (event.motion.xrel != 0) {
            net.next_state.lookangle_y += (float)event.motion.xrel *
                                          rad_over_mouse_pixels *
                                          mouse_sensitivity;
          }
          if (event.motion.yrel != 0) {
            net.next_state.lookangle_x += (float)event.motion.yrel *
                                          rad_over_mouse_pixels *
                                          mouse_sensitivity;
          }
          break;
        }
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
          case SDLK_w:
            net.next_state.keys |= 1u;
            break;
          case SDLK_a:
            net.next_state.keys |= 2u;
            break;
          case SDLK_s:
            net.next_state.keys |= 4u;
            break;
          case SDLK_d:
            net.next_state.keys |= 8u;
            break;
          case SDLK_q:
            net.next_state.keys |= 16u;
            break;
          case SDLK_e:
            net.next_state.keys |= 32u;
            break;
          case SDLK_j:
            net.next_state.keys |= 64u;
            break;
          }
          break;
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
          case SDLK_w:
            net.next_state.keys &= ~1u;
            break;
          case SDLK_a:
            net.next_state.keys &= ~2u;
            break;
          case SDLK_s:
            net.next_state.keys &= ~4u;
            break;
          case SDLK_d:
            net.next_state.keys &= ~8u;
            break;
          case SDLK_q:
            net.next_state.keys &= ~16u;
            break;
          case SDLK_e:
            net.next_state.keys &= ~32u;
            break;
          case SDLK_j:
            net.next_state.keys &= ~64u;
            break;
          case SDLK_SPACE:
            mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
            SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);
            break;
          case SDLK_3:
            shader_program_ix++;
            if (shader_program_ix >= shaders.programs_count()) {
              shader_program_ix = 0;
            }
            break;
          case SDLK_4:
            print_grid = not print_grid;
            break;
          case SDLK_5:
            do_render = not do_render;
            break;
          case SDLK_6:
            resolve_collisions = not resolve_collisions;
            break;
          }
          break;
        }
      }

      // check if shader program has changed
      if (shader_program_ix_prev != shader_program_ix) {
        printf(" * switching to program at index %u\n", shader_program_ix);
        shaders.use_program(shader_program_ix);
        shader_program_ix_prev = shader_program_ix;
      }

      {
        // wait for update thread to remove and add objects to grid
        std::unique_lock<std::mutex> lock{is_rendering_mutex};
        is_rendering_cv.wait(lock, [&is_rendering] { return is_rendering; });

        // note. render and update have acceptable data races on objects
        // position etc

        if (do_render) {
          render();
        }

        // notify update thread that rendering is done
        is_rendering = false;
        lock.unlock();
        is_rendering_cv.notify_one();
      }

      // metrics
      metrics.allocated_objects = objects.allocated_list_len();
      metrics.at_frame_end(stderr);
    }
    //---------------------------------------------------------------------free

    // in case 'update' thread is waiting
    is_rendering = false;
    is_rendering_cv.notify_one();
    update_thread.join();
  }
};

inline engine engine{};

// debugging
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