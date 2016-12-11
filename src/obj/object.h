#pragma once
//--------------------------------------------------------------------- object
#include"../lib.h"
#include"node.h"
#include"bvol.h"
#include"phy.h"
#include"vtbl.h"
//------------------------------------------------------------------------ def
#define object_part_cap 5
typedef struct part part;
typedef struct object{
	node node;
	bvol bvol;
	phy phy;
	vtbl vtbl;
	type type;
	bits*ptr_to_bits;
	part*part[object_part_cap];
}object;
//---------------------------------------------------------------------- -----
static object object_def={
	.node=node_def,
	.bvol=bvol_def,
	.phy=phy_def,
	.vtbl=vtbl_def,
//	.vtbl={	.init=0,
//			.update=0,
//			.collision=0,
//			.render=0,
//			.free=0,
//	},
	.type=type_def,
	.ptr_to_bits=0,
	.part={0,0,0,0},
};
//---------------------------------------------------------------------- init
inline static void object_init(object*o){}
//--------------------------------------------------------------------- update
inline static void object_update(object*o,dt dt){
	vec3_inc_with_vec3_over_dt(&o->phy.p,&o->phy.v,dt);
	vec3_inc_with_vec3_over_dt(&o->phy.a,&o->phy.av,dt);
	if(o->node.Mmw_valid &&
		(o->phy.v.x||o->phy.v.y||o->phy.v.z||
		o->phy.av.x||o->phy.av.y||
		o->phy.av.z))
	{
		o->node.Mmw_valid=0;
	}
}
//------------------------------------------------------------------ collision
inline static void object_collision(object*o,object*other,dt dt){}
//--------------------------------------------------------------------- render
inline static void object_render(object*o){}
//----------------------------------------------------------------------- free
inline static void object_free(object*o){}
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
