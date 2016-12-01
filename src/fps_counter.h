#pragma once

//------------------------------------------------------------------ fps

static Uint32 fps_counter_fps;
static Uint32 fps_counter_frame_count;
static Uint32 fps_counter_calculate_fps_every_x_ms;
static Uint32 fps_counter_t0;

static inline void fps_counter_init() {
	fps_counter_fps = 0;
	fps_counter_frame_count = 0;
	fps_counter_calculate_fps_every_x_ms = 1000;
	fps_counter_t0 = SDL_GetTicks();
}

static inline void fps_counter_free() {}

static inline void fps_counter_before_frame() {
	fps_counter_frame_count++;
}

static inline void fps_counter_after_frame() {

	Uint32 dt = SDL_GetTicks() - fps_counter_t0;

	if (dt < fps_counter_calculate_fps_every_x_ms)
		return;

	if (dt != 0) {
		fps_counter_fps = fps_counter_frame_count * 1000 / dt;
	} else {
		fps_counter_fps = 0;
	}

	printf(" fps: %d\n", fps_counter_fps);

	fps_counter_t0 = SDL_GetTicks();

}

//------------------------------------------------------------------ fps
