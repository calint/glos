# table of contents
* `configuration.hpp` contains constants and global variables used by engine and game
* `application.cpp` contains game logic and implements interface to engine
  - `application_init()` called at init
  - `application_on_update_done()` called after objects have been updated and collisions resolved
  - `application_on_render_done()` called when render frame is done
  - `application_free()` called when application exits (can be empty assuming resources are reclaimed by operating system)
* `objects/` contains the game objects
  - `asteroid_large`
  - `asteroid_medium`
  - `asteroid_small`
  - `bullet`
  - `fragment`
  - `power_up`
  - `ship`
* `utils.hpp` utility functions used by game and game objects

## notes
* engine is multithreaded (see `configuration.hpp` setting `grid_threaded`)
* rendering is done in parallel with updating game objects
* updating objects and collision detections are done on available cores
* awareness of the nature of multithreaded application is recommended
* in smaller applications multithreaded mode might degrade performance
