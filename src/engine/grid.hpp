#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-07
// reviewed: 2024-01-10
// reviewed: 2024-01-16

#include "cell.hpp"

namespace glos {

class grid final {
  std::array<std::array<cell, grid_columns>, grid_rows> cells{};
  std::vector<std::array<cell, grid_columns> *> even_rows{};
  std::vector<std::array<cell, grid_columns> *> odd_rows{};

public:
  inline auto init() -> void {
    for (uint32_t i = 0; i < grid_rows; ++i) {
      if (i % 2) {
        odd_rows.push_back(&cells[i]);
      } else {
        even_rows.push_back(&cells[i]);
      }
    }
  }

  inline auto free() -> void {}

  // called from engine
  inline auto update() -> void {
    if (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::cbegin(even_rows),
                    std::cend(even_rows), [](auto row_array) {
                      for (cell &c : *row_array) {
                        c.update();
                      }
                    });

      std::for_each(std::execution::par_unseq, std::cbegin(odd_rows),
                    std::cend(odd_rows), [](auto row_array) {
                      for (cell &c : *row_array) {
                        c.update();
                      }
                    });
      return;
    }

    for (auto &row : cells) {
      for (cell &c : row) {
        c.update();
      }
    }
  }

  // called from engine
  inline auto resolve_collisions() -> void {
    if (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::begin(even_rows),
                    std::end(even_rows), [](auto row_array) {
                      for (cell &c : *row_array) {
                        c.resolve_collisions();
                      }
                    });

      std::for_each(std::execution::par_unseq, std::begin(odd_rows),
                    std::end(odd_rows), [](auto row_array) {
                      for (cell &c : *row_array) {
                        c.resolve_collisions();
                      }
                    });

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
    assert(o->bounding_radius < grid_cell_size / 2);

    constexpr float gw = grid_cell_size * grid_columns;
    constexpr float gh = grid_cell_size * grid_rows;

    float const r = o->bounding_radius;

    // calculate min max x and z in cell array
    float const xl = gw / 2 + o->position.x - r;
    float const xr = gw / 2 + o->position.x + r;
    float const zt = gh / 2 + o->position.z - r;
    float const zb = gh / 2 + o->position.z + r;

    uint32_t const xil = clamp(int32_t(xl / grid_cell_size), grid_columns - 1);
    uint32_t const xir = clamp(int32_t(xr / grid_cell_size), grid_columns - 1);
    uint32_t const zit = clamp(int32_t(zt / grid_cell_size), grid_rows - 1);
    uint32_t const zib = clamp(int32_t(zb / grid_cell_size), grid_rows - 1);

    // add to cells
    for (uint32_t z = zit; z <= zib; ++z) {
      for (uint32_t x = xil; x <= xir; ++x) {
        cell &c = cells[z][x];
        c.add(o);
      }
    }

    o->overlaps_cells = (xil != xir || zit != zib);
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

  inline auto debug_render_grid() const -> void {
    constexpr float gw = grid_cell_size * grid_columns;
    constexpr float gh = grid_cell_size * grid_rows;
    for (float z = -gh / 2; z <= gh / 2; z += grid_cell_size) {
      debug_render_wcs_line({-gw / 2, 0, z}, {gw / 2, 0, z}, {0, 1, 0, 1});
    }
    for (float x = -gw / 2; x <= gw / 2; x += grid_cell_size) {
      debug_render_wcs_line({x, 0, -gh / 2}, {x, 0, gh / 2}, {0, 1, 0, 1});
    }
  }

private:
  static inline auto clamp(int32_t const i, uint32_t const max) -> uint32_t {
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