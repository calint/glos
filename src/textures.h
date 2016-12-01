#pragma once

//-------------------------------------------------------------------- texture

#define texture_count 2

struct{
	SDL_Texture*ptr;
	int width;
	int height;
}texture[texture_count];

//----------------------------------------------------------------------- init

static inline void textures_init() {}

//----------------------------------------------------------------------- free

static inline void textures_free() {
	for (int i=0;i<texture_count;i++){
		SDL_DestroyTexture(texture[i].ptr);
	}
}

//----------------------------------------------------------------------------

