#pragma once
// reviewed: 2023-12-23

#include "net.hpp"

namespace glos {
class net_server final {
public:
  uint16_t port = 8085;
  net_state state[net_players + 1];
  // note. state[0] is used by server to broadcast to all clients
  //       e.g. delta time for frame (dt)

private:
  int server_fd;
  int clients_fd[net_players + 1];

public:
  inline void init() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      fprintf(stderr, "\n%s:%u: create socket failed\n", __FILE__, __LINE__);
      std::abort();
    }
    {
      int flag = 1;
      int result =
          setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int));
      if (result < 0) {
        fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__,
                __LINE__);
        std::abort();
      }
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "\n%s:%u: bind failed\n", __FILE__, __LINE__);
      std::abort();
    }

    if (listen(server_fd, net_players + 1) == -1) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      perror("");
      std::abort();
    }

    printf(" * waiting for %d players to connect on port %d\n", net_players,
           port);

    for (unsigned i = 1; i < net_players + 1; i++) {
      clients_fd[i] = accept(server_fd, NULL, NULL);

      if (clients_fd[i] == -1) {
        fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
        perror("");
        std::abort();
      }

      int flag = 1;
      int result = setsockopt(clients_fd[i], IPPROTO_TCP, TCP_NODELAY, &flag,
                              sizeof(int));
      if (result == -1) {
        fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
        perror("");
        std::abort();
      }

      printf(" * player %d of %d connected\n", i, net_players);
    }

    printf(" * sending start\n");

    // send the assigned player index to clients
    for (uint32_t i = 1; i < net_players + 1; i++) {
      if (write(clients_fd[i], &i, sizeof(uint32_t)) == -1) {
        fprintf(stderr, "\n%s:%u: could not start player %u: ", __FILE__,
                __LINE__, i);
        perror("");
        std::abort();
      }
    }
  }

  inline void loop() {
    printf(" * entering loop\n");
    constexpr size_t state_read_size = sizeof(net_state);
    uint64_t t0 = SDL_GetPerformanceCounter();
    while (true) {
      for (unsigned i = 1; i < net_players + 1; i++) {
        const ssize_t n = recv(clients_fd[i], &state[i], state_read_size, 0);
        if (n == -1) {
          fprintf(stderr, "\n%s:%u: player %u: ", __FILE__, __LINE__, i);
          perror("");
          std::abort();
        }
        if (n == 0) {
          fprintf(stderr, "\n%s:%u: player %u: disconnected\n", __FILE__,
                  __LINE__, i);
          std::abort();
        }
        if ((unsigned)n != state_read_size) {
          fprintf(stderr, "\n%s:%u: player %u: read was incomplete\n", __FILE__,
                  __LINE__, i);
          std::abort();
        }
      }
      const uint64_t t1 = SDL_GetPerformanceCounter();
      const float dt = (float)(t1 - t0) / (float)SDL_GetPerformanceFrequency();
      t0 = t1;
      // using state[0] to broadcast data from server to all players, such as dt
      state[0].lookangle_x = dt;
      for (unsigned i = 1; i < net_players + 1; i++) {
        ssize_t n = write(clients_fd[i], state, sizeof(state));
        if (n == -1 or n != sizeof(state)) {
          fprintf(stderr, "\n%s:%u: player %u: send failed\n", __FILE__,
                  __LINE__, i);
          std::abort();
        }
      }
    }
  }

  inline void free() {
    close(server_fd);
    server_fd = 0;
  }
};

inline net_server net_server{};
} // namespace glos