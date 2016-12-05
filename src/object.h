#pragma once
//#include<SDL2/SDL2_gfxPrimitives.h>
#include"lib.h"
#include"mat4.h"
#define bit_object_allocated 0
#define object_part_cap 4
//------------------------------------------------------------------------ def

typedef struct object{
	position position;
	velocity velocity;
	angle angle;
	angular_velocity angular_velocity;
	float model_to_world_matrix[4*4];
	bounding_radius bounding_radius;
	scale scale;
//	color color;
	gid glob_id;
	int model_to_world_matrix_is_updated;
	void(*init)(struct object*);
	void(*update)(struct object*,dt);
	void(*collision)(struct object*,struct object*,dt);
	void(*render)(struct object*);
	void(*free)(struct object*);
	void*part[object_part_cap];

	type type;
	bits*bits_ref;
}object;

#define object_ref_validate_cast 0
#define object_validate_delete 0

inline static object*object_ptr(ref r){
	return(object*)r;//? detect cast error
}

//-------------------------------------------------------------------overrides
//
//----------------------------------------------------------------------- init
inline static void _object_init_(object*this){
//	printf(" object init: [ %4s %p ]\n",this->type.path,this);
//	printf(" * new %-12s [ %4s %p ]\n","object",this->type.path,this);
}
//---------------------------------------------------------------------- update
inline static void _object_update_(object*this,dt dt){
	vec4_increase_with_vec4_over_dt(&this->position,&this->velocity,dt);
	vec4_increase_with_vec4_over_dt(&this->angle,&this->angular_velocity,dt);
	if(this->model_to_world_matrix_is_updated &&
		(this->velocity.x||this->velocity.y||this->velocity.z||
		this->angular_velocity.x||this->angular_velocity.y||
		this->angular_velocity.z))
	{
		this->model_to_world_matrix_is_updated=0;
	}
}
inline static void _object_collision_(object*this,object*other,dt dt){}
inline static void _object_render_(object*this){}
inline static void _object_free_(object*this){
//	printf(" * del %-12s [ %4s %p ]\n","object",this->type.path,this);
}
//----------------------------------------------------------------------------
static object object_def={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={0,0,0,0},
	.type={{0,0,0,0,0,0,0,0}},
	.bits_ref=NULL,
	.model_to_world_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=_object_init_,
	.update=_object_update_,
	.collision=_object_collision_,
	.render=_object_render_,
	.free=_object_free_,
	.part={NULL,NULL,NULL,NULL}
};
//----------------------------------------------------------- ------ functions

inline static void object_update_bounding_radius_using_scale(object*this) {
	this->bounding_radius=(bounding_radius)
		sqrtf(this->scale.x*this->scale.x+this->scale.y*this->scale.y);
}

//----------------------------------------------------------------------------

inline static void _object_update_model_to_world_matrix(object*this){
	if(this->model_to_world_matrix_is_updated)
		return;

	mat4_load_translate(this->model_to_world_matrix,&this->position);

	mat4_append_rotation_about_z_axis(
			this->model_to_world_matrix,this->angle.z);

	mat4_scale(this->model_to_world_matrix,&this->scale);

	this->model_to_world_matrix_is_updated=1;
}

//----------------------------------------------------------------------------

inline static void _render_glob_(object*o) {
	const glob*g=globs_ro(o->glob_id);
	if(!g->vbufid)
		return;
	_object_update_model_to_world_matrix(o);
	shader_render_triangle_array(
			g->vbufid,
			g->vbufn,
			g->texbufid,
			o->model_to_world_matrix
		);
}

//----------------------------------------------------------------------------
