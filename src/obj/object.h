#pragma once
#include"../lib.h"
#include"node.h"
#include"bvol.h"
#include"phy.h"
#include"vtbl.h"
//---------------------------------------------------------------------- config

#define object_part_cap 5

//------------------------------------------------------------------------ def
typedef struct object{
	node node;
	bvol bvol;
	phy phy;
	vtbl vtbl;
	type type;
	bits*ptr_to_bits;
	void*part[object_part_cap];
}object;
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
//---------------------------------------------------------------------- init
inline static void object_init(object*this){}
//--------------------------------------------------------------------- update
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
//------------------------------------------------------------------ collision
inline static void object_collision(object*this,object*other,dt dt){}
//--------------------------------------------------------------------- render
inline static void object_render(object*this){}
//----------------------------------------------------------------------- free
inline static void object_at_free(object*this){}
//----------------------------------------------------------- ------------ lib

inline static const float*object_get_updated_Mmw(object*o){
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

	const float*f=object_get_updated_Mmw(o);
	glo_render(o->node.glo,f);
}

//----------------------------------------------------------------------------
