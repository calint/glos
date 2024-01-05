#pragma once
// reviewed: 2024-01-04

#include <condition_variable>
#include <mutex>

// inclusion order relevant
#include "../application/configuration.hpp"
//
#include "metrics.hpp"
//
#include "net.hpp"
//
#include "net_server.hpp"
//
#include "sdl.hpp"
//
#include "window.hpp"
//
#include "shader.hpp"
//
#include "camera.hpp"
//
#include "hud.hpp"
//
#include "material.hpp"
//
#include "glob.hpp"
//
namespace glos {
// debugging functions
inline static void debug_render_wcs_line(glm::vec3 const &from_wcs,
                                         glm::vec3 const &to_wcs,
                                         glm::vec3 const &color);

inline static void debug_render_bounding_sphere(glm::mat4 const &Mmw);

// information about the current frame
class frame_context {
public:
  uint32_t frame_num = 0; // frame number (will rollover)
  uint64_t ms = 0;        // current time since start in milliseconds
  float dt = 0;           // frame delta time in seconds (time step)
};

// globally accessible current frame info
inline frame_context frame_context{};

} // namespace glos

// application interface

// called at initiation
void application_init();
// called by update thread when an update is done
void application_on_update_done();
// called by render thread when a frame has been rendered
void application_on_render_done();
// called at shutdown
void application_free();

// a sphere used when debugging object bounding sphere (set at init)
inline uint32_t glob_ix_bounding_sphere = 0;

//
#include "object.hpp"
//
#include "grid.hpp"
//

namespace glos {

struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
};

// color to clear screen with
inline color background_color{0, 0, 0};

// ambient light used by shader
inline glm::vec3 ambient_light = glm::normalize(glm::vec3{0, 1, 1});

// object the camera should follow
inline object *camera_follow_object = nullptr;

// signal bit corresponding to keyboard
static constexpr uint32_t key_w = 1 << 0;
static constexpr uint32_t key_a = 1 << 1;
static constexpr uint32_t key_s = 1 << 2;
static constexpr uint32_t key_d = 1 << 3;
static constexpr uint32_t key_q = 1 << 4;
static constexpr uint32_t key_e = 1 << 5;
static constexpr uint32_t key_i = 1 << 6;
static constexpr uint32_t key_j = 1 << 7;
static constexpr uint32_t key_k = 1 << 8;
static constexpr uint32_t key_l = 1 << 9;
static constexpr uint32_t key_u = 1 << 10;
static constexpr uint32_t key_o = 1 << 11;

class engine final {
public:
  // index of shader that renders world coordinate system line
  size_t shader_program_ix_render_line = 0;
  // index of shader that renders bounding sphere
  size_t shader_program_ix_render_bounding_sphere = 0;
  // index of current shader
  size_t shader_program_ix = 0;
  // index of previous shader
  size_t shader_program_ix_prv = shader_program_ix;
  // render heads-up-display
  bool render_hud = true;

  inline void init() {
    // set random number generator seed for deterministic behaviour
    srand(0);
    // initiate subsystems
    metrics.init();
    net.init();
    sdl.init();
    window.init();
    shaders.init();
    hud.init();
    textures.init();
    materials.init();
    globs.init();
    objects.init();
    grid.init();

    // line rendering shader
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
      shader_program_ix_render_line =
          shaders.load_program_from_source(vtx, frag);
    }

