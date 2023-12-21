#include "lib.h"

#include "metrics.hpp"

#include "gfx.hpp"

#include "net.h"

#include "grid.h"

#include "obj/objects.h"

static struct _game {
  int *keybits_ptr;
  const object *follow_ptr;
} game;

static unsigned shader_program_ix = 0;
static bool do_main_render = true;
static float camera_lookangle_y = 0;
static float camera_lookangle_x = 0;
static camera camera;
static struct color {
  GLclampf red;
  GLclampf green;
  GLclampf blue;
} bg = {.1f, .1f, 0};

#include "app/init.h"

//----------------------------------------------------------------------- init
inline static void main_init_programs() {
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

    std::vector<int> attrs{shader_apos};
    shader_load_program_from_source(vtx, frag, attrs);
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

    std::vector<int> attrs{shader_apos, shader_argba};
    shader_load_program_from_source(vtx, frag, attrs);
  }
}

inline static void main_init() {
  main_init_programs();
  main_init_scene();
}

inline static void main_render(framectx *fc) {
  camera.update_matrix_wvp();
  glUniformMatrix4fv(shader_umtx_wvp, 1, 0, camera.mxwvp);
  glClearColor(bg.red, bg.green, bg.blue, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  grid_render(fc);
  SDL_GL_SwapWindow(window.ref);
}

static inline void _foreach_object_add_to_grid_(object *o) { grid_add(o); }

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  if (argc > 1 && *argv[1] == 's') {
    netsrv_init();
    netsrv_loop();
    netsrv_free();
  }

  int use_net = 0;
  if (argc > 1 && *argv[1] == 'c') {
    srand(0);
    use_net = 1;
    if (argc > 2) {
      net_host = argv[2];
    }
  }

  puts("");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "type", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "part", sizeof(part));
  printf(": %15s : %-9ld :\n", "object", sizeof(object));
  printf(": %15s : %-9ld :\n", "glo", sizeof(glo));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

  unsigned gloid = 0;
  if (use_net) {
    net_init();
  }
  metrics.init();
  sdl_init();
  window_init();
  shader_init();
  //	objects_init();
  glos_init();
  grid_init();
  main_init();

  {
    puts("");
    const program &p = programs.at(shader_program_ix);
    glUseProgram(p.id);
    printf(" * using program at index %u\n", shader_program_ix);
    for (int ix : p.attributes) {
      glEnableVertexAttribArray(ix);
    }
    puts("");
  }
  unsigned previous_active_program_ix = shader_program_ix;
  const float rad_over_degree = 2.0f * PI / 360.0f;
  float rad_over_mouse_pixels = rad_over_degree * .02f;
  float mouse_sensitivity = 1.5f;
  SDL_bool mouse_mode = SDL_FALSE;
  //	printf("   * \n");
  //	printf("   * press space to grab and ungrab mouse\n");
  //	printf("   * \n");
  SDL_SetRelativeMouseMode(mouse_mode);

  metrics.fps.calculation_intervall_ms = 1000;
  metrics.reset_timer();
  metrics.print_headers(stderr);
  unsigned frameno = 1;

  framectx fc = {
      .dt = 0.1f,
      .tick = 0,
  };
  // grid_clear();
  // objects_foa({ grid_add(o); });
  // grid_update(&fc);

  for (int running = 1; running;) {
    metrics.at_frame_begin();

    if (use_net) {
      net_state_to_send.lookangle_y = camera_lookangle_y;
      net_state_to_send.lookangle_x = camera_lookangle_x;
      net__at__frame_begin();
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
          SDL_GetWindowSize(window.ref, &w, &h); // ? glGet
          camera.wi = (float)w;
          camera.hi = (float)h;
          printf(" * resize to  %u x %u\n", w, h);
          glViewport(0, 0, w, h);
          //					SDL_Rect r={0,0,w,h};
          //					SDL_RenderSetViewport(window.renderer,&r);
          break;
        }
        }
        break;
      }
      case SDL_QUIT:
        running = 0;
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
        case SDLK_ESCAPE:
          mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
          SDL_SetRelativeMouseMode(mouse_mode);
          break;
        case SDLK_w:
          net_state_to_send.keybits |= 1;
          break;
        case SDLK_a:
          net_state_to_send.keybits |= 2;
          break;
        case SDLK_s:
          net_state_to_send.keybits |= 4;
          break;
        case SDLK_d:
          net_state_to_send.keybits |= 8;
          break;
        case SDLK_q:
          net_state_to_send.keybits |= 16;
          break;
        case SDLK_e:
          net_state_to_send.keybits |= 32;
          break;
        case SDLK_o:
          net_state_to_send.keybits |= 64;
          break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          net_state_to_send.keybits &= ~1;
          break;
        case SDLK_a:
          net_state_to_send.keybits &= ~2;
          break;
        case SDLK_s:
          net_state_to_send.keybits &= ~4;
          break;
        case SDLK_d:
          net_state_to_send.keybits &= ~8;
          break;
        case SDLK_q:
          net_state_to_send.keybits &= ~16;
          break;
        case SDLK_e:
          net_state_to_send.keybits &= ~32;
          break;
        case SDLK_o:
          net_state_to_send.keybits &= ~64;
          break;
        case SDLK_SPACE:
          mouse_mode = mouse_mode ? SDL_FALSE : SDL_TRUE;
          SDL_SetRelativeMouseMode(mouse_mode);
          break;
        case SDLK_2: {
          gloid++;
          if (gloid == glos.size()) {
            gloid = 0;
          }
          //? getting pointer to volatile memory
          object_at(0)->node.glo = &glos.at(gloid);
          break;
        }
        case SDLK_3: {
          shader_program_ix++;
          if (shader_program_ix >= programs.size()) {
            shader_program_ix = 0;
          }
          break;
        }
        case SDLK_4:
          camera.type = !camera.type;
          break;
        case SDLK_5:
          printf("   camera: %f  %f  %f", camera.eye.x, camera.eye.y,
                 camera.eye.z);
          //						camera.ortho=!camera.ortho;
          break;
        }
        break;
      }
    }

    if (!use_net) {
      net_state_current[net_active_player_index].keybits =
          net_state_to_send.keybits;
    }
    if (game.keybits_ptr) {
      *game.keybits_ptr = net_state_current[net_active_player_index].keybits;
    }
    if (game.follow_ptr) {
      camera.lookat = game.follow_ptr->physics.position;
    }

    if (previous_active_program_ix != shader_program_ix) {
      printf(" * switching to program at index %u\n", shader_program_ix);
      {
        const program &p = programs.at(previous_active_program_ix);
        for (int ix : p.attributes) {
          printf("   * disable vertex attrib array %d\n", ix);
          glDisableVertexAttribArray(ix);
        }
      }
      {
        const program &p = programs.at(shader_program_ix);
        glUseProgram(p.id);
        for (int ix : p.attributes) {
          printf("   * enable vertex attrib array %d\n", ix);
          glEnableVertexAttribArray(ix);
        }
      }
      previous_active_program_ix = shader_program_ix;
    }

    fc.dt = use_net ? net_dt : metrics.fps.dt;
    fc.tick = frameno;

    frameno++;

    grid_clear();
    // objects_foa({ grid_add(o); });
    objects_foreach_allocated_all(_foreach_object_add_to_grid_);
    grid_update(&fc);
    grid_resolve_collisions(&fc);
    if (do_main_render) {
      main_render(&fc);
    }
    if (use_net) {
      net__at__frame_end();
    }
    metrics.objects_allocated = object_metrics_allocated;
    metrics.at_frame_end(stderr);
  }
  //---------------------------------------------------------------------free
  grid_free();
  glos_free();
  objects_free();
  materials_free();
  shader_free();
  window_free();
  sdl_free();
  if (use_net) {
    net_free();
  }
  metrics.free();
  puts(" * clean exit");
  return 0;
}
//----------------------------------------------------------------------------
