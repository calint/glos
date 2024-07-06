class random_numbers final {
  std::mt19937 engine{random_number_generator_seed};
  std::uniform_int_distribution<int> dist{0, RAND_MAX};

public:
  inline auto next() -> int { return dist(engine); }
};