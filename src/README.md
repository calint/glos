# table of contents
* `main.cpp` entry point instantiating and running the engine
* `engine/` engine code in namespace 'glos'
* `application/` user code that interfaces with the engine

## rationale
* user should only have to write the code in `application/`
* with time and applications engine will adapt to handle encountered scenarios
* engine code is simple and can easily be adapted to custom needs of application

## disclaimers
* source in `hpp` files
  - gives compiler opportunity to optimize in a greater context
  - increases compile time and is not scalable but this framework is meant for smallish applications
* use of `static` storage
  - gives compiler opportunity to optimize
* include order relevant
  - engine includes subsystems in dependency order
  - most `hpp` files also define a global object representing an instance or instances of the defined type 
* `inline` functions
  - all functions are requested to be inlined assuming compilers won't adhere to the hint when it does not make sense, such as big functions called from multiple locations
  - functions called from one location should be inlined
* `Type const &inst` instead `const Type &inst`
  - adheres to reading type from right to left
* for consistency `const char *ptr` and other constant are written as `char const *ptr`
* types are spelled out instead of `auto` for readability
* `auto` is used when the type is too verbose, such as iterators
* `unsigned` is used where negative values do not make sense
* liberal use of public members in classes
  - public members are gradually moved to private scope when not used outside the class or changes need to trigger other actions
* `++i` instead of `i++`
  - for consistency with incrementing of iterators all increments and decrements are done in prefix
