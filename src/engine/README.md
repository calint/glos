# overview
* `engine` coordinates the subsystems
* there is no explicit world class; however, the namespace `glos` contains instances of necessary objects to implement engine
* space is partitioned in a `grid` of `cells` containing objects
  - an `object` may overlap `grid` `cells`
  - `grid` runs `update` and `on_collision` on objects in a parallel and unsequenced way
* `camera` describes how the world is viewed in `window`
  - contains matrix used by `engine` at `render` that transforms world coordinates to view and screen space through projection 
* `object` has a renderable reference to `glob` using an index in `globs`
* `glob` is an opengl renderable object
  - to render a glob, a transform matrix from model to world coordinates is provided to `render`
* `glob` references, using indices, `materials` and `textures` that are created at `load`
* `glob` has a radius calculated at `load` and may be additionally bounded by a convex volume defined by `planes`
* `shaders` contains the opengl programs used for rendering
* `window` is an sdl2 window displaying the rendered result
* `sdl` handles initiation and shutdown of sdl2
* `net` and `net_server` handle single and multiplayer modes by synchronizing player signals rendering at the rate of slowest client

## multithreaded engine
* update and render run on different threads
* additionally, update thread runs `update` and `resolve_collisions` on `grid` `cells` on available cores in parallel and unsequenced order
* attention is needed when objects are interacting with other objects during `update` or `on_collision` because the object being interacted with might be running code from another thread
  - suggestion is to use a synchronized message queue that is handled at `update`
* guarantees given by engine:
  - only one thread is active at a time in an object's `update` or `on_collision`
  - collision between two objects is handled only once, considering that collision between same two objects can be detected on several threads if both objects overlap `grid` `cells``
* awareness that data races between update and render thread on `position`, `angle`, `scale`, `glob_ix` are assumed and ok

```
   update thread                    render thread
   -----------------------------    ---------------
   * clear grid cells               * wait
   * add objects to grid              ...
   * trigger render            ==>-----------------
   -----------------------------    * render
   * update objects in grid           ...
     cells using available cores      ...
   -----------------------------      ...
   * resolve collisions in grid       ...
     cells using available cores      ...
   -----------------------------      ...
   wait for render                    ...
   ...                                ...                              
   -----------------------------<== * trigger update
```
