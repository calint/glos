#pragma once
// #include<ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct net_state {
  unsigned keybits;
  float lookangle_y;
  float lookangle_x;
} net_state;

#define net_cap 2 + 1
static net_state net_state_to_send;
static net_state net_state_current[net_cap];
static uint32_t net_active_player_index = 1;
static float net_dt;
static int net_fd;
static const char *net_host = "127.0.0.1";
static uint16_t net_port = 8085;

inline static void net_connect() {
  struct sockaddr_in server;
  net_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (net_fd == -1) {
    fprintf(stderr, "\n%s:%u: create socket failed\n", __FILE__, __LINE__);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  int flag = 1;
  int result =
      setsockopt(net_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
  if (result < 0) {
    fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  server.sin_addr.s_addr = inet_addr(net_host);
  server.sin_family = AF_INET;
  server.sin_port = htons(net_port);

  printf("[ net ] connecting to %s on port %u\n", net_host, net_port);
  if (connect(net_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "\n%s:%u: connect failed\n", __FILE__, __LINE__);
    fprintf(stderr, "    server: %s  port: %d\n\n", net_host, net_port);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }
  if (recv(net_fd, &net_active_player_index, sizeof(net_active_player_index),
           0) < 0) {
    fprintf(stderr, "\n%s:%u: receive failed\n", __FILE__, __LINE__);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }
}

inline static void net_init() { net_connect(); }

inline static void net__at__frame_begin() {
  // send next key state
  if (send(net_fd, &net_state_to_send, sizeof(net_state_to_send), 0) < 0) {
    fprintf(stderr, "\n%s:%u: send failed\n", __FILE__, __LINE__);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }
}

inline static void net__at__frame_end() {
  const uint64_t t0 = SDL_GetPerformanceCounter();
  // receive previous key states
  if (recv(net_fd, net_state_current, sizeof(net_state_current), 0) < 0) {
    fprintf(stderr, "\n%s:%u: receive failed\n", __FILE__, __LINE__);
    stacktrace_print(stderr);
    fprintf(stderr, "\n\n");
    exit(-1);
  }
  net_dt = net_state_current[0].lookangle_x;
  const uint64_t t1 = SDL_GetPerformanceCounter();
  const float dt = (float)(t1 - t0) / (float)SDL_GetPerformanceFrequency();
  metrics.net_lag_prv_frame = dt;
}

inline static void net_disconnect() {
  if (net_fd) {
    close(net_fd);
    net_fd = 0;
  }
}

inline static void net_free() { net_disconnect(); }
