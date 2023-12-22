#pragma once
#include "net.h"

static int net_port = 8085;
static int netsrv_fd;
static int netsrv_client_fd[net_cap];
static net_state netsrv_state[net_cap];

inline static void netsrv_init() {

  netsrv_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (netsrv_fd == -1) {
    fprintf(stderr, "\n%s:%u: create socket failed\n", __FILE__, __LINE__);
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  int flag = 1;
  int result = setsockopt(netsrv_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
                          sizeof(int));
  if (result < 0) {
    fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(net_port);

  if (bind(netsrv_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "\n%s:%u: bind failed\n", __FILE__, __LINE__);
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  if (listen(netsrv_fd, net_cap) == -1) {
    fprintf(stderr, "\n%s:%u\n", __FILE__, __LINE__);
    perror("cause");
    fprintf(stderr, "\n\n");
    exit(-1);
  }

  printf(" * waiting for %d players to connect on port %d\n", net_cap - 1,
         net_port);

  for (int i = 1; i < net_cap; i++) {
    netsrv_client_fd[i] = accept(netsrv_fd, NULL, NULL);

    int flag = 1;
    int result = setsockopt(netsrv_client_fd[i], IPPROTO_TCP, TCP_NODELAY,
                            (char *)&flag, sizeof(int));
    if (result < 0) {
      fprintf(stderr, "\n%s:%u: set TCP_NODELAY failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    if (netsrv_client_fd[i] < 0) {
      fprintf(stderr, "\n%s:%u\n", __FILE__, __LINE__);
      perror("cause");
      fprintf(stderr, "\n\n");
      exit(-1);
    }
    printf(" * player %d of %d connected\n", i, net_cap - 1);
  }

  printf(" * sending start\n");
  // send net_active_player_index
  for (uint32_t i = 1; i < net_cap; i++) {
    write(netsrv_client_fd[i], &i, sizeof(uint32_t));
  }
}

inline static void netsrv_loop() {
  printf(" * entering loop\n");
  const size_t state_read_size = sizeof(net_state);
  uint64_t t0 = SDL_GetPerformanceCounter();
  while (1) {
    for (unsigned i = 1; i < net_cap; i++) {
      const ssize_t n =
          recv(netsrv_client_fd[i], &netsrv_state[i], state_read_size, 0);
      if (n == -1) {
        fprintf(stderr, "\n%s:%u:\n", __FILE__, __LINE__);
        perror("cause");
        fprintf(stderr, "\n\n");
        exit(-1);
      }
      if (n == 0) {
        fprintf(stderr, "\n%s:%u: player %u disconnected\n", __FILE__, __LINE__,
                i);
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
    netsrv_state[0].lookangle_x = dt;
    for (int i = 1; i < net_cap; i++) {
      write(netsrv_client_fd[i], netsrv_state, sizeof(netsrv_state));
    }
  }
}

inline static void netsrv_free() {
  close(netsrv_fd);
  netsrv_fd = 0;
}
