#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-07

#include "cell.hpp"

namespace glos {
class grid final {
public:
  inline void init() {}

  inline void free() {}

  // called from engine
  inline void update() const {
    if constexpr (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [](auto const &row) {
                      for (cell const &c : row) {
                        c.update();
                      }
                    });
      return;
    }

    for (auto const &row : cells) {
      for (cell const &c : row) {
        c.update();
      }
    }
  }

  // called from engine
  inline void resolve_collisions() const {
    if constexpr (threaded_grid) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [](auto const &row) {
                      for (cell const &c : row) {
                        c.resolve_collisions();
                      }
                    });
      return;
    }

    for (auto const &row : cells) {
      for (cell const &c : row) {
        c.resolve_collisions();
      }
    }
  }

  // called from engine
  inline void render() const {
    for (auto const &row : cells) {
      for (cell const &c : row) {
        c.render();
      }
    }
  }

  // called from engine
  inline void clear() {
    for (auto &row : cells) {
      for (cell &c : row) {
        c.clear();
      }
    }
  }

  // called from engine
  inline void add(object *o) {
    o->clear_flags();
    // note. flags need to be cleared before object 'update' or
    // 'resolve_collisions' and this is an opportunity for that without a
    // separate pass in the loop

    if (grid_columns == 1 and grid_rows == 1) {
      // special case
      cells[0][0].add(o);
      return;
    }

    constexpr float gw = grid_cell_size * grid_columns;
    constexpr float gh = grid_cell_size * grid_rows;

    float const r = o->radius;

    // calculate min max x and z in cell array
    float const xl = gw / 2 + o->position.x - r;
    float const xr = gw / 2 + o->position.x + r;
    float const zt = gh / 2 + o->position.z - r;
    float const zb = gh / 2 + o->position.z + r;

    unsigned const xil = clamp(int(xl / grid_cell_size), grid_columns);
    unsigned const xir = clamp(int(xr / grid_cell_size), grid_columns);
    unsigned const zit = clamp(int(zt / grid_cell_size), grid_rows);
    unsigned const zib = clamp(int(zb / grid_cell_size), grid_rows);

    // add to cells
    for (unsigned z = zit; z <= zib; ++z) {
      for (unsigned x = xil; x <= xir; ++x) {
        cell &c = cells[z][x];
        c.add(o);
      }
    }

    if (xil != xir or zit != zib) {
      o->set_overlaps_cells();
    }
  }

  inline void print() const {
    for (auto const &row : cells) {
      for (cell const &c : row) {
        printf(" %04zu ", c.objects_count());
      }
      printf("\n");
    }
    printf("------------------------\n");
  }

private:
  std::array<std::array<cell, grid_columns>, grid_rows> cells{};

  static inline auto clamp(int const i, int const max_plus_one) -> unsigned {
    if (i < 0) {
      return 0;
    } else if (i >= max_plus_one) {
      return unsigned(max_plus_one - 1);
    }
    return unsigned(i);
  }
};

inline grid grid{};
} // namespace glos