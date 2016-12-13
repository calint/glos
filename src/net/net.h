#pragma once
//#include<ctype.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>

#define net_cap 2

static const char*net_host="127.0.0.1";
static uint16_t net_port=8085;

typedef struct netstate{
	int32_t keybits;
	float lookangle_y;
	float lookangle_x;
}netstate;

static netstate net_state_to_send;
static netstate net_state_current[net_cap];

static uint32_t net_active_player_index;
static float net_dt;

static int net_sockfd;

inline static void net_connect(){
	struct sockaddr_in server;
	net_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(net_sockfd==-1){
		fprintf(stderr,"\n%s:%u: create socket failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	server.sin_addr.s_addr=inet_addr(net_host);
	server.sin_family=AF_INET;
	server.sin_port=htons(net_port);

	if(connect(net_sockfd,(struct sockaddr *)&server,sizeof(server))<0){
		fprintf(stderr,"\n%s:%u: connect failed\n",__FILE__,__LINE__);
		fprintf(stderr,"    server: %s  port: %d\n\n",net_host,net_port);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
	if(recv(net_sockfd,&net_active_player_index,sizeof(net_active_player_index)
			,0)< 0){
		fprintf(stderr,"\n%s:%u: receive failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
}

inline static void net_init(){
	net_connect();
}

inline static void net_at_new_frame(){
	// send next key state
	if(send(net_sockfd,&net_state_to_send,sizeof(net_state_to_send),0)<0){
		fprintf(stderr,"\n%s:%u: send failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	if(recv(net_sockfd,&net_dt,sizeof(net_dt)
			,0)< 0){
		fprintf(stderr,"\n%s:%u: receive failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	// receive previous key states
	if(recv(net_sockfd,net_state_current,sizeof(net_state_current),0)< 0){
		fprintf(stderr,"\n%s:%u: receive failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
//	// receive previous keystate
//	net_current_state[0].keybits=4+2;
//	net_current_state[1].keybits=1+8;
}

inline static void net_disconnect(){
	if(net_sockfd){
		close(net_sockfd);
		net_sockfd=0;
	}
}

inline static void net_free(){net_disconnect();}
