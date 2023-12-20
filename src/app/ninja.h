#pragma once
#include "types.h"
//--------------------------------------------------------------- ninja
//------------------------------------------------------------------------ def
static object ninja_def = {
    .node = node_def,
    .bounding_volume = {.radius = 1.4f, .scale = {1, 1, 1, 0}},
    .physics =
        {
            .position = {0, 0, 0, 0},
            .velocity = {0, 0, 0, 0},
            .angle = {0, 0, 0, 0},
            .angular_velocity = {0, 0, 0, 0},
        },
    .vtbl =
        {
            .init = NULL,
            .update = NULL,
            .collision = NULL,
            .render = _object_render_glo_,
            .free = NULL,
        },
    .type = {type_ninja},
    .alloc_bits_ptr = NULL,
    .part = {NULL, NULL, NULL, NULL},
};
//------------------------------------------------------------------ extension
typedef struct ninja_ext {
  part part;
  unsigned stars;
} ninja_ext;
//------------------------------------------------------------------ overrides
static void _ninja_part_init(object *, part *);
static void _ninja_update(object *, part *, framectx *);
static void _ninja_part_render(object *, part *, framectx *fc);
static void _ninja_free(object *, part *);
//----------------------------------------------------------------------- init
static ninja_ext ninja_part_def = {
    .part =
        (part){
            .init = NULL,
            .update = NULL,
            .render = NULL,
            .free = NULL,
        },
    .stars = 7,
};
//----------------------------------------------------------------------- impl
static void _ninja_part_init(object *po, part *o) {}
static void _ninja_update(object *po, part *o, framectx *fc) {}
static void _ninja_part_render(object *po, part *o, framectx *fc) {}
static void _ninja_part_free(object *po, part *o) {}
// printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
//----------------------------------------------------------------------------

//----------------------------------------------------------------- alloc/free
inline static /*gives*/ object *ninja_alloc_def() {
  object *o = object_alloc(&ninja_def);
  ninja_ext *p = (ninja_ext *)malloc(sizeof(ninja_ext));
  *p = ninja_part_def;
  o->part[0] = (part *)p;
  return o;
}
//----------------------------------------------------------------------------
