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

static bool use_net = false;

// include order relevant
#include "app/configuration.hpp"
//
#include "engine/engine.hpp"
//
#include "app/application.hpp"

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

    glos::shaders.load_program_from_source(vtx, frag);
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

    glos::shaders.load_program_from_source(vtx, frag);
  }
}

inline static void main_render(const unsigned render_frame_num) {
  if (glos::camera_follow_object) {
    glos::camera.look_at = glos::camera_follow_object->position;
  }

  glos::camera.update_matrix_wvp();

  glUniformMatrix4fv(glos::shaders::umtx_wvp, 1, GL_FALSE,
                     glm::value_ptr(glos::camera.Mvp));
  glUniform3fv(glos::shaders::ulht, 1, glm::value_ptr(glos::ambient_light));
  glClearColor(glos::background_color.red, glos::background_color.green,
               glos::background_color.blue, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glos::grid.render(render_frame_num);

  glos::window.swap_buffers();
}

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  // check if this instance is server
  if (argc > 1 && *argv[1] == 's') {
    glos::net_server.init();
    glos::net_server.loop();
    glos::net_server.free();
    return 0;
  }

  puts("\nprogram glos\n");

  // set random number generator seed for deterministic behaviour
  srand(0);

  // this instance is client
  if (argc > 1 && *argv[1] == 'c') {
    // connect to server
    use_net = true;
    if (argc > 2) {
      glos::net.host = argv[2];
    }
  }

  glos::engine.init();

  application.init();

  // apply objects allocated in 'application.init()'
  glos::objects.apply_allocated_instances();

  main_init_shaders();

  puts("");
  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "object", sizeof(glos::object));
  printf(": %15s : %-9ld :\n", "glo", sizeof(glos::glo));
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

  glos::metrics.fps.calculation_intervall_ms = 1000;
  glos::metrics.reset_timer();
  glos::metrics.print_headers(stderr);

  if (use_net) {
    // send initial signals
    glos::net.begin();
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

        glos::grid.clear();

        // add all allocated objects to the grid
        glos::object **const end = glos::objects.allocated_list_end();
        for (glos::object **it = glos::objects.allocated_list(); it < end;
             ++it) {
          glos::object *obj = *it;
          glos::grid.add(obj);
        }

        is_rendering = true;
        lock.unlock();
        is_rendering_cv.notify_one();
      }

      if (print_grid) {
        glos::grid.print();
      }

      // in multiplayer mode use dt from server else previous frame
      glos::frame_context fc{use_net ? glos::net.dt : glos::metrics.fps.dt,
                             update_frame_num};

      glos::grid.update(fc);

      if (resolve_collisions) {
        glos::grid.resolve_collisions(fc);
      }

      // apply changes done at 'update' and 'resolve_collisions'
      glos::objects.apply_freed_instances();
      glos::objects.apply_allocated_instances();

      // callback
      application.at_frame_end();

      // apply changes done by application
      glos::objects.apply_freed_instances();
      glos::objects.apply_allocated_instances();

      // update signals state
      if (use_net) {
        // receive signals from previous frame and send signals of current frame
        glos::net.at_frame_end();
      } else {
        glos::net.states[glos::net.active_state_ix] = glos::net.next_state;
      }
    }
    // in case render thread is waiting
    is_rendering = true;
    is_rendering_cv.notify_one();
  });

  // enter game loop
  while (is_running) {
    glos::metrics.at_frame_begin();

    // poll events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          auto [w, h] = glos::window.get_width_and_height();
          glos::camera.width = float(w);
          glos::camera.height = float(h);
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
          glos::net.next_state.lookangle_y += (float)event.motion.xrel *
                                              rad_over_mouse_pixels *
                                              mouse_sensitivity;
        }
        if (event.motion.yrel != 0) {
          glos::net.next_state.lookangle_x += (float)event.motion.yrel *
                                              rad_over_mouse_pixels *
                                              mouse_sensitivity;
        }
        break;
      }
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          glos::net.next_state.keys |= 1;
          break;
        case SDLK_a:
          glos::net.next_state.keys |= 2;
          break;
        case SDLK_s:
          glos::net.next_state.keys |= 4;
          break;
        case SDLK_d:
          glos::net.next_state.keys |= 8;
          break;
        case SDLK_q:
          glos::net.next_state.keys |= 16;
          break;
        case SDLK_e:
          glos::net.next_state.keys |= 32;
          break;
        case SDLK_o:
          glos::net.next_state.keys |= 64;
          break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          glos::net.next_state.keys &= ~1u;
          break;
        case SDLK_a:
          glos::net.next_state.keys &= ~2u;
          break;
        case SDLK_s:
          glos::net.next_state.keys &= ~4u;
          break;
        case SDLK_d:
          glos::net.next_state.keys &= ~8u;
          break;
        case SDLK_q:
          glos::net.next_state.keys &= ~16u;
          break;
        case SDLK_e:
          glos::net.next_state.keys &= ~32u;
          break;
        case SDLK_o:
          glos::net.next_state.keys &= ~64u;
          break;
        case SDLK_SPACE:
          mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
          SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);
          break;
        case SDLK_3:
          glos::engine.shader_program_ix++;
          if (glos::engine.shader_program_ix >=
              glos::shaders.programs_count()) {
            glos::engine.shader_program_ix = 0;
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
    if (glos::engine.shader_program_ix_prev != glos::engine.shader_program_ix) {
      printf(" * switching to program at index %u\n",
             glos::engine.shader_program_ix);
      glos::shaders.use_program(glos::engine.shader_program_ix);
      glos::engine.shader_program_ix_prev = glos::engine.shader_program_ix;
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
    glos::metrics.allocated_objects = glos::objects.allocated_list_len();
    glos::metrics.at_frame_end(stderr);
  }
  //---------------------------------------------------------------------free

  // in case 'update' thread is waiting
  is_rendering = false;
  is_rendering_cv.notify_one();
  update_thread.join();

  application.free();
  glos::engine.free();
  return 0;
}
