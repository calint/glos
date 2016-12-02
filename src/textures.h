#pragma once

//-------------------------------------------------------------------- texture

#define texture_count 1024
struct{

	SDL_Texture*ptr;

	int width;

	int height;

}texture[texture_count];
int texture_next_free;

//----------------------------------------------------------------------- init

static inline void init_textures(){}

//----------------------------------------------------------------------- free

static inline void free_textures(){
	for (int i=0;i<texture_count;i++){
		SDL_DestroyTexture(texture[i].ptr);
	}
}

//----------------------------------------------------------------------------

static inline void textures_alloc(
	int number_of_textures,
	int*begin_index,
	int*end_index_not_included
){
	if(texture_next_free+number_of_textures>=texture_count){
		perror("buffer overrun");
	}
	texture_next_free+=number_of_textures;
}

