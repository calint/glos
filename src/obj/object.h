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
	phy p_nxt; // physics state for next frame
	position coloff;
	dt velswapdt;
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
	.p_nxt=phy_def,
	.coloff=vec4_def,
	.velswapdt=0,
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
	o->p=o->p_nxt;

	phy*p=&o->p;
	const dt dt=fc->dt;
	vec3_inc_with_vec3_over_dt(&p->p,&p->v,dt);
	vec3_inc_with_vec3_over_dt(&p->a,&p->av,dt);
	o->p_nxt=o->p;
}
inline static const float*object_get_updated_Mmw(object*o){
	if(o->n.Mmw_valid)
		return o->n.Mmw;

	mat4_set_translation(o->n.Mmw,
			&o->p.p);

	mat4_append_rotation_about_z_axis(
			o->n.Mmw,o->p.a.z);

	mat3_scale(o->n.Mmw,&o->b.s);

	o->n.Mmw_valid=1;

	// printf("\n-- %s\n",o->name.data);
	// printf("%f %f %f %f\n",o->n.Mmw[0],o->n.Mmw[1],o->n.Mmw[2],o->n.Mmw[3]);
	// printf("%f %f %f %f\n",o->n.Mmw[4],o->n.Mmw[5],o->n.Mmw[6],o->n.Mmw[7]);
	// printf("%f %f %f %f\n",o->n.Mmw[8],o->n.Mmw[9],o->n.Mmw[10],o->n.Mmw[11]);
	// printf("\npos: %f, %f, %f\n",o->p.p.x,o->p.p.y,o->p.p.z);
	return o->n.Mmw;
}
inline static void _object_render_glo_(object*o,framectx*fc) {
	if(!o->n.glo_ptr)
		return;

	const float*f=object_get_updated_Mmw(o);
	glo_render(o->n.glo_ptr,f);
}
//----------------------------------------------------------------------------