    // info
    printf("class sizes:\n");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9s :\n", "class", "bytes");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9ld :\n", "object", sizeof(object));
    printf(": %15s : %-9ld :\n", "glo", sizeof(glob));
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");

    if (grid_threaded) {
      printf("\nthreaded grid on %u cores\n\n",
             std::thread::hardware_concurrency());
    }

    glob_ix_bounding_sphere =
        globs.load("assets/obj/bounding_sphere.obj", nullptr);

    // initiate 'frame_context' with current time from server or local timer
    // incase 'application_init' needs time
    frame_context = {
        0,
        net.enabled ? net.ms : SDL_GetTicks64(),
        0,
    };

    application_init();

    // apply new objects create at 'application_init'
    objects.apply_allocated_instances();
  }

  inline void free() {
    application_free();
    grid.free();
    objects.free();
    globs.free();
    materials.free();
    textures.free();
    hud.free();
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
                       glm::value_ptr(camera.Mwvp));

    glUniform3fv(shaders::ulht, 1, glm::value_ptr(ambient_light));

    glClearColor(background_color.red, background_color.green,
                 background_color.blue, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.render();

    if (render_hud) {
      shaders.use_program(hud.program_ix);
      hud.render();
      shaders.use_program(shader_program_ix);
    }

    window.swap_buffers();
  }

  inline void run() {
    constexpr float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;

    // mouse related variables
    bool mouse_mode = false;
    float mouse_rad_over_pixels = rad_over_degree * .02f;
    float mouse_sensitivity = 1.5f;

    SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);

    // turn on/off subsystems
    bool do_render = true;
    bool print_grid = false;
    bool resolve_collisions = true;

    // initiate metrics
    metrics.fps.calculation_intervall_ms = 1000;
    metrics.reset_timer();
    metrics.print_headers(stderr);

    // initiate networking
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

    // the update grid thread
    std::thread update_thread([&]() {
      uint32_t frame_num = 0;
      while (is_running) {
        ++frame_num;
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
          // in multiplayer mode use dt and ms from server
          // in single player mode use dt from previous frame and current ms
          frame_context = {
              frame_num,
              net.enabled ? net.ms : SDL_GetTicks64(),
              net.enabled ? net.dt : metrics.fps.dt,
          };

          // printf("frame_context: frame=%i   dt=%f   ms=%lu\n",
          //        frame_context.frame_num, frame_context.dt,
          //        frame_context.ms);

          // notify render thread to start rendering
          is_rendering = true;
          lock.unlock();
          is_rendering_cv.notify_one();
        }

        if (print_grid) {
          grid.print();
        }

        // note. data racing between render and update thread on objects
        // position, angle, scale glob index is ok

        grid.update();

        if (resolve_collisions) {
          grid.resolve_collisions();
        }

        // apply changes done at 'update' and 'resolve_collisions'
        objects.apply_freed_instances();
        objects.apply_allocated_instances();

        // callback application
        application_on_update_done();

        // apply changes done by application
        objects.apply_freed_instances();
        objects.apply_allocated_instances();

        // update signals from network or local
        if (net.enabled) {
          // receive signals from previous frame and send signals of current
          // frame
          net.at_update_done();
        } else {
          // copy signals to active player
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
      SDL_Event event = {0};
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
            net.next_state.look_angle_y += (float)event.motion.xrel *
                                           mouse_rad_over_pixels *
                                           mouse_sensitivity;
          }
          if (event.motion.yrel != 0) {
            net.next_state.look_angle_x += (float)event.motion.yrel *
                                           mouse_rad_over_pixels *
                                           mouse_sensitivity;
          }
          break;
        }
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
          case SDLK_w:
            net.next_state.keys |= key_w;
            break;
          case SDLK_a:
            net.next_state.keys |= key_a;
            break;
          case SDLK_s:
            net.next_state.keys |= key_s;
            break;
          case SDLK_d:
            net.next_state.keys |= key_d;
            break;
          case SDLK_q:
            net.next_state.keys |= key_q;
            break;
          case SDLK_e:
            net.next_state.keys |= key_e;
            break;
          case SDLK_j:
            net.next_state.keys |= key_j;
            break;
          case SDLK_k:
            net.next_state.keys |= key_k;
            break;
          case SDLK_l:
            net.next_state.keys |= key_l;
            break;
          case SDLK_i:
            net.next_state.keys |= key_i;
            break;
          }
          break;
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
          case SDLK_w:
            net.next_state.keys &= ~key_w;
            break;
          case SDLK_a:
            net.next_state.keys &= ~key_a;
            break;
          case SDLK_s:
            net.next_state.keys &= ~key_s;
            break;
          case SDLK_d:
            net.next_state.keys &= ~key_d;
            break;
          case SDLK_q:
            net.next_state.keys &= ~key_q;
            break;
          case SDLK_e:
            net.next_state.keys &= ~key_e;
            break;
          case SDLK_j:
            net.next_state.keys &= ~key_j;
            break;
          case SDLK_k:
            net.next_state.keys &= ~key_k;
            break;
          case SDLK_l:
            net.next_state.keys &= ~key_l;
            break;
          case SDLK_SPACE:
            mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
            SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);
            break;
          case SDLK_F1:
            print_grid = not print_grid;
            break;
          case SDLK_F2:
            resolve_collisions = not resolve_collisions;
            break;
          case SDLK_F3:
            do_render = not do_render;
            break;
          case SDLK_F4:
            ++shader_program_ix;
            if (shader_program_ix >= shaders.programs_count()) {
              shader_program_ix = 0;
            }
            break;
          case SDLK_F5:
            debug_object_planes_normals = not debug_object_planes_normals;
            break;
          case SDLK_F6:
            debug_object_bounding_sphere = not debug_object_bounding_sphere;
            break;
          case SDLK_F7:
            render_hud = not render_hud;
            break;
          }
          break;
        }
      }

      // check if quit was received
      if (not is_running) {
        break;
      }

      // check if shader program has changed
      if (shader_program_ix_prv != shader_program_ix) {
        printf(" * switching to program at index %zu\n", shader_program_ix);
        shaders.use_program(shader_program_ix);
        shader_program_ix_prv = shader_program_ix;
      }

      {
        // wait for update thread to remove and add objects to grid
        std::unique_lock<std::mutex> lock{is_rendering_mutex};
        is_rendering_cv.wait(lock, [&is_rendering] { return is_rendering; });

        // note. render and update have acceptable data races on objects
        // position, angle, scale, glob index etc

        if (do_render) {
          render();
        }

        application_on_render_done();

        // notify update thread that rendering is done
        is_rendering = false;
        lock.unlock();
        is_rendering_cv.notify_one();
      }

      // metrics
      metrics.allocated_objects = uint32_t(objects.allocated_list_len());
      metrics.at_frame_end(stderr);
    }

    // exited game loop
    // notify update thread in case it is waiting
    is_rendering = false;
    is_rendering_cv.notify_one();
    update_thread.join();
  }
};

inline engine engine{};

// debugging (highly inefficient) function for rendering world coordinate system
// lines
inline static void debug_render_wcs_line(glm::vec3 const &from_wcs,
                                         glm::vec3 const &to_wcs,
                                         glm::vec3 const &color) {
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glm::vec3 const line_vertices[]{from_wcs, to_wcs};
  glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  camera.update_matrix_wvp();

  shaders.use_program(engine.shader_program_ix_render_line);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(camera.Mwvp));
  glUniform3fv(1, 1, glm::value_ptr(color));

  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  shaders.use_program(engine.shader_program_ix);
}

inline static void debug_render_bounding_sphere(glm::mat4 const &Mmw) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  globs.at(glob_ix_bounding_sphere).render(Mmw);
  glDisable(GL_BLEND);
}

} // namespace glos