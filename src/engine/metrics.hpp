#pragma once
// reviewed: 2023-12-22

#include <SDL2/SDL.h>

namespace glos {
class metrics final {
public:
  struct fps {
    int average_during_last_intervall = 0;
    int frame_count = 0;
    int calculation_intervall_ms = 0;
    uint32_t time_at_start_of_intervall_ms = 0;
    float dt = 0;
    uint64_t timer_frequency = 0;
    uint64_t timer_tick_at_start_of_frame = 0;
  } fps;

  unsigned tick = 0;
  size_t buffered_vertex_data = 0;
  int buffered_texture_data = 0;
  int allocated_objects = 0;
  int allocated_glos = 0;
  int rendered_objects = 0;
  int rendered_glos = 0;
  int average_fps = 0;
  int rendered_triangles = 0;
  float net_lag = 0;

  inline void init() {}

  inline void free() {}

  inline void reset_timer() {
    fps.time_at_start_of_intervall_ms = SDL_GetTicks();
    fps.timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
    fps.timer_frequency = SDL_GetPerformanceFrequency();
  }

  inline void print_headers(FILE *f) {
    fprintf(f,
            " %6s  %7s  %4s  %7s  %6s  %6s  %6s  %8s  "
            "%4s  %8s  %8s\n",
            "ms", "dt", "fps", "netlag", "nobj", "drwo", "drwg", "drwt", "nglo",
            "vtxbufs", "texbufs");
  }

  inline void print(FILE *f) {
    fprintf(f,
            " %06u  %0.5f  %04d  %0.5f  %06u  %06u  %06u  %08u  %04u  "
            "%08zu  %08u\n",
            tick, fps.dt, average_fps, net_lag, allocated_objects,
            rendered_objects, rendered_glos, rendered_triangles, allocated_glos,
            buffered_vertex_data, buffered_texture_data);
  }

  inline void at_frame_begin() {
    if (!fps.frame_count) {
      reset_timer();
    }
    fps.frame_count++;
    rendered_objects = 0;
    rendered_glos = 0;
    rendered_triangles = 0;
    tick = SDL_GetTicks();
    net_lag = 0;
  }

  inline void at_frame_end(FILE *f) {
    {
      const Uint64 t1 = SDL_GetPerformanceCounter();
      const Uint64 dt_ticks = t1 - fps.timer_tick_at_start_of_frame;
      fps.timer_tick_at_start_of_frame = t1;
      fps.dt = (float)dt_ticks / (float)fps.timer_frequency;

      if (fps.dt > .1) {
        fps.dt = .1f; // minimum 10 fps
      }

      if (fps.dt == 0) {
        fps.dt = .00001f; // maximum 100000 fps
      }
    }

    const Uint32 t1 = SDL_GetTicks();
    const int dt = int(t1 - fps.time_at_start_of_intervall_ms);

    if (dt < fps.calculation_intervall_ms) {
      return;
    }

    if (dt != 0) {
      fps.average_during_last_intervall = fps.frame_count * 1000 / dt;
    } else {
      fps.average_during_last_intervall = 0;
    }

    average_fps = fps.average_during_last_intervall;
    fps.time_at_start_of_intervall_ms = t1;
    fps.frame_count = 0;

    print(f);
  }
};

inline metrics metrics{};
} // namespace glos