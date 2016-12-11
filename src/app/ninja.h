#pragma once
//---------------------------------------------------------------------- ninja
//------------------------------------------------------------------------ def
static object ninja_def={
	.n=node_def,
	.b={	.r=1.4f,
			.s={1,1,1,0}},
	.p={	.p={0,0,0,0},
			.v={0,0,0,0},
			.a={0,0,0,0},
			.av={0,0,0,0},},
	.v={	.init=NULL,
			.update=NULL,
			.collision=NULL,
			.render=object_render_glob,
			.free=NULL,
	},
	.t={{'a','a',0,0,0,0,0,0}},
	.part={NULL,NULL,NULL,NULL},
	.ptr_to_bits=NULL,
};
//------------------------------------------------------------------ extension
typedef struct ninja{
	part part;
	unsigned stars;
}ninja_ext;
//------------------------------------------------------------------ overrides
//static void _ninja_part_init(object*,part*);
static void _ninja_update(object*,part*,dt);
//static void _ninja_part_render(object*,part*);
//static void _ninja_free(object*,part*);
//----------------------------------------------------------------------- init
static ninja_ext ninja_part_def={
	.part=(part){
		.init=NULL,
		.update=_ninja_update,
		.render=NULL,
		.free=NULL,
	},
	.stars=7,
};
//----------------------------------------------------------------------- impl
static void _ninja_part_init(object*po,part*o){}
static void _ninja_update(object*po,part*o,dt dt){}
static void _ninja_part_render(object*po,part*o){}
static void _ninja_part_free(object*po,part*o){}
//printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
//----------------------------------------------------------------------------


//----------------------------------------------------------------- alloc/free
inline static/*gives*/object*ninja_alloc_def(){
	object*o=object_alloc(&ninja_def);
	ninja_ext*p=malloc(sizeof(ninja_ext));
	*p=ninja_part_def;
	o->part[0]=(part*)p;
	return o;
}
//----------------------------------------------------------------------------
