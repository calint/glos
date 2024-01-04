# overview
* update and render run on different threads
* additionally, update thread runs `update` and `resolve_collisions` on grid cells on available cores in parallel and unsequenced order
* attention is needed when objects are interacting with other objects during `update` or `on_collision` because the object being interacted with might be running code from another thread
  - suggestion is to use a synchronized message queue that is handled at `update`
* guarantees given by engine:
  - only one thread is active at a time in an object's `update` or `on_collision`
  - collision between two objects is handled only once, considering that collision between same two objects can be detected on several threads if both objects overlap grid cells
* awareness that data races between update and render thread on `position`, `angle`, `scale`, `glob_ix` are assumed and ok

```
   update thread                    render thread
   -----------------------------    ---------------
   * clear grid cells               * wait
   * add objects to grid              ...
   * trigger render ------------>-------------------
   -----------------------------
   * update objects in grid         * render
     cells using available cores      ...
   -----------------------------      ...
   * resolve collisions in grid       ...
     cells using available cores      ...
   -----------------------------      ...
   wait for render                    ...
   ...                                ...                              
   -----------------------------<-- * trigger update
   loop                             loop
```
