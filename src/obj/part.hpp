#pragma once
class part final {
public:
  void (*init)(object *, part *){};
  void (*update)(object *, part *, framectx *){};
  void (*render)(object *, part *, framectx *){};
  void (*free)(object *, part *){};
};
