# table of contents
* `main.cpp` entry point for instantiating and running the engine
* `engine/` engine code in namespace `glos`
* `application/` user code that interfaces with the engine

## rationale
* user should only write code in `application/`
* with time and applications, engine will adapt to handle encountered scenarios
* engine code is simple and can be easily adapted to the custom needs of an application

## disclaimers
* source in `hpp` files
  - gives compiler opportunity to optimize in a broader context
  - increases compile time and is not scalable; however, this framework is intended for smallish applications
* use of `static` storage
  - gives compiler opportunity to optimize
* include order relevant
  - engine includes subsystems in dependency order
  - most `hpp` files also define a global instance or instances of the defined type 
* `inline` functions
  - all functions are requested to be inlined assuming compilers won't adhere to the hint when it does not make sense, such as big functions called from multiple locations
  - functions called from one location should be inlined
* right to left notation `Type const &inst` instead of `const Type &inst`
* for consistency, constants are written after the type such as `char const *ptr` instead of `const char *ptr` and `float const x` instead of `const float x`
* types are spelled out instead of using `auto` for readability
* `auto` is used when the type is too verbose, such as iterators
* `unsigned` is used where negative values do not make sense
* liberal use of public members in classes
  - public members are moved to private scope when not used outside the class
  - when change to a public member triggers other actions, accessors are written, and member moved to private scope
* `++i` instead of `i++`
  - for consistency with incrementing iterators, all increments and decrements are done in prefix