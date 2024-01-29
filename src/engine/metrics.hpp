#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class metrics final {
public:
  struct fps {
    uint32_t calculation_interval_ms = 1000;
    uint64_t time_at_start_of_interval_ms = 0;
    uint32_t frame_count = 0;
    uint32_t average_during_last_interval = 0;
  } fps{};

  uint64_t timer_tick_at_start_of_frame = 0;
  uint64_t ms = 0;
  float dt = 0;
  size_t buffered_vertex_data = 0;
  size_t buffered_texture_data = 0;
  uint32_t allocated_objects = 0;
  uint32_t allocated_globs = 0;
  uint32_t rendered_objects = 0;
  uint32_t rendered_globs = 0;
  uint32_t rendered_triangles = 0;
  uint64_t update_begin_tick = 0;
  float update_pass_ms = 0;
  uint64_t render_begin_tick = 0;
  float render_pass_ms = 0;
  float net_lag = 0;
  bool enable_print = true;

  inline void init() {}

  inline void free() {}

  inline void begin() {
    timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
    fps.time_at_start_of_interval_ms = SDL_GetTicks64();
  }

  inline void print_headers(FILE *f) const {
    if (not enable_print) {
      return;
    }

    fprintf(f, " %7s  %7s  %5s  %7s  %7s  %7s  %6s  %6s  %6s  %9s\n", "ms",
            "dt_ms", "fps", "drw_ms", "upd_ms", "netlag", "nobj", "drw_o",
            "drw_g", "drw_t");
  }

  inline void print(FILE *f) const {
    if (not enable_print) {
      return;
    }

    fprintf(
        f, " %07lu  %7.4f  %05u  %7.4f  %7.4f  %7.4f  %06u  %06u  %06u  %09u\n",
        ms, dt * 1000, fps.average_during_last_interval, render_pass_ms,
        update_pass_ms, net_lag, allocated_objects, rendered_objects,
        rendered_globs, rendered_triangles);
  }

  inline void update_begin() {
    update_begin_tick = SDL_GetPerformanceCounter();
  }

  inline void update_end() {
    uint64_t const tick = SDL_GetPerformanceCounter();
    update_pass_ms = float(tick - update_begin_tick) * 1000 /
                     float(SDL_GetPerformanceFrequency());
  }

  inline void render_begin() {
    render_begin_tick = SDL_GetPerformanceCounter();
  }

  inline void render_end() {
    uint64_t const tick = SDL_GetPerformanceCounter();
    render_pass_ms = float(tick - render_begin_tick) * 1000 /
                     float(SDL_GetPerformanceFrequency());
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
      uint64_t const dt_ticks = t1 - timer_tick_at_start_of_frame;
      dt = float(dt_ticks) / float(SDL_GetPerformanceFrequency());
      timer_tick_at_start_of_frame = t1;

      if (dt > 0.1) {
        dt = 0.1f; // minimum 10 fps
      } else if (dt == 0) {
        dt = 0.000001f; // maximum 100'000 fps
      }
    }

    uint64_t const t1 = SDL_GetTicks64();
    uint64_t const dt_interval = t1 - fps.time_at_start_of_interval_ms;

    if (dt_interval < fps.calculation_interval_ms) {
      return;
    }

    fps.average_during_last_interval =
        fps.frame_count * uint32_t(1000) / uint32_t(dt_interval);
    fps.time_at_start_of_interval_ms = t1;
    fps.frame_count = 0;

    print(f);
  }
};

static metrics metrics{};
} // namespace glos