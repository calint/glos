// reviewed: 2023-12-22

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>

// include order relevant
#include "app/configuration.hpp"
//
#include "engine/engine.hpp"
//

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  // check if this instance is server
  if (argc > 1 && *argv[1] == 's') {
    glos::net_server.init();
    glos::net_server.loop();
    glos::net_server.free();
    return 0;
  }

  puts("\nprogram glos\n");

  // this instance is client
  if (argc > 1 && *argv[1] == 'c') {
    // connect to server
    glos::net_enabled = true;
    if (argc > 2) {
      glos::net.host = argv[2];
    }
  }

  glos::engine.init();

  application_init();

  puts("");
  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "object", sizeof(glos::object));
  printf(": %15s : %-9ld :\n", "glo", sizeof(glos::glo));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");

  if (grid_threaded) {
    printf("\nthreaded grid on %d cores\n",
           std::thread::hardware_concurrency());
  }
  // sdl.play_path("music/ambience.mp3");

  glos::engine.run();

  application_free();
  glos::engine.free();
  return 0;
}
