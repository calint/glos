#pragma once
//#include<ctype.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>

#define net_cap 8

static struct{
	int32_t keybits;
	float lookangle_y;
	float lookangle_x;
}net_to_send,net_current_state[net_cap];

static unsigned net_active_player_index;

static int net_sockfd;

inline static void net_connect(){
	const char*host="127.0.0.1";
	const uint16_t port=8085;


	struct sockaddr_in server;
	net_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(net_sockfd==-1){
		perror("could not make socket");
		exit(-1);
	}

	server.sin_addr.s_addr=inet_addr(host);
	server.sin_family=AF_INET;
	server.sin_port=htons(port);

	if(connect(net_sockfd,(struct sockaddr *)&server,sizeof(server))<0){
		fprintf(stderr,"\n%s:%u: connect failed\n",__FILE__,__LINE__);
		fprintf(stderr,"    server: %s  port: %d\n\n",host,port);
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
	if(send(net_sockfd,&net_to_send,sizeof(net_to_send),0)<0){
		fprintf(stderr,"\n%s:%u: send failed\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
	// receive previous key states
	if(recv(net_sockfd,net_current_state,sizeof(net_current_state),0)< 0){
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
