#pragma once
#include "net.hpp"

class net_server final {
public:
  int port = 8085;
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
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    int flag = 1;
    int result =
        setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int));
    if (result < 0) {
      fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "\n%s:%u: bind failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    if (listen(server_fd, net_players + 1) == -1) {
      fprintf(stderr, "\n%s:%u\n", __FILE__, __LINE__);
      perror("cause");
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    printf(" * waiting for %d players to connect on port %d\n", net_players,
           port);

    for (unsigned i = 1; i < net_players + 1; i++) {
      clients_fd[i] = accept(server_fd, NULL, NULL);

      int flag = 1;
      int result = setsockopt(clients_fd[i], IPPROTO_TCP, TCP_NODELAY, &flag,
                              sizeof(int));
      if (result < 0) {
        fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__,
                __LINE__);
        fprintf(stderr, "\n\n");
        exit(-1);
      }

      if (clients_fd[i] < 0) {
        fprintf(stderr, "\n%s:%u\n", __FILE__, __LINE__);
        perror("cause");
        fprintf(stderr, "\n\n");
        exit(-1);
      }
      printf(" * player %d of %d connected\n", i, net_players);
    }

    printf(" * sending start\n");
    // send the player index to clients
    for (uint32_t i = 1; i < net_players + 1; i++) {
      write(clients_fd[i], &i, sizeof(uint32_t));
    }
  }

  inline void loop() {
    printf(" * entering loop\n");
    constexpr size_t state_read_size = sizeof(net_state);
    uint64_t t0 = SDL_GetPerformanceCounter();
    while (1) {
      for (unsigned i = 1; i < net_players + 1; i++) {
        const ssize_t n = recv(clients_fd[i], &state[i], state_read_size, 0);
        if (n == -1) {
          fprintf(stderr, "\n%s:%u:\n", __FILE__, __LINE__);
          perror("cause");
          fprintf(stderr, "\n\n");
          exit(-1);
        }
        if (n == 0) {
          fprintf(stderr, "\n%s:%u: player %u disconnected\n", __FILE__,
                  __LINE__, i);
          fprintf(stderr, "\n\n");
          exit(-1);
        }
        if ((unsigned)n != state_read_size) {
          fprintf(stderr, "\n%s:%u: not full read\n", __FILE__, __LINE__);
          fprintf(stderr, "\n\n");
          exit(-1);
        }
      }
      const uint64_t t1 = SDL_GetPerformanceCounter();
      const float dt = (float)(t1 - t0) / (float)SDL_GetPerformanceFrequency();
      t0 = t1;
      // using state[0] to send info from server to all players, such as dt
      state[0].lookangle_x = dt;
      for (unsigned i = 1; i < net_players + 1; i++) {
        write(clients_fd[i], state, sizeof(state));
      }
    }
  }

  inline void free() {
    close(server_fd);
    server_fd = 0;
  }
};

static net_server net_server{};
