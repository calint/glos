#pragma once
//---------------------------------------------------------------------- santa
#include"../lib.h"
#include"../obj.h"
//------------------------------------------------------------------------ def
static object santa_def={
	.n=node_def,
	.b={	.r=1.4f,
			.s={1,1,1,0}},
	.p={	.p={0,0,0,0},
			.v={0,0,0,0},
			.a={0,0,0,0},
			.av={0,0,0,0},},
	.v={	.init=_santa_init_,
			.update=NULL,
			.collision=NULL,
			.render=object_render_glob,
			.free=NULL,
	},
	.t={{'a','b',0,0,0,0,0,0}},
	.part={NULL,NULL,NULL,NULL},
	.ptr_to_bits=NULL,
};
//----------------------------------------------------------------------- init
inline static void _santa_init_(object*this){
	this->b.s=(scale){.5,.5,.5,0};
	this->p.av.z=90;
	this->p.p.x=.5f;
	bvol_update_radius_using_scale(&this->b);
}
//----------------------------------------------------------------------------
