# glos

experimental 3d game framework using simple data layer 2 and opengl in c++

intention:
* multiplayer framework
  - synchronized client input signals
  - clients render at the rate of the slowest client
  - server synchronizes clients signals
* importing blender exported 'obj' files
* explore opengl rendering
* intuitive programming of game objects and logic
* collision detection
  - bounding radius

todo:
* collision detection
  - bounding boxes
  - bounding convex volumes
* collision reaction
  - sphere vs sphere
  - sphere vs box
  - box vs box
  - convex volume vs convex volume
* multithreaded grid of cells that partition world space
  - update and collision detection of objects

ubuntu packages used:
* sdl related packages
* opengl related packages
* libglm-dev
