#pragma once
//--------------------------------------------------------------- santa
//------------------------------------------------------------------------ def
static object santa_def={
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
	.t={type_santa},
	.part={NULL,NULL,NULL,NULL},
	.alloc_bits_ptr=NULL,
};
//------------------------------------------------------------------ extension
typedef struct santa{
	part part;
	unsigned keybits;
}santa_ext;
//------------------------------------------------------------------ overrides
static void _santa_part_init(object*,part*);
static void _santa_update(object*,part*,dt);
static void _santa_part_render(object*,part*);
static void _santa_free(object*,part*);
//----------------------------------------------------------------------- init
static santa_ext santa_ext_def={
	.part=(part){
		.init=NULL,
		.update=_santa_update,
		.render=NULL,
		.free=NULL,
	},
	.keybits=0,
};
//----------------------------------------------------------------------- impl
static void _santa_part_init(object*po,part*o){}
static void _santa_update(object*po,part*o,dt dt){
//	printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
	santa_ext*p=(santa_ext*)o;
	unsigned n=p->keybits;
	if(n!=0){
		// wasd keys
		if(n&1)vec3_inc_with_vec3_over_dt(&po->p.p,&(velocity){0,0,-1,0},dt);
		if(n&2)vec3_inc_with_vec3_over_dt(&po->p.p,&(velocity){0,-1,0,0},dt);
		if(n&4)vec3_inc_with_vec3_over_dt(&po->p.p,&(velocity){0,0,1,0},dt);
		if(n&8)vec3_inc_with_vec3_over_dt(&po->p.p,&(velocity){0,1,0,0},dt);
		po->n.Mmw_valid=0;
		while(n){
			if(n&1)printf("1");
			else printf("0");
			n>>=1;
		}
		printf("\n");
		printf(" %f %f %f\n",po->p.p.x,po->p.p.y,po->p.p.z);
	}
}
static void _santa_part_render(object*po,part*o){}
static void _santa_part_free(object*po,part*o){}
//printf("%s:%u  [ %p %p ]\n",__FILE__,__LINE__,(void*)po,(void*)o);
//----------------------------------------------------------------------------


//----------------------------------------------------------------- alloc/free
inline static/*gives*/object*santa_alloc_def(){
	object*o=object_alloc(&santa_def);
	santa_ext*p=malloc(sizeof(santa_ext));
	*p=santa_ext_def;
	o->part[0]=(part*)p;
	return o;
}
//----------------------------------------------------------------------------
