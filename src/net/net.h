#pragma once
#include"ctype.h"

#define net_cap 8

struct{
	int32_t keybits;
	float lookangle_y;
	float lookangle_x;
}net_to_send,nets_current_state[net_cap];
//
//static net nets[net_cap];

static unsigned net_active_player_index;

inline static void net_init(){
}

inline static void net_connect(){

}

inline static void net_send(){
	// send
}

inline static void net_recv(){
	nets_current_state[0].keybits=4+2;
	nets_current_state[1].keybits=1+8;
}

inline static void net_disconnect(){

}

inline static void net_free(){
	// close network if active

}
