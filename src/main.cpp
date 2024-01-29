// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-16

//
#include "engine/engine.hpp"
//
#include "application/application.hpp"

auto main(int argc, char *argv[]) -> int {

  if (argc > 1 and *argv[1] == 's') {
    // instance is server
    glos::net_server.init();
    glos::net_server.run();
    glos::net_server.free();
    return 0;
  }

  // instance is client
  puts("\nprogram glos\n");

  if (argc > 1 and *argv[1] == 'c') {
    // multiplayer client, enable 'net'
    glos::net.enabled = true;
    if (argc > 2) {
      // server ip
      glos::net.host = argv[2];
    }
  }

  glos::engine.init();

  glos::engine.run();

  glos::engine.free();

  return 0;
}
