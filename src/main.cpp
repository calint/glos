// reviewed: 2023-12-22

#include "engine/engine.hpp"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  // check if this instance is server
  if (argc > 1 && *argv[1] == 's') {
    glos::net_server.init();
    glos::net_server.run();
    glos::net_server.free();
    return 0;
  }

  puts("\nprogram glos\n");

  // this instance is client
  if (argc > 1 && *argv[1] == 'c') {
    // connect to server
    glos::net.enabled = true;
    if (argc > 2) {
      glos::net.host = argv[2];
    }
  }

  glos::engine.init();

  glos::engine.run();

  glos::engine.free();
  
  return 0;
}
