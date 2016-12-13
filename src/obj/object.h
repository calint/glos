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
	unsigned updtk;
	unsigned drwtk;
	node n;
	bvol b;
	phy p;
	vtbl v;
	type t;
	bits*alloc_bits_ptr;
	void*part[object_part_cap];
	str name;
}object;
//---------------------------------------------------------------------- -----
static object object_def={
	.updtk=0,
	.drwtk=0,
	.n=node_def,
	.b=bvol_def,
	.p=phy_def,
	.v=vtbl_def,
	.t=type_def,
	.part=parts_def,
	.alloc_bits_ptr=0,
	.name=str_def,
};
//----------------------------------------------------------------------- init
inline static void object_init(object*o){}
//--------------------------------------------------------------------- update
inline static void object_update(object*o,framectx*fc){
	vec3_inc_with_vec3_over_dt(&o->p.p,&o->p.v,fc->dt);
	vec3_inc_with_vec3_over_dt(&o->p.a,&o->p.av,fc->dt);
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
inline static void _object_free_(object*o){}
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
inline static void object_render_glo(object*o,framectx*fc) {
	if(!o->n.glo_ptr)
		return;

	const float*f=object_get_updated_Mmw(o);
	glo_render(o->n.glo_ptr,f);
}
//----------------------------------------------------------------------------



//	.vtbl={	.init=0,
//			.update=0,
//			.collision=0,
//			.render=0,
//			.free=0,
//	},
