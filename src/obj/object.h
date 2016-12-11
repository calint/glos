#pragma once
#include"node.h"
#include"bvol.h"
#include"phy.h"
//---------------------------------------------------------------------- config

#define object_part_cap 5

//------------------------------------------------------------------------ def

struct object;
typedef struct _vtbl{
	void(*init)(struct object*);
	void(*update)(struct object*,dt);
	void(*collision)(struct object*,struct object*,dt);
	void(*render)(struct object*);
	void(*free)(struct object*);
}vtbl;
#define vtbl_def {0,0,0,0,0}

typedef struct object{
	node node;
	bvol bvol;
	phy phy;
	vtbl vtbl;
	type type;
	bits*ptr_to_bits;
	void*part[object_part_cap];
}object;
//
//----------------------------------------------------------------------- init
inline static void object_init(object*this){}
//---------------------------------------------------------------------- update
inline static void object_update(object*this,dt dt){
	vec3_inc_with_vec3_over_dt(&this->phy.p,&this->phy.v,dt);
	vec3_inc_with_vec3_over_dt(&this->phy.a,&this->phy.av,dt);
	if(this->node.Mmw_valid &&
		(this->phy.v.x||this->phy.v.y||this->phy.v.z||
		this->phy.av.x||this->phy.av.y||
		this->phy.av.z))
	{
		this->node.Mmw_valid=0;
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
	.vtbl=vtbl_def,
//	.vtbl={object_init,
//			object_update,
//			object_collision,
//			object_render,
//			object_at_free
//	},
	.type={{0,0,0,0,0,0,0,0}},
	.ptr_to_bits=0,
	.part={0,0,0,0},
};
//----------------------------------------------------------- ------ functions

inline static void object_update_bounding_radius_using_scale(object*o) {
	o->bvol.r=(bounding_radius)
		sqrtf(o->bvol.s.x*o->bvol.s.x+o->bvol.s.y*o->bvol.s.y);
}

//----------------------------------------------------------------------------

inline static const float*object_get_updated_matrix_model_to_world(object*o){
	if(o->node.Mmw_valid)
		return o->node.Mmw;

	mat4_set_translation(o->node.Mmw,
			&o->phy.p);

	mat4_append_rotation_about_z_axis(
			o->node.Mmw,o->phy.a.z);

	mat4_scale(o->node.Mmw,&o->bvol.s);

	o->node.Mmw_valid=1;

	return o->node.Mmw;
}

//----------------------------------------------------------------------------

inline static void object_render_glob(object*o) {
	if(!o->node.glo)
		return;
	const float*f=object_get_updated_matrix_model_to_world(o);
	glo_render(o->node.glo,f);
}

//----------------------------------------------------------------------------
