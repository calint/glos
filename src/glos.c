#include "SDL.h"

static SDL_Rect sprites_pos[2];
static SDL_Surface*sprites_image[2];

static inline void sprites_init() {
	SDL_Surface* temp = SDL_LoadBMP("logo.bmp");
	SDL_Surface* bg = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	sprites_image[0]=bg;
	sprites_image[1]=bg;

	sprites_pos[0].x=100;
	sprites_pos[0].y=100;
	sprites_pos[0].w=74;
	sprites_pos[0].h=55;

	sprites_pos[1].x=200;
	sprites_pos[1].y=100;
	sprites_pos[1].w=74;
	sprites_pos[1].h=55;
}

static inline void sprites_free() {
	for (int i = 0; i < sizeof sprites_pos; i++) {
		SDL_FreeSurface(sprites_image[i]);
	}
}

static inline void sprites_render(SDL_Surface*s) {
	for (int i = 0; i < sizeof sprites_pos; i++) {
		SDL_BlitSurface(sprites_image[i], NULL, s, &sprites_pos[i]);
	}
}

int main(int argc, char *argv[]) {
	/* initialize SDL */
	SDL_Init(SDL_INIT_VIDEO);

	/* set the title bar */
	SDL_WM_SetCaption("SDL Test", "SDL Test");

	/* create window */
	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, 0);

	/* load bitmap to temp surface */

	SDL_Event event;
	int gameover = 0;

	sprites_init();

	/* message pump */
	while (!gameover) {
		/* look for an event */
		if (SDL_PollEvent(&event)) {
			/* an event was found */
			switch (event.type) {
			/* close button clicked */
			case SDL_QUIT:
				gameover = 1;
				break;

				/* handle the keyboard */
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameover = 1;
					break;
				}
				break;
			}
		}

		sprites_render(screen);

		/* update the screen */
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}

	sprites_free();

	/* cleanup SDL */
	SDL_Quit();

	return 0;
}
