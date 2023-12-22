#pragma once
// reviewed: 2023-12-22

#include "../obj/object.hpp"
#include "cell.hpp"

class grid final {
  static constexpr unsigned ncells = grid_ncells_wide * grid_ncells_high;

public:
  cell cells[grid_ncells_high][grid_ncells_wide];

  inline void init() {}

  inline void free() {}

  inline void update(const frame_ctx &fc) {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->update(fc);
      p++;
    }
  }

  inline void resolve_collisions(const frame_ctx &fc) {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->resolve_collisions(fc);
      p++;
    }
  }

  inline void render(const frame_ctx &fc) {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->render(fc);
      p++;
    }
  }

  inline void clear() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      p->clear();
      p++;
    }
  }

  inline void add(object *o) {
    if (ncells == 1) {
      cells[0][0].add(o);
      return;
    }

    //	printf("[ grid ] add %s %p\n",o->n.glo_ptr->name.data,(void*)o);
    const float gw = grid_cell_size * grid_ncells_wide;
    const float gh = grid_cell_size * grid_ncells_high;

    const float r = o->volume.radius;

    const float xl = gw / 2 + o->physics.position.x - r;
    const float xr = gw / 2 + o->physics.position.x + r;
    const float zl = gh / 2 + o->physics.position.z - r;
    const float zr = gh / 2 + o->physics.position.z + r;

    int xil = (int)(xl / grid_cell_size);
    int xir = (int)((xr) / grid_cell_size);
    int zil = (int)(zl / grid_cell_size);
    int zir = (int)((zr) / grid_cell_size);

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

    if (xil == xir and zil == zir) {
      clear_bit(o->grid_ifc.bits, grid_ifc_overlaps);
    } else {
      set_bit(o->grid_ifc.bits, grid_ifc_overlaps);
    }
  }

  inline void print() {
    cell *p = cells[0];
    unsigned i = ncells;
    while (i--) {
      printf(" %zu ", p->objects.size());
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
