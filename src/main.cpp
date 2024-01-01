// reviewed: 2023-12-22

#include "engine/engine.hpp"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {

  if (argc > 1 && *argv[1] == 's') {
    // instance is a server
    glos::net_server.init();
    glos::net_server.run();
    glos::net_server.free();
    return 0;
  }

  puts("\nprogram glos\n");

  if (argc > 1 && *argv[1] == 'c') {
    // multiplayer client, enable 'net'
    glos::net.enabled = true;
    if (argc > 2) {
      // host ip
      glos::net.host = argv[2];
    }
  }

  glos::engine.init();

  glos::engine.run();

  glos::engine.free();

  return 0;
}
