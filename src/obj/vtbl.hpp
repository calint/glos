#pragma once
class object;
class vtbl final {
public:
  void (*init)(object *){};
  void (*update)(object *, framectx *){};
  void (*collision)(object *, object *, framectx *){};
  void (*render)(object *, framectx *){};
  void (*free)(object *){};
};
