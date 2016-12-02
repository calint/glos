#pragma once
#include <stdlib.h>
#include "window.h"
#include "textures.h"

//--------------------------------------------------------------------- sprites

#define sprite_count 8

struct {

	double x, y;

	double dx, dy;

	int w, h;

	SDL_Texture *texture;

} sprite[sprite_count];

//------------------------------------------------------------------------ init

static inline void sprites_init() {
	for (int i = 0; i < sprite_count; i++) {
		sprite[i].texture = texture[0].ptr;
		sprite[i].x = random_range(0, window_width);
		sprite[i].y = random_range(0, window_height);
		sprite[i].w = 74;
		sprite[i].h = 55;

		sprite[i].dx = random_range(-5, 5);
		sprite[i].dy = random_range(-5, 5);
	}
}

//------------------------------------------------------------------------ free

static inline void sprites_free() {
}

//---------------------------------------------------------------------- update

static inline void sprites_update(double dt) {
	for (int i = 0; i < sprite_count; i++) {
		sprite[i].x += sprite[i].dx * dt;
		sprite[i].y += sprite[i].dy * dt;
	}
}

//=--------------------------------------------------------------------- render

static inline void sprites_render(SDL_Renderer*r) {
	for (int i = 0; i < sprite_count; i++) {

		SDL_Rect dest = { (int) sprite[i].x, (int) sprite[i].y,
				(int) sprite[i].w, (int) sprite[i].h };

		SDL_RenderCopy(r, sprite[i].texture, NULL, &dest);

	}

}

//-----------------------------------------------------------------------------
