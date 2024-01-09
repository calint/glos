#pragma once
// reviewed: 2023-12-23
// reviewed: 2024-01-04
// reviewed: 2024-01-06

namespace glos {

// client state
class net_state final {
public:
  uint64_t keys = 0;
  float look_angle_y = 0;
  float look_angle_x = 0;
};

// initial packet sent by server
class net_init_packet final {
public:
  uint32_t player_ix = 0;
  uint64_t ms = 0;
};

class net final {
public:
  bool enabled = false;
  net_state next_state{};
  std::array<net_state, net_players + 1> states{};
  uint32_t player_ix = 1;
  float dt = 0;
  uint64_t ms = 0;
  char const *host = "127.0.0.1";
  uint16_t port = 8085;

  inline void init() {
    if (net_players < 1) {
      fprintf(stderr,
              "\n%s:%d: configuration 'net_players' must be at least 1\n",
              __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }

    if (not enabled) {
      return;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
      fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }

    int flag = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag))) {
      fprintf(stderr, "\n%s:%d: cannot set TCP_NODELAY\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }

    struct sockaddr_in server {};
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    printf("[ net ] connecting to '%s' on port %u\n", host, port);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }

    printf("[ net ] connected. waiting for go ahead\n");
    net_init_packet nip{};
    ssize_t const n = recv(fd, &nip, sizeof(nip), 0);
    if (n == -1) {
      fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }
    if (n == 0) {
      fprintf(stderr, "%s:%d: server disconnected\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }
    if (size_t(n) != sizeof(nip)) {
      fprintf(stderr, "%s:%d: incomplete receive\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }

    // get server assigned player index and time in milliseconds
    player_ix = nip.player_ix;
    ms = nip.ms;

    printf("[ net ] playing player %u\n", player_ix);
    printf("[ net ] server time: %lu ms\n\n", ms);
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
    // calculate net lag
    uint64_t const t0 = SDL_GetPerformanceCounter();

    // receive signals from previous frame
    ssize_t const n = recv(fd, states.data(), sizeof(states), 0);
    if (n == -1) {
      fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }
    if (n == 0) {
      fprintf(stderr, "\n%s:%d: server disconnected\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }
    if (size_t(n) != sizeof(states)) {
      fprintf(stderr, "\n%s:%d: incomplete read of states\n", __FILE__,
              __LINE__);
      fflush(stderr);
      std::abort();
    }

    // send current frame signals
    send_state();

    // get server dt and time from server state
    dt = states[0].look_angle_x;
    ms = states[0].keys;

    // calculate network lag
    uint64_t const t1 = SDL_GetPerformanceCounter();
    metrics.net_lag = float(t1 - t0) / float(SDL_GetPerformanceFrequency());
  }

private:
  int fd = 0;

  inline void send_state() const {
    ssize_t const n = send(fd, &next_state, sizeof(next_state), 0);
    if (n == -1) {
      fprintf(stderr, "\n%s:%d: ", __FILE__, __LINE__);
      perror("");
      fflush(stderr);
      std::abort();
    }
    if (size_t(n) != sizeof(next_state)) {
      fprintf(stderr, "\n%s:%d: incomplete send\n", __FILE__, __LINE__);
      fflush(stderr);
      std::abort();
    }
  }
};

inline net net{};
} // namespace glos