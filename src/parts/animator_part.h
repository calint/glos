#pragma once
//------------------------------------------------------------------------ part
typedef struct animator_part{
	part part;
	float time_duration_per_frame;
	int texture_index_for_first_frame;
	int texture_index_for_last_frame;
	int current_texture_index;
	dt animation_time_left_for_current_frame;

}animator_part;
//--------------------------------------------------------------------- default
static animator_part default_animator_part={
	.part={1},
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

	*a=initializer?*initializer:default_animator_part;
	return a;
}
//---------------------------------------------------------------------- update
static void update_animator_part(animator_part*a,dt dt){
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
//------------------------------------------------------------------------- lib
inline static animator_part*animator_part_(void*p){
	return(animator_part*)p;
}
