# glos

experimental 3d multiplayer game framework using simple data layer 2 and opengl in c++

intention:
* multiplayer framework
  - synchronized client input signals
  - clients render at the rate of the slowest client
  - server synchronizes clients signals
* importing blender exported 'obj' files
* explore opengl rendering
* intuitive programming of application objects and logic
* application code written in `src/app/` and included by `src/main.cpp`
* collision detection
  - bounding radius
  - bounding boxes (todo)
  - bounding convex volumes (todo)
* collision reaction
  - sphere vs sphere (todo)
  - sphere vs box (todo)
  - box vs box (todo)
  - convex volume vs convex volume (todo)
* multithreaded grid of cells that partition world space
  - update and collision detection of objects (todo)

ubuntu packages used:
* sdl2 related packages
* opengl related packages 
* libglm-dev - opengl math
* libtbb-dev - parallel for_each
