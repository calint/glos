#pragma once

//------------------------------------------------------------------------ part

typedef struct anim{

	float time_duration_per_frame;
	float animation_time_left_for_current_frame;
	int texture_index_for_first_frame;
	int texture_index_for_last_frame;
	int current_index_in_textures;

}anim;

//--------------------------------------------------------------------- default

static anim default_anim={
		.time_duration_per_frame=0,
		.animation_time_left_for_current_frame=0,
		.texture_index_for_first_frame=0,
		.texture_index_for_last_frame=0,
		.current_index_in_textures=0,
};

//----------------------------------------------------------------------------

static void update_anim(void*this,dt dt){
	anim*a=this;

	a->animation_time_left_for_current_frame-=dt;
	if(a->animation_time_left_for_current_frame<0){ // next frame
		a->current_index_in_textures++;
		if(a->current_index_in_textures>a->texture_index_for_last_frame){
			a->current_index_in_textures=a->texture_index_for_first_frame;
		}
//		printf(" %p  frame   %d\n",(void*)a,a->current_index_in_textures);
		a->animation_time_left_for_current_frame+=a->time_duration_per_frame;
	}
}

//----------------------------------------------------------------------------
