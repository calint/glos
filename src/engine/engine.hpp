#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

// all includes used by the subsystems
#include <GLES3/gl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <condition_variable>
#include <execution>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <mutex>
#include <netinet/tcp.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unordered_map>
#include <vector>

// include order of subsystems relevant
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
#include "shaders.hpp"
//
#include "camera.hpp"
//
#include "hud.hpp"
//
#include "texture.hpp"
//
#include "material.hpp"
//
#include "glob.hpp"
//
namespace glos {
// debugging functions
static inline void debug_render_wcs_line(glm::vec3 const &from_wcs,
                                         glm::vec3 const &to_wcs,
                                         glm::vec4 const &color);

static inline void debug_render_bounding_sphere(glm::mat4 const &Mmw);

// information about the current frame
class frame_context {
public:
  uint64_t frame_num = 0; // frame number (will rollover)
  uint64_t ms = 0;        // current time since start in milliseconds
  float dt = 0;           // frame delta time in seconds (time step)
};

// globally accessible current frame info
inline frame_context frame_context{};

// a sphere used when debugging object bounding sphere (set at 'init()')
inline uint32_t glob_ix_bounding_sphere = 0;
} // namespace glos

//
// application interface
//

// called at initiation
void application_init();
// called by update thread when an update is done
void application_on_update_done();
// called by render thread when a frame has been rendered
void application_on_render_done();
// called at shutdown
void application_free();

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

// signal bit corresponding to keyboard key
static constexpr uint32_t key_w = 1U << 0U;
static constexpr uint32_t key_a = 1U << 1U;
static constexpr uint32_t key_s = 1U << 2U;
static constexpr uint32_t key_d = 1U << 3U;
static constexpr uint32_t key_q = 1U << 4U;
static constexpr uint32_t key_e = 1U << 5U;
static constexpr uint32_t key_i = 1U << 6U;
static constexpr uint32_t key_j = 1U << 7U;
static constexpr uint32_t key_k = 1U << 8U;
static constexpr uint32_t key_l = 1U << 9U;
static constexpr uint32_t key_u = 1U << 10U;
static constexpr uint32_t key_o = 1U << 11U;

class engine final {
public:
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
      constexpr char const *vtx = R"(
  #version 330 core
  uniform mat4 umtx_wvp; // world-to-view-to-projection
  layout(location = 0) in vec3 apos; // world coordinates
  void main() {
	  gl_Position = umtx_wvp * vec4(apos, 1);
  }
  )";

      constexpr char const *frag = R"(
  #version 330 core
  uniform vec4 ucolor;
  out vec4 rgba;
  void main() {
    rgba = ucolor;
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
    printf(": %15s : %-9zu :\n", "object", sizeof(object));
    printf(": %15s : %-9zu :\n", "glob", sizeof(glob));
    printf(": %15s : %-9zu :\n", "cell_entry", sizeof(cell_entry));
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    puts("");

    if (threaded_grid) {
      printf("threaded grid on %u cores\n\n",
             std::thread::hardware_concurrency());
    }

    // the bounding sphere used for debugging
    glob_ix_bounding_sphere =
        globs.load("assets/obj/bounding_sphere.obj", nullptr);

    // initiate 'frame_context' with current time from server or local timer
    // incase 'application_init()' needs time
    frame_context = {
        0,
        net.enabled ? net.ms : SDL_GetTicks64(),
        0,
    };

    // set defaults for metrics
    metrics.fps.calculation_interval_ms = 1000;
    metrics.enable_print = metrics_print;

    application_init();

    // apply new objects create at 'application_init()'
    objects.apply_allocated_instances();

    printf("\nglobs: %u   vertex data: %zu B  texture data: %zu B\n\n",
           metrics.allocated_globs, metrics.buffered_vertex_data,
           metrics.buffered_texture_data);
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
    metrics.free();
  }

  inline void run() {
    if (net.enabled) {
      // initiate networking by sending initial signals
      net.begin();
    }

    if (threaded_update) {
      // update runs as separate thread
      start_update_thread();
    }

    // start metrics
    metrics.print_headers(stderr);
    metrics.begin();

    SDL_SetRelativeMouseMode(is_mouse_mode ? SDL_TRUE : SDL_FALSE);

    // enter game loop
    while (true) {
      metrics.at_frame_begin();

      // read keyboard, mouse and handle window events
      handle_events();

      // check if quit was received
      if (not is_running) {
        break;
      }

      if (threaded_update) {
        // update runs in separate thread
        render_thread_loop_body();
      } else {
        // update runs on main thread

        render();

        metrics.update_begin();
        update_pass_1();
        update_pass_2();
        metrics.update_end();

        // swap buffers after update to allow debugging rendering
        window.swap_buffers();
      }

      metrics.allocated_objects = uint32_t(objects.allocated_list_len());
      metrics.at_frame_end(stderr);
    }

    if (threaded_update) {
      // trigger update thread incase it is waiting
      std::unique_lock<std::mutex> lock{is_rendering_mutex};
      is_rendering = false;
      lock.unlock();
      is_rendering_cv.notify_one();
      update_thread.join();
    }
  }

