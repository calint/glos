// reviewed: 2023-12-22

#include <SDL2/SDL.h>
#include <condition_variable>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <mutex>

inline static void debug_render_wcs_line(const glm::vec3 &from_wcs,
                                         const glm::vec3 &to_wcs,
                                         const glm::vec3 &color);

// include order of subsystems relevant
#include "app/configuration.hpp"
//
#include "metrics.hpp"
//
#include "net/net.hpp"
//
#include "sdl/sdl.hpp"
//
#include "sdl/window.hpp"
//
#include "gfx/shader.hpp"
//
#include "gfx/camera.hpp"
//
#include "gfx/material.hpp"
//
#include "gfx/glo.hpp"
//
class frame_ctx {
public:
  float dt = 0;
  unsigned tick = 0;
};
#include "obj/object.hpp"
//
#include "grid/grid.hpp"
//
#include "net/net.hpp"
//
#include "net/net_server.hpp"

static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} background_color = {0, 0, 0};

static glm::vec3 ambient_light = glm::normalize(glm::vec3{0, 1, 1});

static object *camera_follow_object = nullptr;

//
#include "app/application.hpp"

static int shader_program_line = 0;

//----------------------------------------------------------------------- init
inline static void main_init_shaders() {
  {
    const char *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
out float depth;
void main() {
  gl_Position = umtx_wvp * umtx_mw * vec4(apos, 1);
  depth = gl_Position.z / 300; // magic number is camera far plane
}
    )";

    const char *frag = R"(
#version 330 core
in float depth;
out vec4 rgba;
void main() {
  rgba = vec4(vec3(depth), 1.0);
}
    )";

    shaders.load_program_from_source(vtx, frag);
  }
  {
    const char *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
in vec4 argba;
out vec4 vrgba;
void main() {
  gl_Position = umtx_wvp * umtx_mw * vec4(apos, 1);
  vrgba = argba;
}
    )";

    const char *frag = R"(
#version 330 core
in vec4 vrgba;
out vec4 rgba;
void main(){
  rgba = vrgba;
}
    )";

    shaders.load_program_from_source(vtx, frag);
  }
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
    shader_program_line = shaders.load_program_from_source(vtx, frag);
  }
}

static int shader_program_ix = 0;
static int shader_program_ix_prev = shader_program_ix;

// for debugging
inline static void debug_render_wcs_line(const glm::vec3 &from_wcs,
                                         const glm::vec3 &to_wcs,
                                         const glm::vec3 &color) {
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

  shaders.use_program(shader_program_line);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(camera.Mvp));
  glUniform3fv(1, 1, glm::value_ptr(color));

  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  shaders.use_program(shader_program_ix);
}

inline static void main_render(const unsigned render_frame_num) {
  if (camera_follow_object) {
    camera.look_at = camera_follow_object->physics.position;
  }

  camera.update_matrix_wvp();

  glUniformMatrix4fv(shaders::umtx_wvp, 1, GL_FALSE,
                     glm::value_ptr(camera.Mvp));
  glUniform3fv(shaders::ulht, 1, glm::value_ptr(ambient_light));
  glClearColor(background_color.red, background_color.green,
               background_color.blue, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  grid.render(render_frame_num);

  window.swap_buffers();
}

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  // check if this instance is server
  if (argc > 1 && *argv[1] == 's') {
    net_server.init();
    net_server.loop();
    net_server.free();
    return 0;
  }

  puts("\nprogram glos\n");

  // set random number generator seed for deterministic behaviour
  srand(0);

  // this instance is client
  bool use_net = false;
  if (argc > 1 && *argv[1] == 'c') {
    // connect to server
    use_net = true;
    if (argc > 2) {
      net.host = argv[2];
    }
  }

  // initiate subsystems
  if (use_net) {
    net.init();
  }
  metrics.init();
  sdl.init();
  window.init();
  shaders.init();
  materials.init();
  glos.init();
  objects.init();
  grid.init();
  application.init();
  // apply objects allocated in 'application.init()'
  objects.apply_allocated_instances();

  main_init_shaders();

  puts("");
  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "object", sizeof(object));
  printf(": %15s : %-9ld :\n", "glo", sizeof(glo));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");

  if (grid_threaded) {
    printf("\nthreaded grid on %d cores\n",
           std::thread::hardware_concurrency());
  }
  // sdl.play_path("music/ambience.mp3");

  const float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;
  float rad_over_mouse_pixels = rad_over_degree * .02f;
  float mouse_sensitivity = 1.5f;
  bool mouse_mode = false;
  unsigned render_frame_num = 0;
  bool do_main_render = true;
  bool print_grid = false;
  bool resolve_collisions = true;

  SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);

  metrics.fps.calculation_intervall_ms = 1000;
  metrics.reset_timer();
  metrics.print_headers(stderr);

  if (use_net) {
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
    unsigned update_frame_num = 0;
    while (is_running) {
      update_frame_num++;
      {
        std::unique_lock<std::mutex> lock{is_rendering_mutex};
        is_rendering_cv.wait(lock,
                             [&is_rendering] { return not is_rendering; });

        // printf("update %u\n", update_frame_num);

        grid.clear();

        // add all allocated objects to the grid
        object **const end = objects.allocated_list_end();
        for (object **it = objects.allocated_list(); it < end; it++) {
          object *obj = *it;
          grid.add(obj);
        }

        is_rendering = true;
        lock.unlock();
        is_rendering_cv.notify_one();
      }

      if (print_grid) {
        grid.print();
      }

      // in multiplayer mode use dt from server else previous frame
      frame_ctx fc{use_net ? net.dt : metrics.fps.dt, update_frame_num};

      grid.update(fc);

      if (resolve_collisions) {
        grid.resolve_collisions(fc);
      }

      // apply changes done at 'update' and 'resolve_collisions'
      objects.apply_freed_instances();
      objects.apply_allocated_instances();

      // callback
      application.at_frame_end();

      // apply changes done by application
      objects.apply_freed_instances();
      objects.apply_allocated_instances();

      // update signals state
      if (use_net) {
        // receive signals from previous frame and send signals of current frame
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
          camera.width = (float)w;
          camera.height = (float)h;
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
          net.next_state.keys |= 1;
          break;
        case SDLK_a:
          net.next_state.keys |= 2;
          break;
        case SDLK_s:
          net.next_state.keys |= 4;
          break;
        case SDLK_d:
          net.next_state.keys |= 8;
          break;
        case SDLK_q:
          net.next_state.keys |= 16;
          break;
        case SDLK_e:
          net.next_state.keys |= 32;
          break;
        case SDLK_o:
          net.next_state.keys |= 64;
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
        case SDLK_o:
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
          do_main_render = not do_main_render;
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
      // don't render when grid is adding objects to cells
      std::unique_lock<std::mutex> lock{is_rendering_mutex};
      is_rendering_cv.wait(lock, [&is_rendering] { return is_rendering; });

      render_frame_num++;
      // printf("render %d\n", render_frame_num);

      if (do_main_render) {
        main_render(render_frame_num);
      }

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

  application.free();
  grid.free();
  objects.free();
  glos.free();
  materials.free();
  shaders.free();
  window.free();
  sdl.free();
  if (use_net) {
    net.free();
  }
  metrics.print(stderr);
  metrics.free();
  return 0;
}
