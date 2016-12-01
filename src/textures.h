#pragma once

//------------------------------------------------------------------ texture
#define texture_count 1
static SDL_Texture *texture[texture_count];

static inline void textures_init() {
	SDL_Surface *bmp = SDL_LoadBMP("logo.bmp");
	if (!bmp)
		exit(4);

	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, bmp);
	SDL_FreeSurface(bmp);
	if (!tex)
		exit(5);

	texture[0] = tex;
}

static inline void texture_free() {
	for (int i = 0; i < texture_count; i++) {
		SDL_DestroyTexture(texture[i]);
	}
}
//------------------------------------------------------------------ texture

