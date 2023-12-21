#pragma once
class part final {
public:
  void (*init)(object *, part *){};
  void (*update)(object *, part *, const framectx *){};
  void (*render)(object *, part *, const framectx *){};
  void (*free)(object *, part *){};
};
