#pragma once
// reviewed: 2023-12-23

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class net_state {
public:
  unsigned keys;
  float lookangle_y;
  float lookangle_x;
};

class net {
public:
  net_state next_state;
  net_state states[net_players + 1];
  uint32_t active_state_ix = 1;
  float dt;
  int fd;
  const char *host = "127.0.0.1";
  uint16_t port = 8085;

  inline void init() {
    struct sockaddr_in server;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      perror("");
      exit(-1);
    }

    int flag = 1;
    int result =
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    if (result < 0) {
      fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
      exit(-1);
    }

    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    printf("[ net ] connecting to '%s' on port %u\n", host, port);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      perror("");
      exit(-1);
    }
    printf("[ net ] connected. waiting for go ahead\n");
    ssize_t n = recv(fd, &active_state_ix, sizeof(active_state_ix), 0);
    if (n <= 0) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      if (n == 0) {
        fprintf(stderr, "server disconnected\n");
      } else {
        perror("");
      }
      exit(-1);
    }
    printf("[ net ] playing player %u\n", active_state_ix);
  }

  inline void free() {
    if (fd) {
      close(fd);
      fd = 0;
    }
  }

  inline void begin() { send_state(); }

  inline void at_frame_end() {
    const uint64_t t0 = SDL_GetPerformanceCounter();
    // receive signals from previous frame
    if (recv(fd, states, sizeof(states), 0) < 0) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      perror("");
      exit(-1);
    }
    // send current frame signals
    send_state();
    // get server dt from server state (using a float value)
    dt = states[0].lookangle_x;
    // calculate network used lag
    const uint64_t t1 = SDL_GetPerformanceCounter();
    const float dt = (float)(t1 - t0) / (float)SDL_GetPerformanceFrequency();
    metrics.net_lag = dt;
  }

private:
  inline void send_state() {
    if (send(fd, &next_state, sizeof(next_state), 0) < 0) {
      fprintf(stderr, "\n%s:%u: ", __FILE__, __LINE__);
      perror("");
      exit(-1);
    }
  }
};

static net net{};