private:
  friend void debug_render_wcs_line(glm::vec3 const &from_wcs,
                                    glm::vec3 const &to_wcs,
                                    glm::vec4 const &color);

  static constexpr float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;
  uint64_t frame_num = 0;
  bool is_resolve_collisions = true;
  bool is_print_grid = false;
  bool is_running = true;
  bool is_render = true;
  bool is_render_hud = hud_enabled;
  bool is_mouse_mode = false;
  float mouse_rad_over_pixels = rad_over_degree * .02f;
  float mouse_sensitivity = 1.5f;
  // index of shader that renders world coordinate system line
  size_t shader_program_ix_render_line = 0;
  // index of current shader
  size_t shader_program_ix = 0;
  // index of previous shader
  size_t shader_program_ix_prv = shader_program_ix;

  // synchronization of update and render thread
  std::thread update_thread{};
  bool is_rendering = true;
  std::mutex is_rendering_mutex{};
  std::condition_variable is_rendering_cv{};

  inline void render() {
    if (not is_render) {
      return;
    }

    // check if shader program has changed
    if (shader_program_ix_prv != shader_program_ix) {
      printf(" * switching to program at index %zu\n", shader_program_ix);
      shaders.use_program(shader_program_ix);
      shader_program_ix_prv = shader_program_ix;
    }

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

    if (is_render_hud) {
      shaders.use_program(hud.program_ix);
      hud.render();
      shaders.use_program(shader_program_ix);
    }

    grid.render();

    application_on_render_done();
  }

  inline void update_pass_1() {
    // remove all objects from grid
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
    ++frame_num;

    if (net.enabled) {
      frame_context = {frame_num, net.ms, net.dt};
    } else {
      frame_context = {frame_num, SDL_GetTicks64(), metrics.dt};
    }
  }

  inline void update_pass_2() const {
    if (is_print_grid) {
      grid.print();
    }

    // note. data racing between render and update thread on objects
    // position, angle, scale glob index is ok (?)

    grid.update();

    if (is_resolve_collisions) {
      grid.resolve_collisions();
    }

    // apply changes done at 'update()' and 'resolve_collisions()'
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
      net.receive_and_send();
    } else {
      // copy signals to active player
      net.states[net.player_ix] = net.next_state;
    }
  }

  inline void start_update_thread() {
    update_thread = std::thread([this]() {
      while (true) {
        {
          // wait until render thread is done before removing and adding objects
          // to grid
          std::unique_lock<std::mutex> lock{is_rendering_mutex};
          is_rendering_cv.wait(lock, [this] { return not is_rendering; });

          if (not is_running) {
            return;
          }

          metrics.update_begin();

          update_pass_1();

          // notify render thread to start rendering
          is_rendering = true;
          lock.unlock();
          is_rendering_cv.notify_one();
        }

        // running in parallel with render thread
        update_pass_2();

        metrics.update_end();
      }
    });
  }

  void render_thread_loop_body() {
    // wait for update thread to remove and add objects to grid
    std::unique_lock<std::mutex> lock{is_rendering_mutex};
    is_rendering_cv.wait(lock, [this] { return is_rendering; });

    // note. render and update have acceptable (?) data races on objects
    // position, angle, scale, glob index etc

    render();

    window.swap_buffers();

    // notify update thread that rendering is done
    is_rendering = false;
    lock.unlock();
    is_rendering_cv.notify_one();
  }

  inline void handle_events() {
    // poll events
    SDL_Event event = {};
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
        case SDLK_u:
          net.next_state.keys |= key_u;
          break;
        case SDLK_i:
          net.next_state.keys |= key_i;
          break;
        case SDLK_o:
          net.next_state.keys |= key_o;
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
        case SDLK_u:
          net.next_state.keys &= ~key_u;
          break;
        case SDLK_i:
          net.next_state.keys &= ~key_i;
          break;
        case SDLK_o:
          net.next_state.keys &= ~key_o;
          break;
        case SDLK_SPACE:
          is_mouse_mode = is_mouse_mode ? SDL_FALSE : SDL_TRUE;
          SDL_SetRelativeMouseMode(is_mouse_mode ? SDL_TRUE : SDL_FALSE);
          break;
        case SDLK_F1:
          is_print_grid = not is_print_grid;
          break;
        case SDLK_F2:
          is_resolve_collisions = not is_resolve_collisions;
          break;
        case SDLK_F3:
          is_render = not is_render;
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
          is_render_hud = not is_render_hud;
          break;
        }
        break;
      }
    }
  }
};

inline engine engine{};

// debugging (highly inefficient) function for rendering world coordinate system
// lines
static inline void debug_render_wcs_line(glm::vec3 const &from_wcs,
                                         glm::vec3 const &to_wcs,
                                         glm::vec4 const &color) {
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

  camera.update_matrix_wvp();

  shaders.use_program(engine.shader_program_ix_render_line);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(camera.Mwvp));
  glUniform4fv(1, 1, glm::value_ptr(color));

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDrawArrays(GL_LINES, 0, 2);
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  shaders.use_program(engine.shader_program_ix);
}

static inline void debug_render_bounding_sphere(glm::mat4 const &Mmw) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  globs.at(glob_ix_bounding_sphere).render(Mmw);
  glDisable(GL_BLEND);
}

} // namespace glos