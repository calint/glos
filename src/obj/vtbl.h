#pragma once
typedef struct object object;
typedef struct vtbl {
  void (*init)(object *);
  void (*update)(object *, framectx *);
  void (*collision)(object *, object *, framectx *);
  void (*render)(object *, framectx *);
  void (*free)(object *);
} vtbl;
#define vtbl_def                                                               \
  { 0, 0, 0, 0, 0 }
