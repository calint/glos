#pragma once
#include"../gfx.h"
//---------------------------------------------------------------------- config

#define object_part_cap 5

//------------------------------------------------------------------------ def

typedef struct object{
	position position;                                                 // 16 B
	velocity velocity;                                                 // 16 B
	angle angle;                                                       // 16 B
	angular_velocity angular_velocity;                                 // 16 B

	float matrix_vertices_model_to_world[16];                        // 64 B
	float matrix_normals_model_to_world[9];                         // 36 B

	bounding_radius bounding_radius;                                   //  4 B
	scale scale;                                                       // 16 B
	glo*glo;                                                           //  8 B
	bool matrix_vertices_model_to_world_valid;                         //  4 B
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
	if(this->matrix_vertices_model_to_world_valid &&
		(this->velocity.x||this->velocity.y||this->velocity.z||
		this->angular_velocity.x||this->angular_velocity.y||
		this->angular_velocity.z))
	{
		this->matrix_vertices_model_to_world_valid=0;
	}
}
inline static void object_collision(object*this,object*other,dt dt){}
inline static void object_render(object*this){}
inline static void object_at_free(object*this){}
//----------------------------------------------------------------------------
static object object_def={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={0,0,0,0},
	.type={{0,0,0,0,0,0,0,0}},
	.ptr_to_bits=0,
	.matrix_vertices_model_to_world={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=object_init,
	.update=object_update,
	.collision=object_collision,
	.render=object_render,
	.free=object_at_free,
	.part={0,0,0,0}
};
//----------------------------------------------------------- ------ functions

inline static void object_update_bounding_radius_using_scale(object*o) {
	o->bounding_radius=(bounding_radius)
		sqrtf(o->scale.x*o->scale.x+o->scale.y*o->scale.y);
}

//----------------------------------------------------------------------------

inline static const float*object_get_updated_matrix_model_to_world(object*o){
	if(o->matrix_vertices_model_to_world_valid)
		return o->matrix_vertices_model_to_world;

	mat4_set_translation(o->matrix_vertices_model_to_world,&o->position);

	mat4_append_rotation_about_z_axis(
			o->matrix_vertices_model_to_world,o->angle.z);

	mat4_scale(o->matrix_vertices_model_to_world,&o->scale);

	o->matrix_vertices_model_to_world_valid=1;

	return o->matrix_vertices_model_to_world;
}

//----------------------------------------------------------------------------

inline static void object_render_glob(object*o) {
	if(!o->glo)
		return;
	const float*f=object_get_updated_matrix_model_to_world(o);
	glo_render(o->glo,f);
}

//----------------------------------------------------------------------------
