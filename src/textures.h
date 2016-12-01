#pragma once

//------------------------------------------------------------------ texture
#define texture_count 2
static SDL_Texture *texture[texture_count];

static inline void textures_init() {
	SDL_Surface *bmp;
	SDL_Texture *tex;

	// --- --- - -- - ----- - - - - --
	if (!(bmp = SDL_LoadBMP("logo.bmp")))
		exit(4);
	tex = SDL_CreateTextureFromSurface(renderer, bmp);
	if (!(tex = SDL_CreateTextureFromSurface(renderer, bmp)))
		exit(5);
	SDL_FreeSurface(bmp);
	texture[0] = tex;

	// --- --- - -- - ----- - - - - --
	if (!(bmp = SDL_LoadBMP("sdl_logo.bmp")))
		exit(4);
	tex = SDL_CreateTextureFromSurface(renderer, bmp);
	if (!(tex = SDL_CreateTextureFromSurface(renderer, bmp)))
		exit(5);
	SDL_FreeSurface(bmp);
	texture[1] = tex;
}

static inline void textures_free() {
	for (int i = 0; i < texture_count; i++) {
		SDL_DestroyTexture(texture[i]);
	}
}
//------------------------------------------------------------------ texture

