#pragma once
//--------------------------------------------------------------------- object
#include"../lib.h"
#include"node.h"
#include"bvol.h"
#include"phy.h"
#include"vtbl.h"
//------------------------------------------------------------------------ def
#define object_part_cap 5
#define parts_def {0,0,0,0,0}
typedef struct part part;
typedef struct object{
	node n;
	bvol b;
	phy p;
	vtbl v;
	type t;
	bits*ptr_to_bits;
	part*part[object_part_cap];
}object;
//---------------------------------------------------------------------- -----
static object object_def={
	.n=node_def,
	.b=bvol_def,
	.p=phy_def,
	.v=vtbl_def,
	.t=type_def,
	.part=parts_def,
	.ptr_to_bits=0,
};
//----------------------------------------------------------------------- init
inline static void object_init(object*o){}
//--------------------------------------------------------------------- update
inline static void object_update(object*o,dt dt){
	vec3_inc_with_vec3_over_dt(&o->p.p,&o->p.v,dt);
	vec3_inc_with_vec3_over_dt(&o->p.a,&o->p.av,dt);
	if(o->n.Mmw_valid &&
		(o->p.v.x||o->p.v.y||o->p.v.z||
		o->p.av.x||o->p.av.y||
		o->p.av.z))
	{
		o->n.Mmw_valid=0;
	}
}
//------------------------------------------------------------------ collision
inline static void object_collision(object*o,object*other,dt dt){}
//--------------------------------------------------------------------- render
inline static void object_render(object*o){}
//----------------------------------------------------------------------- free
inline static void object_free(object*o){}
//----------------------------------------------------------------------------








//----------------------------------------------------------- ------------ lib
inline static const float*object_get_updated_Mmw(object*o){
	if(o->n.Mmw_valid)
		return o->n.Mmw;

	mat4_set_translation(o->n.Mmw,
			&o->p.p);

	mat4_append_rotation_about_z_axis(
			o->n.Mmw,o->p.a.z);

	mat4_scale(o->n.Mmw,&o->b.s);

	o->n.Mmw_valid=1;

	return o->n.Mmw;
}
//----------------------------------------------------------------------------
inline static void object_render_glob(object*o) {
	if(!o->n.glo)
		return;

	const float*f=object_get_updated_Mmw(o);
	glo_render(o->n.glo,f);
}
//----------------------------------------------------------------------------



//	.vtbl={	.init=0,
//			.update=0,
//			.collision=0,
//			.render=0,
//			.free=0,
//	},
