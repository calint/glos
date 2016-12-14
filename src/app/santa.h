#pragma once
//--------------------------------------------------------------- santa
//------------------------------------------------------------------------ def
static object santa_obj_def={
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
			.render=_object_render_glo_,
			.free=NULL,
	},
	.t={type_santa},
	.part={NULL,NULL,NULL,NULL},
	.alloc_bits_ptr=NULL,
};
//------------------------------------------------------------------ extension
typedef struct santa{
	part part;
	int32_t*keybits_ptr;
	glo*bounding_glo_ptr;
}santa;
//------------------------------------------------------------------ overrides
static void _santa_init(object*,part*);
static void _santa_update(object*,part*,framectx*fc);
static void _santa_render(object*,part*,framectx*fc);
static void _santa_free(object*,part*);
//----------------------------------------------------------------------- init
static santa santa_def={
	.part=(part){
		.init=NULL,
		.update=_santa_update,
		.render=_santa_render,
		.free=NULL,
	},
	.keybits_ptr=0,
	.bounding_glo_ptr=NULL,
};
//----------------------------------------------------------------------- impl
static void _santa_update(object*po,part*o,framectx*fc){
//	printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
	santa*p=(santa*)o;
	if(!p->keybits_ptr)
		return;
	int n=*p->keybits_ptr;
	velocity*v=&po->p.v;
	*v=vec4_def;
	if(n!=0){
		// wasd keys
		if(n&1)v->z+=+1;
		if(n&2)v->x+=-1;
		if(n&4)v->z+=-1;
		if(n&8)v->x+=+1;
		vec3_scale(v,10);
		po->n.Mmw_valid=0;
	}
}
static void _santa_render(object*o,part*po,framectx*fc){
	santa*p=(santa*)po;
	if(!p->bounding_glo_ptr)
		return;
	const float*f=object_get_updated_Mmw(o);
	glo_render(p->bounding_glo_ptr,f);
}
static void _santa_free(object*po,part*o){}
//printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
//----------------------------------------------------------------------------


//----------------------------------------------------------------- alloc/free
inline static/*gives*/object*santa_alloc_def(){
	object*o=object_alloc(&santa_obj_def);
	santa*p=malloc(sizeof(santa));
	*p=santa_def;
	o->part[0]=(part*)p;
	return o;
}
//----------------------------------------------------------------------------
