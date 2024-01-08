#pragma once
// reviewed: 2023-12-23
// reviewed: 2024-01-04
// reviewed: 2024-01-06

#include "net.hpp"

namespace glos {
class net_server final {
public:
  uint16_t port = 8085;

  inline void init() {
    if (SDL_Init(SDL_INIT_TIMER)) {
      fprintf(stderr, "\n%s:%d: cannot initiate sdl timer: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      fprintf(stderr, "\n%s:%d: cannot create socket\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }

    {
      int flag = 1;
      if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &flag,
                     sizeof(flag))) {
        fprintf(stderr, "\n%s:%d: cannot set TCP_NODELAY\n", __FILE__,
                __LINE__);
        fflush(stderr);
        std::abort();
      }
    }

    struct sockaddr_in server {};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server))) {
      fprintf(stderr, "\n%s:%d: cannot bind socket\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }

    if (listen(server_fd, net_players)) {
      fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }

    printf(" * waiting for %d players to connect on port %d\n", net_players,
           port);

    for (unsigned i = 1; i < net_players + 1; ++i) {
      clients_fd[i] = accept(server_fd, nullptr, nullptr);
      if (clients_fd[i] == -1) {
        fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
        perror("");
        fflush(stderr);
        std::abort();
      }

      int flag = 1;
      if (setsockopt(clients_fd[i], IPPROTO_TCP, TCP_NODELAY, &flag,
                     sizeof(flag))) {
        fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
        perror("");
        fflush(stderr);
        std::abort();
      }

      printf(" * player %d of %d connected\n", i, net_players);
    }

    printf(" * sending start\n");

    net_init_packet nip{};
    nip.ms = SDL_GetTicks64();
    // send the assigned player index to clients
    for (uint32_t i = 1; i < net_players + 1; ++i) {
      // send initial packet to clients
      nip.player_ix = i;
      ssize_t const n = send(clients_fd[i], &nip, sizeof(nip), 0);
      if (n == -1) {
        fprintf(stderr, "\n%s:%d: could not send initial packet to player %u: ",
                __FILE__, __LINE__, i);
        perror("");
        fflush(stderr);
        std::abort();
      }
      if (size_t(n) != sizeof(nip)) {
        fprintf(stderr, "\n%s:%d: incomplete send to player %u: ", __FILE__,
                __LINE__, i);
        perror("");
        fflush(stderr);
        std::abort();
      }
    }
  }

  inline void run() {
    printf(" * entering loop\n");
    uint64_t t0 = SDL_GetPerformanceCounter();
    while (true) {
      // note. state[0] contains server info sent to all clients
      //       state[1] etc are the client states
      for (unsigned i = 1; i < net_players + 1; ++i) {
        ssize_t const n = recv(clients_fd[i], &state[i], sizeof(state[i]), 0);
        if (n == -1) {
          fprintf(stderr, "\n%s:%d: player %u: ", __FILE__, __LINE__, i);
          perror("");
          fflush(stderr);
          std::abort();
        }
        if (n == 0) {
          fprintf(stderr, "\n%s:%d: player %u: disconnected\n", __FILE__,
                  __LINE__, i);
          fflush(stderr);
          std::abort();
        }
        if (size_t(n) != sizeof(state[i])) {
          fprintf(stderr, "\n%s:%d: player %u: read was incomplete\n", __FILE__,
                  __LINE__, i);
          fflush(stderr);
          std::abort();
        }
      }

      // calculate the delta time for this frame
      uint64_t const t1 = SDL_GetPerformanceCounter();
      float const dt = float(t1 - t0) / float(SDL_GetPerformanceFrequency());
      t0 = t1;

      // using state[0] to broadcast data from server to all players
      state[0].look_angle_x = dt;
      state[0].keys = SDL_GetTicks64();

      for (unsigned i = 1; i < net_players + 1; ++i) {
        ssize_t const n = send(clients_fd[i], state, sizeof(state), 0);
        if (n == -1) {
          fprintf(stderr, "\n%s:%d: player %u: ", __FILE__, __LINE__, i);
          perror("");
          fflush(stderr);
          std::abort();
        }
        if (size_t(n) != sizeof(state)) {
          fprintf(stderr, "\n%s:%d: player %u: send was incomplete\n", __FILE__,
                  __LINE__, i);
          fflush(stderr);
          std::abort();
        }
      }
    }
  }

  inline void free() {
    close(server_fd);
    server_fd = 0;
    SDL_Quit();
  }

private:
  int server_fd = 0;
  int clients_fd[net_players + 1] = {};
  net_state state[net_players + 1]{};
  // note. state[0] is used by server to broadcast to all clients
  //       delta time for frame (dt) and current server time in ms
};

inline net_server net_server{};
} // namespace glos