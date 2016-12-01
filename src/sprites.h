#pragma once
#include <stdlib.h>

#include "window.h"
#include "textures.h"
//------------------------------------------------------------------ sprites
#define sprite_count 1024
struct {
	float x, y, dx, dy;
	int w, h;
	SDL_Texture *img;
} sprite[sprite_count];

static inline void sprites_init() {
	for (int i = 0; i < sprite_count; i++) {
		sprite[i].img = texture[0];
		sprite[i].x = randomRange(0, window_width);
		sprite[i].y = randomRange(0, window_height);
		sprite[i].w = 74;
		sprite[i].h = 55;

		sprite[i].dx = randomRange(-5, 5);
		sprite[i].dy = randomRange(-5, 5);
	}
}

static inline void sprites_free() {}

static inline void sprites_render(SDL_Renderer*r) {

	for (int i = 0; i < sprite_count; i++) {
		SDL_RenderCopy(r, sprite[i].img, NULL, NULL);
	}

}
//------------------------------------------------------------------ sprites
