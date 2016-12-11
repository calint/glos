#pragma once
#include"../lib.h"
typedef struct node{
	float Mmw[16];
	float Mnmw[9];
	unsigned Mmw_valid;
	glo*glo;
}node;
#define node_def {mat4_identity_,mat3_identity_,0,0}

