#pragma once
class object;
class vtbl final {
public:
  void (*init)(object *){};
  void (*update)(object *, const framectx *){};
  void (*collision)(object *, object *, const framectx *){};
  void (*render)(object *, const framectx *){};
  void (*free)(object *){};
};
