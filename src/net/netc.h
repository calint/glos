#pragma once
#include"ctype.h"

#define netc_cap 8

struct{
	uint32_t keybits;
	float pointer_angle_about_y;
	float pointer_angle_about_x;
}netc,netc_prv;

static netcs[netc_cap];

static unsigned netc_active_player_index;

inline static void netc_init(){
}

inline static void netc_connect(){

}

inline static void netc_send(){
	netc_prv=netc;
	// send
}

inline static void netc_recv(){

}

inline static void netc_disconnect(){

}

inline static void netc_free(){
	// close network if active

}
