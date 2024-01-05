#pragma once
// reviewed: 2023-12-23
// reviewed: 2024-01-04

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace glos {
class net_state final {
public:
  uint64_t keys = 0;
  float look_angle_y = 0;
  float look_angle_x = 0;
};

class net_init_packet final {
public:
  uint32_t player_ix = 0;
  uint64_t ms = 0;
};

class net final {
public:
  net_state next_state{};
  net_state states[net_players + 1];
  uint32_t active_state_ix = 1;
  float dt = 0;
  uint64_t ms = 0;
  char const *host = "127.0.0.1";
  uint16_t port = 8085;
  bool enabled = false;

  inline void init() {
    if (not enabled) {
      return;
    }
    struct sockaddr_in server;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
      printf("%s:%d: ", __FILE__, __LINE__);
      perror("");
      std::abort();
    }

    int flag = 1;
    int result =
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    if (result < 0) {
      printf("%s:%d: cannot set TCP_NODELAY\n", __FILE__, __LINE__);
      std::abort();
    }

    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    printf("[ net ] connecting to '%s' on port %u\n", host, port);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      printf("%s:%d: ", __FILE__, __LINE__);
      perror("");
      std::abort();
    }

    printf("[ net ] connected. waiting for go ahead\n");
    net_init_packet nip{};
    ssize_t const n = recv(fd, &nip, sizeof(nip), 0);
    if (n <= 0) {
      printf("%s:%u: ", __FILE__, __LINE__);
      if (n == 0) {
        printf("server disconnected\n");
      } else {
        perror("");
      }
      std::abort();
    }
    active_state_ix = nip.player_ix;
    ms = nip.ms;
    printf("[ net ] playing player %u\n", active_state_ix);
    printf("[ net ] server time %lu ms\n\n", ms);
  }

  inline void free() {
    if (not enabled) {
      return;
    }
    if (fd) {
      close(fd);
      fd = 0;
    }
  }

  inline void begin() { send_state(); }

  inline void at_update_done() {
    uint64_t const t0 = SDL_GetPerformanceCounter();
    // receive signals from previous frame
    ssize_t const n = recv(fd, states, sizeof(states), 0);
    if (n == -1) {
      printf("%s:%d: ", __FILE__, __LINE__);
      perror("");
      std::abort();
    }
    if (size_t(n) != sizeof(states)) {
      printf("%s:%d: incomplete read of states\n", __FILE__, __LINE__);
      std::abort();
    }
    // send current frame signals
    send_state();
    // get server dt from server state (using a float value)
    dt = states[0].look_angle_x;
    ms = states[0].keys;
    // calculate network used lag
    uint64_t const t1 = SDL_GetPerformanceCounter();
    metrics.net_lag = float(t1 - t0) / float(SDL_GetPerformanceFrequency());
  }

private:
  int fd = 0;

  inline void send_state() const {
    ssize_t const n = send(fd, &next_state, sizeof(next_state), 0);
    if (n < 0) {
      printf("%s:%d: ", __FILE__, __LINE__);
      perror("");
      std::abort();
    }
    if (size_t(n) != sizeof(next_state)) {
      printf("%s:%d: incomplete send\n", __FILE__, __LINE__);
      std::abort();
    }
  }
};

inline net net{};
} // namespace glos