#pragma once
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

#define net_cap 2 + 1

class net {
public:
  net_state state_to_send;
  net_state state_current[net_cap];
  uint32_t active_player_index = 1;
  float dt;
  int fd;
  const char *host = "127.0.0.1";
  uint16_t port = 8085;

  inline void init() {
    struct sockaddr_in server;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
      fprintf(stderr, "\n%s:%u: create socket failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    int flag = 1;
    int result =
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    if (result < 0) {
      fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    printf("[ net ] connecting to %s on port %u\n", host, port);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "\n%s:%u: connect failed\n", __FILE__, __LINE__);
      fprintf(stderr, "    server: %s  port: %d\n\n", host, port);
      fprintf(stderr, "\n\n");
      exit(-1);
    }
    printf("[ net ] connected. waiting for go ahead\n");
    if (recv(fd, &active_player_index, sizeof(active_player_index), 0) < 0) {
      fprintf(stderr, "\n%s:%u: receive failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }
    printf("[ net ] go\n");
  }

  inline void free() {
    if (fd) {
      close(fd);
      fd = 0;
    }
  }

  inline void at_frame_begin() {
    // send next key state
    if (send(fd, &state_to_send, sizeof(state_to_send), 0) < 0) {
      fprintf(stderr, "\n%s:%u: send failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }
  }

  inline void at_frame_end() {
    const uint64_t t0 = SDL_GetPerformanceCounter();
    // receive previous key states
    if (recv(fd, state_current, sizeof(state_current), 0) < 0) {
      fprintf(stderr, "\n%s:%u: receive failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }
    dt = state_current[0].lookangle_x;
    const uint64_t t1 = SDL_GetPerformanceCounter();
    const float dt = (float)(t1 - t0) / (float)SDL_GetPerformanceFrequency();
    metrics.net_lag_prv_frame = dt;
  }
};

static net net{};