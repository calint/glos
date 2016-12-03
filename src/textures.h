#pragma once

//-------------------------------------------------------------------- texture

#define texture_count 1024
struct{
	SDL_Texture*ptr;
	int width;
	int height;
}texture[texture_count];
//int texture_next_index;

//----------------------------------------------------------------------- init

static inline void init_textures(){}

//----------------------------------------------------------------------- free

static inline void free_textures(){
	for (int i=0;i<texture_count;i++){
		SDL_DestroyTexture(texture[i].ptr);
	}
}

//----------------------------------------------------------------------------

inline static void load_texture(int id,const char*file_path){
	SDL_Surface*loaded_surface=IMG_Load(file_path);
	if(!loaded_surface){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(12);
	}
	SDL_Texture*tex=SDL_CreateTextureFromSurface(
			window.renderer,
			loaded_surface);
	if(!tex){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(13);
	}
	texture[id].ptr=tex;
	SDL_QueryTexture(tex,NULL,NULL,&texture[id].width,&texture[id].height);
	SDL_FreeSurface(loaded_surface);
}

//
//static inline void alloc_textures(
//	int number_of_textures,
//	int*begin_index,
//	int*end_index_not_included
//){
//	if(texture_next_index+number_of_textures>=texture_count){
//		perror("buffer overrun");
//	}
//	texture_next_index+=number_of_textures;
//}
//
