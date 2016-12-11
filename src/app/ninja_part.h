//------------------------------------------------------------------ ninja_part
#pragma once
//------------------------------------------------------------------- overrides
static void ninja_part_init(object*o,part*this);
static void ninja_part_update(object*o,part*this,dt dt);
static void ninja_part_render(object*o,part*this);
static void ninja_part_free(object*o,part*this);
//---------------------------------------------------------------------- define
typedef struct ninja_part{
	part part;
	float time_duration_per_frame;
	int texture_index_for_first_frame;
	int texture_index_for_last_frame;
	int current_texture_index;
	dt _animation_time_left_for_this_frame;
}ninja_part;
//--------------------------------------------------------------------- default
static ninja_part ninja_part_def={
	.part=(part){
		.init=ninja_part_init,
		.update=ninja_part_update,
		.render=ninja_part_render,
		.free=ninja_part_free,
	},
	.time_duration_per_frame=0,
	.texture_index_for_first_frame=0,
	.texture_index_for_last_frame=0,
	.current_texture_index=0,
	._animation_time_left_for_this_frame=0,
};
//-------------------------------------------------------------- implementation
static void ninja_part_update(object*o,part*this,dt dt){
//	printf(" %s %u : [ %p %p ]\n",__FILE__,__LINE__,(void*)o,(void*)this);
	if(o->position.x>1){
		o->position.x=1;
		o->velocity.x=-o->velocity.x;
		o->angular_velocity.z=-o->angular_velocity.z;
	}else if(o->position.x<-1){
		o->position.x=-1;
		o->velocity.x=-o->velocity.x;
		o->angular_velocity.z=-o->angular_velocity.z;
	}
	if(o->position.y>1){
		o->position.y=1;
		o->velocity.y=-o->velocity.y;
		o->angular_velocity.z=-o->angular_velocity.z;
	}else if(o->position.y<-1){
		o->position.y=-1;
		o->velocity.y=-o->velocity.y;
		o->angular_velocity.z=-o->angular_velocity.z;
	}
}
//-------------------------------------------------------------- implementation
static void ninja_part_render(object*o,part*this){}
static void ninja_part_init(object*o,part*this){}
static void ninja_part_free(object*o,part*this){}
//-----------------------------------------------------------------------------
