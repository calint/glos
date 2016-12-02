#pragma once

//------------------------------------------------------------------------ part

typedef struct animator_part{

	float time_duration_per_frame;

	int texture_index_for_first_frame;

	int texture_index_for_last_frame;

	int current_texture_index;

	dt_in_seconds animation_time_left_for_current_frame;

} animator_part;

//------------------------------------------------------------------------- lib

inline static animator_part*animator_part_(void*p){
	return(animator_part*)p;
}

//--------------------------------------------------------------------- default

static animator_part animator={
	.time_duration_per_frame=0,
	.texture_index_for_first_frame=0,
	.texture_index_for_last_frame=0,
	.current_texture_index=0,
	.animation_time_left_for_current_frame=0,
};

//----------------------------------------------------------------------- alloc

/**gives*/animator_part*alloc_animator_part(animator_part*initializer){
	animator_part*a=malloc(sizeof(animator_part));
	if(!a){
		perror("out of memory");
		exit(21);
	}

	*a=initializer?*initializer:animator;
	return a;
}

//---------------------------------------------------------------------- update

static void update_anim(void*an_anim,dt_in_seconds dt){
	animator_part*a=an_anim;
	a->animation_time_left_for_current_frame-=dt;
	if(a->animation_time_left_for_current_frame<0){ // next frame
		a->current_texture_index++;
		if(a->current_texture_index>a->texture_index_for_last_frame){
			a->current_texture_index=a->texture_index_for_first_frame;
		}
//		printf(" %p  frame   %d\n",(void*)a,a->current_index_in_textures);
		a->animation_time_left_for_current_frame+=a->time_duration_per_frame;
	}
}

//----------------------------------------------------------------------------
