#pragma once
#include "sdl.h"

class metrics final {
public:
  struct fps {
    uint32_t average_during_last_intervall = 0;
    uint32_t frame_count = 0;
    uint32_t calculation_intervall_ms = 0;
    uint32_t time_at_start_of_intervall_in_ms = 0;
    float dt = 0;
    uint64_t timer_frequency = 0;
    uint64_t timer_tick_at_start_of_frame = 0;
  } fps;

  unsigned tick = 0;
  size_t buffered_vertex_data = 0;
  unsigned buffered_texture_data = 0;
  unsigned objects_allocated = 0;
  unsigned glos_allocated = 0;
  unsigned objects_updated_prv_frame = 0;
  unsigned parts_updated_prv_frame = 0;
  unsigned objects_rendered_prv_frame = 0;
  unsigned parts_rendered_prv_frame = 0;
  unsigned average_fps = 0;
  unsigned triangles_rendered_prv_frame = 0;
  float net_lag_prv_frame = 0;
  unsigned collision_detections_prv_frame = 0;
  unsigned collision_detections_possible_prv_frame = 0;
  unsigned collision_detections_considered_prv_frame = 0;
  unsigned collision_grid_overlap_check = 0;

  inline void init() {}
  inline void free() {}
  inline void reset_timer() {
    fps.time_at_start_of_intervall_in_ms = SDL_GetTicks();
    fps.timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
    fps.timer_frequency = SDL_GetPerformanceFrequency();
  }
  inline void print_headers(FILE *f) {
    fprintf(f,
            " %6s  %6s  %4s  %7s  %6s  %5s  %5s  %5s  %5s  %6s  %6s  %6s  %6s  "
            "%8s  %5s  %8s  %8s\n",
            "ms", "dt", "fps", "netlag", "nobj", "colp", "colc", "colf",
            "olchk", "upd", "rend", "pupd", "prend", "gtri", "nglo", "arrbufs",
            "texbufs");
  }
  inline void print(FILE *f) {
    fprintf(
        f,
        " %06u  %0.4f  %04d  %0.5f  %06u  %05u  %05u  %05u  %05u  %06u  %06u "
        " %06u  %06u  %08u"
        "  %05u  %08lu  %08u\n",
        tick, fps.dt, average_fps, net_lag_prv_frame, objects_allocated,
        collision_detections_possible_prv_frame,
        collision_detections_considered_prv_frame,
        collision_detections_prv_frame, collision_grid_overlap_check,
        objects_updated_prv_frame, objects_rendered_prv_frame,
        parts_updated_prv_frame, parts_rendered_prv_frame,
        triangles_rendered_prv_frame, glos_allocated, buffered_vertex_data,
        buffered_texture_data);
  }

  inline void at_frame_begin() {
    if (!fps.frame_count) {
      reset_timer();
    }
    fps.frame_count++;
    objects_rendered_prv_frame = 0;
    objects_updated_prv_frame = 0;
    parts_rendered_prv_frame = 0;
    parts_updated_prv_frame = 0;
    triangles_rendered_prv_frame = 0;
    tick = SDL_GetTicks();
    net_lag_prv_frame = 0;
    collision_detections_prv_frame = 0;
    collision_detections_possible_prv_frame = 0;
    collision_detections_considered_prv_frame = 0;
    collision_grid_overlap_check = 0;
  }

  inline void at_frame_end(FILE *f) {
    {
      Uint64 t1 = SDL_GetPerformanceCounter();
      Uint64 dt_ticks = t1 - fps.timer_tick_at_start_of_frame;
      fps.timer_tick_at_start_of_frame = t1;
      fps.dt = (float)dt_ticks / (float)fps.timer_frequency;

      if (fps.dt > .1)
        fps.dt = .1f; //? magicnumber

      if (fps.dt == 0)
        fps.dt = .00001f; //? magicnumber
    }

    Uint32 t1 = SDL_GetTicks();
    Uint32 dt = t1 - fps.time_at_start_of_intervall_in_ms;

    if (dt < fps.calculation_intervall_ms) {
      return;
    }

    if (dt != 0) {
      fps.average_during_last_intervall = fps.frame_count * 1000 / dt;
    } else {
      fps.average_during_last_intervall = 0;
    }

    average_fps = fps.average_during_last_intervall;
    fps.time_at_start_of_intervall_in_ms = t1;
    fps.frame_count = 0;

    print(f);
  }
};

static metrics metrics{};
