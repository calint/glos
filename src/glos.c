#include "SDL.h"

//------------------------------------------------------------------ library
static inline float random() {
	return rand() / (float) RAND_MAX;
}

static inline int randomRange(int low, int high) {
	int range = high - low;
	return (int) (random() * range) + low;
}
//------------------------------------------------------------------ screen
#define screen_width 512
#define screen_height 512
static SDL_Window *win;
static SDL_Renderer *ren;
static inline void screen_init() {

	win = SDL_CreateWindow("Hello World!", 100, 100, screen_width,
	screen_height, SDL_WINDOW_SHOWN);
	if (!win) {
		SDL_Quit();
		exit(2);
	}

	ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!ren) {
		SDL_DestroyWindow(win);
		SDL_Quit();
		exit(3);
	}

}

static inline void screen_free() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
}

//------------------------------------------------------------------ surfaces
#define texture_count 1
static SDL_Texture *texture[texture_count];

static inline void textures_init() {
	SDL_Surface *bmp = SDL_LoadBMP("logo.bmp");
	if (!bmp) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		exit(4);
	}

	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_FreeSurface(bmp);
	if (!tex) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		exit(5);
	}

	texture[0] = tex;
}

static inline void texture_free() {
	for (int i = 0; i < texture_count; i++) {
		SDL_DestroyTexture(texture[i]);
	}
}

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
		sprite[i].x = randomRange(0, screen_width);
		sprite[i].y = randomRange(0, screen_height);
		sprite[i].w = 74;
		sprite[i].h = 55;

		sprite[i].dx = randomRange(-5, 5);
		sprite[i].dy = randomRange(-5, 5);
	}
}

static inline void sprites_update() {
}

static inline void sprites_render(SDL_Renderer*r) {
	SDL_RenderClear(r);

	for (int i = 0; i < sprite_count; i++) {
		SDL_RenderCopy(r, sprite[0].img, NULL, NULL);
	}

	SDL_RenderPresent(ren);
}

//--------------------------------------------------------------------- main

void print_SDL_version(char* preamble, SDL_version* v) {
	printf("%s %u.%u.%u\n", preamble, v->major, v->minor, v->patch);
}

void print_SDL_versions() {

	SDL_version ver;

	SDL_VERSION(&ver);

	print_SDL_version("SDL compile-time version", &ver);

}

int main(int argc, char *argv[]) {
	print_SDL_versions();

	if (SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Quit();
		return 1;
	}

	screen_init();

	textures_init();

	sprites_init();

	Uint32 currentFPS = 0;

	int frame_count = 0;

	Uint32 calculate_fps_every_x_ms = 1000;

	Uint32 t0 = SDL_GetTicks();

	SDL_Event event;

	int gameover = 0;

	while (!gameover) {

		frame_count++;

		if (SDL_PollEvent(&event)) {

			switch (event.type) {

			case SDL_QUIT:
				gameover = 1;
				break;

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

		sprites_render(ren);

		Uint32 dt = SDL_GetTicks() - t0;

		if (dt < calculate_fps_every_x_ms)
			continue;

		if (dt != 0) {
			currentFPS = frame_count * 1000 / dt;
		} else {
			currentFPS = 0;
		}

		printf(" fps: %d\n", currentFPS);

		t0 = SDL_GetTicks();
	}

	screen_free();

	SDL_Quit();

	return 0;
}
