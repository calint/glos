// reviewed: 2023-12-22

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// include order of subsystems relevant
#include "app/defs.hpp"
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
#include "net/net_server.hpp"

static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} background_color = {0, 0, .3f};

//
#include "app/application.hpp"

//----------------------------------------------------------------------- init
inline static void main_init_shaders() {
  {
    const char *vtx = R"(
#version 130
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
out float depth;
void main(){
  gl_Position=umtx_wvp*umtx_mw*vec4(apos,1);
//  depth=gl_Position.z/2000.0;
  depth=gl_Position.z/gl_Position.w;
}
    )";

    const char *frag = R"(
#version 130
in float depth;
out vec4 rgba;
void main(){
  rgba=vec4(vec3(depth),1.0);
}
    )";

    shaders.load_program_from_source(vtx, frag, {shaders::apos});
  }
  {
    const char *vtx = R"(
#version 130
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
in vec4 argba;
out vec4 vrgba;
void main(){
  gl_Position=umtx_wvp*umtx_mw*vec4(apos,1);
  vrgba=argba;
}
    )";

    const char *frag = R"(
#version 130
in vec4 vrgba;
out vec4 rgba;
void main(){
  rgba=vrgba;
}
    )";

    shaders.load_program_from_source(vtx, frag,
                                     {shaders::apos, shaders::argba});
  }
}

inline static void main_render(const frame_ctx &fc) {
  camera.update_matrix_wvp();
  glUniformMatrix4fv(shaders::umtx_wvp, 1, GL_FALSE,
                     glm::value_ptr(camera.Mvp));
  glClearColor(background_color.red, background_color.green,
               background_color.blue, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  grid.render(fc);
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

  puts("");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "type", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "object", sizeof(object));
  printf(": %15s : %-9ld :\n", "glo", sizeof(glo));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

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

  main_init_shaders();

  // sdl.play_path("music/ambience.mp3");

  unsigned shader_program_ix = 0;
  unsigned shader_program_ix_prev = shader_program_ix;
  bool do_main_render = true;
  bool print_grid = false;
  const float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;
  float rad_over_mouse_pixels = rad_over_degree * .02f;
  float mouse_sensitivity = 1.5f;
  bool mouse_mode = false;
  unsigned frame_num = 0;

  {
    puts("");
    const program &p = shaders.programs.at(shader_program_ix);
    glUseProgram(p.id);
    printf(" * using program at index %u\n", shader_program_ix);
    for (int ix : p.enabled_attributes) {
      glEnableVertexAttribArray(ix);
    }
    puts("");
  }

  SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);

  metrics.fps.calculation_intervall_ms = 1000;
  metrics.reset_timer();
  metrics.print_headers(stderr);

  if (use_net) {
    // send initial signals
    net.begin();
  }

  // information about dt (elapsed time in previous frame) and a unique frame
  // number (rollover possible)
  frame_ctx fc{};

  // enter game loop
  for (bool running = true; running;) {
    metrics.at_frame_begin();

    // poll events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          int w = 0, h = 0;
          SDL_GetWindowSize(window.sdl_window, &w, &h);
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
        running = false;
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
          net.next_state.keys &= ~1;
          break;
        case SDLK_a:
          net.next_state.keys &= ~2;
          break;
        case SDLK_s:
          net.next_state.keys &= ~4;
          break;
        case SDLK_d:
          net.next_state.keys &= ~8;
          break;
        case SDLK_q:
          net.next_state.keys &= ~16;
          break;
        case SDLK_e:
          net.next_state.keys &= ~32;
          break;
        case SDLK_o:
          net.next_state.keys &= ~64;
          break;
        case SDLK_SPACE:
          mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
          SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);
          break;
        case SDLK_3:
          shader_program_ix++;
          if (shader_program_ix >= shaders.programs.size()) {
            shader_program_ix = 0;
          }
          break;
        }
        break;
      }
    }

    // check if shader program has changed
    if (shader_program_ix_prev != shader_program_ix) {
      printf(" * switching to program at index %u\n", shader_program_ix);
      {
        const program &p = shaders.programs.at(shader_program_ix_prev);
        for (int ix : p.enabled_attributes) {
          printf("   * disable vertex attrib array %d\n", ix);
          glDisableVertexAttribArray(ix);
        }
      }
      {
        const program &p = shaders.programs.at(shader_program_ix);
        glUseProgram(p.id);
        for (int ix : p.enabled_attributes) {
          printf("   * enable vertex attrib array %d\n", ix);
          glEnableVertexAttribArray(ix);
        }
      }
      shader_program_ix_prev = shader_program_ix;
    }

    // start frame
    frame_num++;

    // in multiplayer mode use dt from server else previous frame
    fc.dt = use_net ? net.dt : metrics.fps.dt;
    fc.tick = frame_num;

    // update grid
    grid.clear();
    // add all allocated objects to the grid
    object **const end = objects.store.allocated_list_end();
    for (object **it = objects.store.allocated_list(); it < end; it++) {
      object *obj = *it;
      grid.add(obj);
    }
    if (print_grid) {
      grid.print();
    }
    grid.update(fc);
    grid.resolve_collisions(fc);
    // apply delete on objects that have died during 'update' and 'on_collision'
    objects.apply_free();

    if (do_main_render) {
      main_render(fc);
    }

    // update signals state
    if (use_net) {
      // receive signals from previous frame and send signals of current frame
      net.at_frame_end();
    } else {
      net.states[net.active_state_ix] = net.next_state;
    }

    // callback
    application.at_frame_end();

    // metrics
    metrics.objects_allocated = objects.allocated_objects_count();
    metrics.at_frame_end(stderr);
  }
  //---------------------------------------------------------------------free
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
  puts(" * clean exit");
  return 0;
}
