#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-07
// reviewed: 2024-01-10
// reviewed: 2024-01-16

#include "cell.hpp"
// #include "task_thread_pool.hpp"

namespace glos {

class grid final {
  std::array<std::array<cell, grid_columns>, grid_rows> cells{};
  // task_thread_pool::task_thread_pool pool{};

public:
  inline auto init() -> void {}

  inline auto free() -> void {}

  // called from engine
  inline auto update() -> void {
    if (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::cbegin(cells),
                    std::cend(cells), [](auto const &row) {
                      for (cell const &c : row) {
                        c.update();
                      }
                    });

      // for (auto const &row : cells) {
      //   pool.submit_detach([&row]() {
      //     for (cell const &c : row) {
      //       c.update();
      //     }
      //   });
      // }
      // pool.wait_for_tasks();

      return;
    }

    for (auto const &row : cells) {
      for (cell const &c : row) {
        c.update();
      }
    }
  }

  // called from engine
  inline auto resolve_collisions() -> void {
    if (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [](auto &row) {
                      for (cell &c : row) {
                        c.resolve_collisions();
                      }
                    });

      // for (auto const &row : cells) {
      //   pool.submit_detach([&row]() {
      //     for (cell const &c : row) {
      //       c.resolve_collisions();
      //     }
      //   });
      // }
      // pool.wait_for_tasks();

      return;
    }

    for (auto &row : cells) {
      for (cell &c : row) {
        c.resolve_collisions();
      }
    }
  }

  // called from engine
  inline auto render() const -> void {
    for (auto const &row : cells) {
      for (cell const &c : row) {
        c.render();
      }
    }
  }

  // called from engine
  inline auto clear() -> void {
    for (auto &row : cells) {
      for (cell &c : row) {
        c.clear();
      }
    }
  }

  // called from engine
  inline auto add(object *o) -> void {
    constexpr float gw = grid_cell_size * grid_columns;
    constexpr float gh = grid_cell_size * grid_rows;

    float const r = o->bounding_radius;

    // calculate min max x and z in cell array
    float const xl = gw / 2 + o->position.x - r;
    float const xr = gw / 2 + o->position.x + r;
    float const zt = gh / 2 + o->position.z - r;
    float const zb = gh / 2 + o->position.z + r;

    uint32_t const xil = clamp(int(xl / grid_cell_size), grid_columns - 1);
    uint32_t const xir = clamp(int(xr / grid_cell_size), grid_columns - 1);
    uint32_t const zit = clamp(int(zt / grid_cell_size), grid_rows - 1);
    uint32_t const zib = clamp(int(zb / grid_cell_size), grid_rows - 1);

    // add to cells
    for (uint32_t z = zit; z <= zib; ++z) {
      for (uint32_t x = xil; x <= xir; ++x) {
        cell &c = cells[z][x];
        c.add(o);
      }
    }

    o->overlaps_cells = (xil != xir or zit != zib);
  }

  inline auto print() const -> void {
    for (auto const &row : cells) {
      for (cell const &c : row) {
        printf(" %04zu ", c.objects_count());
      }
      printf("\n");
    }
    printf("------------------------\n");
  }

private:
  static inline auto clamp(int const i, uint32_t const max) -> uint32_t {
    if (i < 0) {
      return 0;
    }
    if (uint32_t(i) > max) {
      return max;
    }
    return uint32_t(i);
  }
};

static grid grid{};
} // namespace glos