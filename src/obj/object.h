#pragma once
//--------------------------------------------------------------------- object
#include"../lib.h"
#include"node.h"
#include"bvol.h"
#include"phy.h"
#include"vtbl.h"
#include"../grid/grid-ifc.h"
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
	phy p_prv; // physics state from previous frame
//	phy p_nxt; // physics state for next frame
	grid_ifc g;
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
	.p_prv=phy_def,
//	.p_nxt=phy_def,
	.g=grid_ifc_def,
	.v=vtbl_def,
	.t=type_def,
	.part=parts_def,
	.alloc_bits_ptr=0,
	.name=str_def,
};
//----------------------------------------------------------------------------
inline static void object_update(object*o,framectx*fc){
	if(o->n.Mmw_valid && ( // if matrix is flagged valid and
			!vec3_equals(&o->p_prv.p,&o->p.p) || // if position
			!vec3_equals(&o->p_prv.a,&o->p.a) )){ // or angle changed
		o->n.Mmw_valid=0; // invalidate matrix
	}

	o->p_prv=o->p;

	phy*p=&o->p;
	const dt dt=fc->dt;
	vec3_inc_with_vec3_over_dt(&p->p,&p->v,dt);
	vec3_inc_with_vec3_over_dt(&p->a,&p->av,dt);
}
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
inline static void _object_render_glo_(object*o,framectx*fc) {
	if(!o->n.glo_ptr)
		return;

	const float*f=object_get_updated_Mmw(o);
	glo_render(o->n.glo_ptr,f);
}
//----------------------------------------------------------------------------
