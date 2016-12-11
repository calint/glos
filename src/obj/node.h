#pragma once

typedef struct _node{
	float matrix_vertices_model_to_world[16];
	float matrix_normals_model_to_world[9];
	unsigned matrix_vertices_model_to_world_valid;
	unsigned ts_parent_upd_mtx_mw;
	glo*glo;
}node;
#define node_def {mat4_identity,mat3_identity,0,0,NULL}

