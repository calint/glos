#pragma once
#include"ctype.h"

#define net_cap 8

static struct{
	int32_t keybits;
	float lookangle_y;
	float lookangle_x;
}net_to_send,net_current_state[net_cap];

static unsigned net_active_player_index;

inline static void net_init(){
}

inline static void net_connect(){

}

inline static void net_at_new_frame(){
	// send next key state
	// ...

	// receive previous keystate
	net_current_state[0].keybits=4+2;
	net_current_state[1].keybits=1+8;
}

inline static void net_disconnect(){

}

inline static void net_free(){
	// close network if active
}
