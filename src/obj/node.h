#pragma once

typedef struct _node{
	float Mmw[16];
	float Mnmw[9];
	unsigned Mmw_valid;
	glo*glo;
}node;
#define node_def {mat4_identity,mat3_identity,0,NULL}

