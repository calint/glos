#pragma once
#include "../lib.h"
#include "../obj/object.h"
#include "../obj/part.h"
#include "cell.h"

#define grid_cell_size 5
#define grid_ncells_wide 1
#define grid_ncells_high 1
#define grid_ncells grid_ncells_wide *grid_ncells_high

static cell cells[grid_ncells_high][grid_ncells_wide];

inline static void grid_init() {}

inline static void grid_update(framectx *fc) {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  //	printf("-------------------------\n");
  while (i--) {
    cell_update(p, fc);
    p++;
  }
}

inline static void grid_resolve_collisions(framectx *fc) {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    cell_resolve_collisions(p++, fc);
  }
}

inline static void grid_render(framectx *fc) {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    cell_render(p++, fc);
  }
}

inline static void grid_clear() {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    cell_clear(p++);
  }
}

inline static int clamp(int i, int min, int max_plus_one) {
  if (i < min)
    return min;

  if (i >= max_plus_one)
    return max_plus_one - 1;

  return i;
}

inline static void grid_add(object *o) {
  if (grid_ncells == 1) {
    cell_add_object(&cells[0][0], o);
    return;
  }

  //	printf("[ grid ] add %s %p\n",o->n.glo_ptr->name.data,(void*)o);
  const float gw = grid_cell_size * grid_ncells_wide;
  const float gh = grid_cell_size * grid_ncells_high;

  const float r = o->bounding_volume.radius;

  const float xl = gw / 2 - o->physics.position.x - r;
  const float xr = gw / 2 - o->physics.position.x + r;
  const float zl = gh / 2 - o->physics.position.z - r;
  const float zr = gh / 2 - o->physics.position.z + r;

  int xil = (int)(xl / grid_cell_size);
  xil = clamp(xil, 0, grid_ncells_wide);

  int xir = (int)((xr) / grid_cell_size);
  xir = clamp(xir, 0, grid_ncells_wide);

  int zil = (int)(zl / grid_cell_size);
  zil = clamp(zil, 0, grid_ncells_high);

  int zir = (int)((zr) / grid_cell_size);
  zir = clamp(zir, 0, grid_ncells_high);

  for (int z = zil; z <= zir; z++) {
    for (int x = xil; x <= xir; x++) {
      cell *c = &cells[z][x];
      cell_add_object(c, o);
    }
  }

  if (xil == xir && zil == zir) {
    clear_bit(&o->grid_ifc.bits, grid_ifc_overlaps);
  } else {
    set_bit(&o->grid_ifc.bits, grid_ifc_overlaps);
  }
}

inline static void grid_print() {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    //		cell_print(p++);
    printf(" %d ", p++->objrefs.count);
    if (!(i % grid_ncells_wide))
      printf("\n");
  }
  printf("\n------------------------\n");
}

inline static void grid_print_list() {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    printf(" cell[%p]:\n", (void *)p);
    dynp_foa(&p->objrefs, {
      object *oo = o;
      printf("   %s\n", oo->name.data);
    });
    p++;
  }
}

inline static void grid_free() {
  cell *p = &cells[0][0];
  unsigned i = grid_ncells;
  while (i--) {
    cell_free(p++);
  }
}
