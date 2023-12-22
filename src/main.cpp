#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// include order of subsystems relevant
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
  float dt;
  unsigned tick;
};
#include "obj/object.hpp"
//
#include "grid/grid.hpp"
//
#include "net/net.hpp"
#include "net/net_server.hpp"

static struct game {
  unsigned *keys_ptr;
  const object *follow_object;
} game;

static unsigned shader_program_ix = 0;
static bool do_main_render = true;
static float camera_lookangle_y = 0;
static float camera_lookangle_x = 0;

static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} bg = {.1f, .1f, 0};

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
  glUniformMatrix4fv(shaders::umtx_wvp, 1, 0, glm::value_ptr(camera.Mvp));
  glClearColor(bg.red, bg.green, bg.blue, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  grid.render(fc);
  SDL_GL_SwapWindow(window.ref);
}

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  if (argc > 1 && *argv[1] == 's') {
    net_server_init();
    net_server_loop();
    net_server_free();
  }

  bool use_net = false;
  if (argc > 1 && *argv[1] == 'c') {
    srand(0);
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
  unsigned shader_program_ix_prev = shader_program_ix;
  const float rad_over_degree = 2.0f * glm::pi<float>() / 360.0f;
  float rad_over_mouse_pixels = rad_over_degree * .02f;
  float mouse_sensitivity = 1.5f;
  bool mouse_mode = false;
  SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);

  metrics.fps.calculation_intervall_ms = 1000;
  metrics.reset_timer();
  metrics.print_headers(stderr);
  unsigned frame_num = 0;

  frame_ctx fc = {
      .dt = 0.1f,
      .tick = 0,
  };

  for (bool running = true; running;) {
    metrics.at_frame_begin();

    if (use_net) {
      net.state_to_send.lookangle_y = camera_lookangle_y;
      net.state_to_send.lookangle_x = camera_lookangle_x;
      net.at_frame_begin();
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          SDL_Log("Window %d size changed to %dx%d", event.window.windowID,
                  event.window.data1, event.window.data2);
          int w, h;
          SDL_GetWindowSize(window.ref, &w, &h);
          camera.width = (float)w;
          camera.height = (float)h;
          printf(" * resize to  %u x %u\n", w, h);
          glViewport(0, 0, w, h);
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
          camera_lookangle_y += (float)event.motion.xrel *
                                rad_over_mouse_pixels * mouse_sensitivity;
        }
        if (event.motion.yrel != 0) {
          camera_lookangle_x += (float)event.motion.yrel *
                                rad_over_mouse_pixels * mouse_sensitivity;
        }
        break;
      }
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          net.state_to_send.keys |= 1;
          break;
        case SDLK_a:
          net.state_to_send.keys |= 2;
          break;
        case SDLK_s:
          net.state_to_send.keys |= 4;
          break;
        case SDLK_d:
          net.state_to_send.keys |= 8;
          break;
        case SDLK_q:
          net.state_to_send.keys |= 16;
          break;
        case SDLK_e:
          net.state_to_send.keys |= 32;
          break;
        case SDLK_o:
          net.state_to_send.keys |= 64;
          break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          net.state_to_send.keys &= ~1;
          break;
        case SDLK_a:
          net.state_to_send.keys &= ~2;
          break;
        case SDLK_s:
          net.state_to_send.keys &= ~4;
          break;
        case SDLK_d:
          net.state_to_send.keys &= ~8;
          break;
        case SDLK_q:
          net.state_to_send.keys &= ~16;
          break;
        case SDLK_e:
          net.state_to_send.keys &= ~32;
          break;
        case SDLK_o:
          net.state_to_send.keys &= ~64;
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

    if (!use_net) {
      net.state_current[net.active_player_index].keys = net.state_to_send.keys;
    }
    if (game.keys_ptr) {
      *game.keys_ptr = net.state_current[net.active_player_index].keys;
    }
    if (game.follow_object) {
      camera.look_at = game.follow_object->physics.position;
    }

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

    fc.dt = use_net ? net.dt : metrics.fps.dt;
    fc.tick = frame_num;

    frame_num++;

    grid.clear();
    for (object *o : objects.store) {
      grid.add(o);
    }
    grid.update(fc);
    grid.resolve_collisions(fc);
    if (do_main_render) {
      main_render(fc);
    }
    if (use_net) {
      net.at_frame_end();
    }
    application.at_frame_completed();
    metrics.objects_allocated = objects.store.size();
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
//----------------------------------------------------------------------------
