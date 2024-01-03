# glos

experimental 3d multiplayer game framework using simple data layer 2 and opengl es3 in c++ 20

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
* game code written in `src/application/`
* collision detection
  - sphere vs sphere
  - sphere vs convex volume
  - convex volume vs convex volume
* collision reaction
  - sphere vs sphere
  - sphere vs convex volume _(todo)_
  - convex volume vs convex volume _(todo)_

ubuntu packages used:
* libglm-dev: opengl math
* libtbb-dev: parallel unsequenced `for_each`
* sdl2 related packages
* opengl related packages

## sample game
* multiplayer enabled proof of concept
* developed to test the framework
* howto keys
  - w: thrust
  - a: rotate left
  - d: rotate right
  - j: fire
  - F1: print cells
  - F2: turn on/off collisions
  - F3: turn on/off render
  - F4: cycle through shaders
  - F5: turn on/off normals
  - F6: turn on/off bounding spheres
  - F7: turn on/off hud

![starting multiplayer mode](https://github.com/calint/glos/assets/1920811/ed27167b-525e-4696-8a0c-2cb2fb52ec14)

![multiplayer mode](https://github.com/calint/glos/assets/1920811/697dbe55-b3b0-41ef-8bfa-ab67666291c8)

## todo
* tidy the source in `src/application`
* tidy the source in `src/engine`
* another sample game in a sandbox-like environment
* grid cells and objects culling to view frustum
* and much more in ![todo.txt](https://github.com/calint/glos/blob/master/todo.txt)
