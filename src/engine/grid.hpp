#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04

#include "cell.hpp"
// #include "task_thread_pool.hpp"
#include <execution>

namespace glos {
class grid final {
public:
  inline void init() {}

  inline void free() {}

  // called from main
  inline void update() {
    if (grid_threaded) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [](cell(&row)[grid_ncells_wide]) {
                      for (cell &c : row) {
                        c.update();
                      }
                    });

      // for (int i = 0; i < grid_ncells_high; i++) {
      //   pool.submit_detach([&fc, &row = cells[i]]() {
      //     for (int j = 0; j < grid_ncells_wide; j++) {
      //       row[j].update(fc);
      //     }
      //   });
      // }
      // pool.wait_for_tasks();

    } else {
      cell *p = cells[0];
      unsigned i = ncells;
      while (i--) {
        p->update();
        p++;
      }
    }
  }

  // called from main
  inline void resolve_collisions() {
    if (grid_threaded) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [](cell(&row)[grid_ncells_wide]) {
                      for (cell &c : row) {
                        c.resolve_collisions();
                      }
                    });

      // for (int i = 0; i < grid_ncells_high; i++) {
      //   pool.submit_detach([&fc, &row = cells[i]]() {
      //     for (int j = 0; j < grid_ncells_wide; j++) {
      //       row[j].resolve_collisions(fc);
      //     }
      //   });
      // }
      // pool.wait_for_tasks();

    } else {
      cell *p = cells[0];
      unsigned i = ncells;
      while (i--) {
        p->resolve_collisions();
        p++;
      }
    }
  }

  // called from main
  inline void render() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->render();
      p++;
    }
  }

  // called from main
  inline void clear() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->clear();
      p++;
    }
  }

  // called from main
  inline void add(object *o) {
    o->clear_flags();
    // note. flags need to be cleared before entering object 'update' or
    // 'resolve_collisions' and this is an opportunity for that without a
    // separate step in loop

    if (ncells == 1) {
      // special case
      cells[0][0].add(o);
      return;
    }

    constexpr float gw = grid_cell_size * grid_ncells_wide;
    constexpr float gh = grid_cell_size * grid_ncells_high;

    float const r = o->radius;

    // calculate min max x and z in cell array
    float const xl = gw / 2 + o->position.x - r;
    float const xr = gw / 2 + o->position.x + r;
    float const zt = gh / 2 + o->position.z - r;
    float const zb = gh / 2 + o->position.z + r;

    int const xil = clamp(int(xl / grid_cell_size), 0, grid_ncells_wide);
    int const xir = clamp(int(xr / grid_cell_size), 0, grid_ncells_wide);
    int const zit = clamp(int(zt / grid_cell_size), 0, grid_ncells_high);
    int const zib = clamp(int(zb / grid_cell_size), 0, grid_ncells_high);

    // add to grid
    for (int z = zit; z <= zib; z++) {
      for (int x = xil; x <= xir; x++) {
        cell *c = &cells[z][x];
        c->add(o);
      }
    }

    if (xil != xir or zit != zib) {
      o->set_overlaps_cells();
    }
  }

  inline void print() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      printf(" %d ", p->objects_count());
      if (!(i % grid_ncells_wide)) {
        printf("\n");
      }
      p++;
    }
    printf("------------------------\n");
  }

private:
  static constexpr int ncells = grid_ncells_wide * grid_ncells_high;

  // task_thread_pool::task_thread_pool pool{};

  cell cells[grid_ncells_high][grid_ncells_wide];

  inline static int clamp(int i, int min, int max_plus_one) {
    if (i < min) {
      return min;
    } else if (i >= max_plus_one) {
      return max_plus_one - 1;
    }
    return i;
  }
};

inline grid grid{};
} // namespace glos