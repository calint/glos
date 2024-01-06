#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04

namespace glos {
class metrics final {
public:
  struct fps {
    uint32_t calculation_interval_ms = 0;
    uint64_t time_at_start_of_interval_ms = 0;
    uint32_t frame_count = 0;
    uint32_t average_during_last_interval = 0;
    float dt = 0;
    uint64_t timer_tick_at_start_of_frame = 0;
  } fps;

  uint64_t ms = 0;
  size_t buffered_vertex_data = 0;
  size_t buffered_texture_data = 0;
  uint32_t allocated_objects = 0;
  uint32_t allocated_globs = 0;
  uint32_t rendered_objects = 0;
  uint32_t rendered_globs = 0;
  uint32_t average_fps = 0;
  uint32_t rendered_triangles = 0;
  float net_lag = 0;
  bool enable_print = true;

  inline void init() {}

  inline void free() {}

  inline void reset_timer() {
    fps.frame_count = 0;
    fps.time_at_start_of_interval_ms = SDL_GetTicks64();
    fps.timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
  }

  inline void print_headers(FILE *f) const {
    if (not enable_print) {
      return;
    }
    fprintf(f,
            " %6s  %7s  %4s  %7s  %6s  %6s  %6s  %8s  "
            "%4s  %8s  %8s\n",
            "ms", "dt", "fps", "netlag", "nobj", "drwo", "drwg", "drwt", "nglo",
            "vtxbufs", "texbufs");
  }

  inline void print(FILE *f) const {
    if (not enable_print) {
      return;
    }
    fprintf(f,
            " %06lu  %0.5f  %04u  %0.5f  %06u  %06u  %06u  %08u  %04u  "
            "%08zu  %08zu\n",
            ms, fps.dt, average_fps, net_lag, allocated_objects,
            rendered_objects, rendered_globs, rendered_triangles,
            allocated_globs, buffered_vertex_data, buffered_texture_data);
  }

  inline void at_frame_begin() {
    ++fps.frame_count;
    rendered_objects = 0;
    rendered_globs = 0;
    rendered_triangles = 0;
    ms = SDL_GetTicks64();
    net_lag = 0;
  }

  inline void at_frame_end(FILE *f) {
    {
      uint64_t const t1 = SDL_GetPerformanceCounter();
      uint64_t const dt_ticks = t1 - fps.timer_tick_at_start_of_frame;
      fps.dt = float(dt_ticks) / float(SDL_GetPerformanceFrequency());
      fps.timer_tick_at_start_of_frame = t1;

      // at rollover 'fps.dt' would be huge and gets capped to 0.1
      if (fps.dt > 0.1) {
        fps.dt = 0.1f; // minimum 10 fps
      }

      if (fps.dt == 0) {
        fps.dt = 0.000001f; // maximum 100'000 fps
      }
    }

    uint64_t const t1 = SDL_GetTicks64();
    uint64_t const dt = t1 - fps.time_at_start_of_interval_ms;

    if (dt < fps.calculation_interval_ms) {
      return;
    }

    if (dt != 0) {
      fps.average_during_last_interval = fps.frame_count * 1000 / dt;
    } else {
      fps.average_during_last_interval = 0;
    }

    average_fps = fps.average_during_last_interval;
    fps.time_at_start_of_interval_ms = t1;
    reset_timer();

    print(f);
  }
};

inline metrics metrics{};
} // namespace glos