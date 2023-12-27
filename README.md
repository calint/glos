# glos

experimental 3d multiplayer game framework using simple data layer 2 and opengl in c++

intention:
* multiplayer framework
  - synchronized client input signals
  - clients render at the rate of the slowest client
  - server synchronizes clients signals
* multithreaded grid of cells that partition world space
  - update and collision detection of objects
* importing blender exported `obj` files
* explore legacy opengl rendering
* intuitive programming of game objects and logic
* game code written in `src/app/` which is included by `src/main.cpp`
* collision detection
  - bounding radius
  - bounding boxes _(todo)_
  - bounding convex volumes _(todo)_
* collision reaction
  - sphere vs sphere _(todo)_
  - sphere vs box _(todo)_
  - box vs box _(todo)_
  - convex volume vs convex volume _(todo)_

ubuntu packages used:
* libglm-dev: opengl math
* libtbb-dev: parallel unsequenced `for_each`
* sdl2 related packages
* opengl related packages 
