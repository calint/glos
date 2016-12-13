#pragma once
#include"net.h"

static int netsrv_sockfd;
static int netsrv_client_sock_fd[net_cap];

static netstate netsrv_state[net_cap];

inline static void netsrv_init(){

	netsrv_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(netsrv_sockfd==-1){
		fprintf(stderr,"\n%s:%u: create socket failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(net_port);

	if(bind(netsrv_sockfd,(struct sockaddr *)&server,sizeof(server))<0){
		fprintf(stderr,"\n%s:%u: bind failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	if(listen(netsrv_sockfd,net_cap)==-1){
		fprintf(stderr,"\n%s:%u\n",__FILE__,__LINE__);
		perror("cause");
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	printf(" * waiting for %d players to connect on port %d\n",
			net_cap,net_port);

	for(int i=0;i<net_cap;i++){
//		struct sockaddr_in client;
//		const socklen_t c=sizeof(struct sockaddr_in);
//
//		netsrv_client_sock_fd[i]=
//				accept(netsrv_sockfd,(struct sockaddr*)&client,&c);
//
		netsrv_client_sock_fd[i]=accept(netsrv_sockfd,NULL,NULL);

		if(netsrv_client_sock_fd[i]<0){
			fprintf(stderr,"\n%s:%u\n",__FILE__,__LINE__);
			perror("cause");
			stacktrace_print(stderr);
			fprintf(stderr,"\n\n");
			exit(-1);
		}
		printf(" * player %d of %d connected\n",i+1,net_cap);
	}

	printf(" * sending start\n");
	// send net_active_player_index
	for(uint32_t i=0;i<net_cap;i++){
		write(netsrv_client_sock_fd[i],&i,sizeof(uint32_t));
	}
}

inline static void netsrv_loop(){
	printf(" * entering loop\n");
	const size_t fullreadsize=sizeof(netstate);
	while(1){
		const uint64_t t0=SDL_GetPerformanceCounter();
		for(int i=0;i<net_cap;i++){
			const ssize_t n=recv(netsrv_client_sock_fd[i],&netsrv_state[i],
					fullreadsize,0);
			if(n==-1){
				fprintf(stderr,"\n%s:%u:\n",__FILE__,__LINE__);
				perror("cause");
				stacktrace_print(stderr);
				fprintf(stderr,"\n\n");
				exit(-1);
			}
			if((unsigned)n!=fullreadsize){
				fprintf(stderr,"\n%s:%u: not full read\n",__FILE__,__LINE__);
				perror("cause");
				stacktrace_print(stderr);
				fprintf(stderr,"\n\n");
				exit(-1);
			}
		}
		const uint64_t t1=SDL_GetPerformanceCounter();
		const float dt=(float)(t1-t0)/(float)SDL_GetPerformanceFrequency();
//		printf("netsrv_dt: %f\n",dt);
		for(int i=0;i<net_cap;i++){
			write(netsrv_client_sock_fd[i],&dt,sizeof(dt));
			write(netsrv_client_sock_fd[i],netsrv_state,sizeof(netsrv_state));
		}
	}
}


inline static void netsrv_free(){
	close(netsrv_sockfd);
	netsrv_sockfd=0;
}
