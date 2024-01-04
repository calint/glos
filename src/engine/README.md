# overview
* `engine` coordinates the subsystems
* there is no explicit world class; however, namespace `glos` contains instances of necessary objects to implement engine
* space is partitioned in a `grid` of `cells` containing objects
  - `object` may overlap `grid` `cells`
  - `grid` runs an `update` then a `resolve_collisions` pass on `cells` in a parallel and unsequenced way
  - `cells` call `object` `update` in the update pass and `on_collision` when collision is detected in `resolve_collision` pass
  - `object` `update` is called by only one thread
  - `object` `on_collision` is called by only one thread and once for each collision with other `object`
* `object` refers to a 3d model, `glob`, using an index in `globs`
  - has a state such as `position`, `angle`, `scale` etc
* `glob`
  - rendered using opengl and a transform matrix for model to world coordinates provided to its `render`
  - using indices, references `materials` and `textures` that are created at `load`
  - has a radius calculated at `load` and may be additionally bounded by a convex volume defined by `planes`
* `planes` can detect collision with spheres and other `planes`
  - collision with spheres is done by checking if distance from sphere center to all planes is less than radius or negative
  - collision with other `planes` is done by checking if any point in `planes` A is behind all `planes` B or vice versa
* `material` is stored in `materials` and are unique to a `glob`
* `texture` is stored in `textures` and can be shared by multiple `globs`
* `camera` describes how the world is viewed in `window`
  - contains matrix used by `engine` at `render` to transform world coordinates to screen
* `window` is a double buffer sdl2 opengl window displaying the rendered result
* `sdl` handles initiation and shutdown of sdl2
* `shaders` contains the opengl programs used for rendering
* `hud` is a heads-up-display rendered after the view
* `net` and `net_server` handle single and multiplayer modes
  - synchronizes players signals
  - limits frame rate of all players to the slowest client
* `metrics` keeps track of frame time and statistics
* `token` is a helper to parse text

## multithreaded engine
* update and render run on different threads
* additionally, update thread runs `update` and `resolve_collisions` on `grid` `cells` on available cores in parallel and unsequenced order
* guarantees given by engine:
  - only one thread is active at a time in an object's `update` or `on_collision`
  - collision between two objects is handled only once, considering that collision between same two objects can be detected on several threads if both objects overlap `grid` `cells`
* awareness that data races between update and render thread on `object` `position`, `angle`, `scale`, `glob_ix` are assumed and ok
* attention is needed when objects are interacting with other objects during `update` or `on_collision` because the object being interacted with might be running code on another thread
  - suggestion is to use a synchronized message queue that is handled at `update`

```
   update thread                    render thread
   -----------------------------    -----------------
   * clear grid cells               * wait for update
   * add objects to grid              ...
   * trigger render            ==>-------------------
   * update objects in grid         * render
     cells using available cores      ...
   * resolve collisions in grid       ...
     cells using available cores      ...
   * wait for render                  ...
     ...                              ...                              
   -----------------------------<== * trigger update
```
