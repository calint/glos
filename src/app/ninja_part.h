//------------------------------------------------------------------ ninja_part
#pragma once
//------------------------------------------------------------------- overrides
static void _ninja_part_init_(object*o,part*this);
static void _ninja_part_update_(object*o,part*this,dt dt);
static void _ninja_part_render_(object*o,part*this);
static void _ninja_part_free_(object*o,part*this);
//---------------------------------------------------------------------- define
typedef struct ninja_part{
	part part;
	float time_duration_per_frame;
	int texture_index_for_first_frame;
	int texture_index_for_last_frame;
	int current_texture_index;
	dt animation_time_left_for_current_frame;
}ninja_part;
//--------------------------------------------------------------------- default
static ninja_part _ninja_part_={
	.part=(part){
		.init=_ninja_part_init_,
		.update=_ninja_part_update_,
		.render=_ninja_part_render_,
		.free=_ninja_part_free_,
	},
	.time_duration_per_frame=0,
	.texture_index_for_first_frame=0,
	.texture_index_for_last_frame=0,
	.current_texture_index=0,
	.animation_time_left_for_current_frame=0,
};
//----------------------------------------------------------------------- alloc
/**gives*/ninja_part*ninja_part_alloc(
		ninja_part*initializer
	){
	ninja_part*a=malloc(sizeof(ninja_part));
	if(!a){
		perror("out of memory");
		fprintf(stderr,"\t%s\n\n%s %d\n","",__FILE__,__LINE__);
		exit(21);
	}

	*a=initializer?*initializer:_ninja_part_;
	return a;
}
//-------------------------------------------------------------- implementation
static void _ninja_part_update_(object*o,part*this,dt dt){
	if(o->position.x>1){
		o->position.x=1;
		o->velocity.x=-o->velocity.x;
		o->angular_velocity.z=-o->angular_velocity.z;
	}else if(o->position.x<-1){
		o->position.x=-1;
		o->velocity.x=-o->velocity.x;
		o->angular_velocity.z=-o->angular_velocity.z;
	}
}
//-------------------------------------------------------------- implementation
static void _ninja_part_render_(object*o,part*this){
//	drawables_draw(4);
}
static void _ninja_part_init_(object*o,part*this){
//	printf(" * new %-12s [ %p %p ]\n","ninja_part",o,this);
}
static void _ninja_part_free_(object*o,part*this){
//	printf(" * del %-12s [ %p %p ]\n","ninja_part",o,this);
}
//-----------------------------------------------------------------------------
