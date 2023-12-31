#pragma once
// reviewed: 2023-12-22

#include "cell.hpp"
// #include "task_thread_pool.hpp"
#include <execution>

namespace glos {
class grid final {
  static constexpr int ncells = grid_ncells_wide * grid_ncells_high;
  // task_thread_pool::task_thread_pool pool{};

public:
  cell cells[grid_ncells_high][grid_ncells_wide];

  inline void init() {}

  inline void free() {}

  // called from main
  inline void update(frame_context const &fc) {
    if (grid_threaded) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [&fc](cell(&row)[grid_ncells_wide]) {
                      for (cell &c : row) {
                        c.update(fc);
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
        p->update(fc);
        p++;
      }
    }
  }

  // called from main
  inline void resolve_collisions(frame_context const &fc) {
    if (grid_threaded) {
      std::for_each(std::execution::par_unseq, std::begin(cells),
                    std::end(cells), [&fc](cell(&row)[grid_ncells_wide]) {
                      for (cell &c : row) {
                        c.resolve_collisions(fc);
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
        p->resolve_collisions(fc);
        p++;
      }
    }
  }

  // called from main
  inline void render(unsigned const render_frame_num) {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->render(render_frame_num);
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
    if (ncells == 1) {
      cells[0][0].add(o);
      return;
    }

    constexpr float gw = grid_cell_size * grid_ncells_wide;
    constexpr float gh = grid_cell_size * grid_ncells_high;

    const float r = o->volume.radius;

    const float xl = gw / 2 + o->physics.position.x - r;
    const float xr = gw / 2 + o->physics.position.x + r;
    const float zl = gh / 2 + o->physics.position.z - r;
    const float zr = gh / 2 + o->physics.position.z + r;

    int xil = (int)(xl / grid_cell_size);
    int xir = (int)(xr / grid_cell_size);
    int zil = (int)(zl / grid_cell_size);
    int zir = (int)(zr / grid_cell_size);

    xil = clamp(xil, 0, grid_ncells_wide);
    xir = clamp(xir, 0, grid_ncells_wide);
    zil = clamp(zil, 0, grid_ncells_high);
    zir = clamp(zir, 0, grid_ncells_high);

    for (int z = zil; z <= zir; z++) {
      for (int x = xil; x <= xir; x++) {
        cell *c = &cells[z][x];
        c->add(o);
      }
    }

    o->clear_flags();
    // note. flags need to be cleared before entering 'update' or
    // 'resolve_collisions'
    if (xil != xir or zil != zir) {
      o->set_overlaps_cells();
    }
  }

  inline void print() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      printf(" %zu ", p->ols.size());
      if (!(i % grid_ncells_wide)) {
        printf("\n");
      }
      p++;
    }
    printf("------------------------\n");
  }

private:
  inline static int clamp(int i, int min, int max_plus_one) {
    if (i < min) {
      return min;
    }

    if (i >= max_plus_one)
      return max_plus_one - 1;

    return i;
  }
};

static grid grid{};
} // namespace glos