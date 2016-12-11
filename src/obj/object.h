#pragma once
#include"../gfx.h"
//---------------------------------------------------------------------- config

#define object_part_cap 5


#define mat3_identity {1,0,0, 0,1,0, 0,0,1}
#define mat4_identity {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}

//------------------------------------------------------------------------ def
typedef struct _node{
	float matrix_vertices_model_to_world[16];
	float matrix_normals_model_to_world[9];
	unsigned matrix_vertices_model_to_world_valid;
	unsigned ts_parent_upd_mtx_mw;
	glo*glo;
}node;
#define node_def {mat4_identity,mat3_identity,0,0,NULL}

typedef struct _bvol{
	bounding_radius bounding_radius;
	scale scale;
}bvol;
#define bvol_def {0,vec4_def}

typedef struct object{
	node _node;
	bvol bvol;
	position position;                                                 // 16 B
	velocity velocity;                                                 // 16 B
	angle angle;                                                       // 16 B
	angular_velocity angular_velocity;                                 // 16 B
	void(*init)(struct object*);
	void(*update)(struct object*,dt);
	void(*collision)(struct object*,struct object*,dt);
	void(*render)(struct object*);
	void(*free)(struct object*);
	void*part[object_part_cap];
	type type;
	bits*ptr_to_bits;
	struct object*parent;
}object;
//
//----------------------------------------------------------------------- init
inline static void object_init(object*this){}
//---------------------------------------------------------------------- update
inline static void object_update(object*this,dt dt){
	vec3_inc_with_vec3_over_dt(&this->position,&this->velocity,dt);
	vec3_inc_with_vec3_over_dt(&this->angle,&this->angular_velocity,dt);
	if(this->_node.matrix_vertices_model_to_world_valid &&
		(this->velocity.x||this->velocity.y||this->velocity.z||
		this->angular_velocity.x||this->angular_velocity.y||
		this->angular_velocity.z))
	{
		this->_node.matrix_vertices_model_to_world_valid=0;
	}
}
inline static void object_collision(object*this,object*other,dt dt){}
inline static void object_render(object*this){}
inline static void object_at_free(object*this){}
//----------------------------------------------------------------------------
static object object_def={
	._node=node_def,
	.bvol=bvol_def,
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.type={{0,0,0,0,0,0,0,0}},
	.ptr_to_bits=0,
	.init=object_init,
	.update=object_update,
	.collision=object_collision,
	.render=object_render,
	.free=object_at_free,
	.part={0,0,0,0}
};
//----------------------------------------------------------- ------ functions

inline static void object_update_bounding_radius_using_scale(object*o) {
	o->bvol.bounding_radius=(bounding_radius)
		sqrtf(o->bvol.scale.x*o->bvol.scale.x+o->bvol.scale.y*o->bvol.scale.y);
}

//----------------------------------------------------------------------------

inline static const float*object_get_updated_matrix_model_to_world(object*o){
	if(o->_node.matrix_vertices_model_to_world_valid)
		return o->_node.matrix_vertices_model_to_world;

	mat4_set_translation(o->_node.matrix_vertices_model_to_world,&o->position);

	mat4_append_rotation_about_z_axis(
			o->_node.matrix_vertices_model_to_world,o->angle.z);

	mat4_scale(o->_node.matrix_vertices_model_to_world,&o->bvol.scale);

	o->_node.matrix_vertices_model_to_world_valid=1;

	return o->_node.matrix_vertices_model_to_world;
}

//----------------------------------------------------------------------------

inline static void object_render_glob(object*o) {
	if(!o->_node.glo)
		return;
	const float*f=object_get_updated_matrix_model_to_world(o);
	glo_render(o->_node.glo,f);
}

//----------------------------------------------------------------------------
