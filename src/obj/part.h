#pragma once
typedef struct part {
  void (*init)(object *, part *);
  void (*update)(object *, part *, framectx *);
  void (*render)(object *, part *, framectx *);
  void (*free)(object *, part *);
} part;
