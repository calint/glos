#pragma once

//------------------------------------------------------------------------ part

typedef struct animator_part{

	float time_duration_per_frame;

	int texture_index_for_first_frame;

	int texture_index_for_last_frame;

	int current_index_in_textures;

	dt animation_time_left_for_current_frame;

}animator_part;

//--------------------------------------------------------------------- default

static animator_part anim={
		.time_duration_per_frame=0,
		.texture_index_for_first_frame=0,
		.texture_index_for_last_frame=0,
		.current_index_in_textures=0,
		.animation_time_left_for_current_frame=0,
};

//----------------------------------------------------------------------------

static void update_anim(void*an_anim,dt dt){
	animator_part*a=an_anim;
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
