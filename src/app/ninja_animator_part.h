#pragma once
//#include"../object.h"
//------------------------------------------------------------------------ part

typedef struct ninja_animator_part{
	part part;
	float time_duration_per_frame;
	int texture_index_for_first_frame;
	int texture_index_for_last_frame;
	int current_texture_index;
	dt animation_time_left_for_current_frame;
}ninja_animator_part;

//---------------------------------------------------------------------- update
static void update_ninja_animator_part(object*o,part*this,dt dt){
//	printf("update ninja part");
}

//--------------------------------------------------------------------- default

static ninja_animator_part default_ninja_animator_part={
	.part=(part){
		.type_id=3,
		.init=0,
		.update=update_ninja_animator_part,
		.render=0,
		.free=0,
	},
	.time_duration_per_frame=0,
	.texture_index_for_first_frame=0,
	.texture_index_for_last_frame=0,
	.current_texture_index=0,
	.animation_time_left_for_current_frame=0,
};

//----------------------------------------------------------------------- alloc
/**gives*/ninja_animator_part*alloc_ninja_animator_part(
		ninja_animator_part*initializer
	){
	ninja_animator_part*a=malloc(sizeof(ninja_animator_part));
	if(!a){
		perror("out of memory");
		exit(21);
	}

	*a=initializer?*initializer:default_ninja_animator_part;
	return a;
}

