#pragma once
#include"../gfx.h"
#include"bvol.h"
#include"node.h"
#include"phy.h"
//---------------------------------------------------------------------- config

#define object_part_cap 5

//------------------------------------------------------------------------ def

typedef struct object{
	node node;
	type type;
	bits*ptr_to_bits;
	bvol bvol;
	phy phy;
	void(*init)(struct object*);
	void(*update)(struct object*,dt);
	void(*collision)(struct object*,struct object*,dt);
	void(*render)(struct object*);
	void(*free)(struct object*);
	void*part[object_part_cap];
}object;
//
//----------------------------------------------------------------------- init
inline static void object_init(object*this){}
//---------------------------------------------------------------------- update
inline static void object_update(object*this,dt dt){
	vec3_inc_with_vec3_over_dt(&this->phy.position,&this->phy.velocity,dt);
	vec3_inc_with_vec3_over_dt(&this->phy.angle,&this->phy.angular_velocity,dt);
	if(this->node.matrix_vertices_model_to_world_valid &&
		(this->phy.velocity.x||this->phy.velocity.y||this->phy.velocity.z||
		this->phy.angular_velocity.x||this->phy.angular_velocity.y||
		this->phy.angular_velocity.z))
	{
		this->node.matrix_vertices_model_to_world_valid=0;
	}
}
inline static void object_collision(object*this,object*other,dt dt){}
inline static void object_render(object*this){}
inline static void object_at_free(object*this){}
//----------------------------------------------------------------------------
static object object_def={
	.node=node_def,
	.bvol=bvol_def,
	.phy=phy_def,
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
	if(o->node.matrix_vertices_model_to_world_valid)
		return o->node.matrix_vertices_model_to_world;

	mat4_set_translation(o->node.matrix_vertices_model_to_world,
			&o->phy.position);

	mat4_append_rotation_about_z_axis(
			o->node.matrix_vertices_model_to_world,o->phy.angle.z);

	mat4_scale(o->node.matrix_vertices_model_to_world,&o->bvol.scale);

	o->node.matrix_vertices_model_to_world_valid=1;

	return o->node.matrix_vertices_model_to_world;
}

//----------------------------------------------------------------------------

inline static void object_render_glob(object*o) {
	if(!o->node.glo)
		return;
	const float*f=object_get_updated_matrix_model_to_world(o);
	glo_render(o->node.glo,f);
}

//----------------------------------------------------------------------------
