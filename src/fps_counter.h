#pragma once
#include "sdl.h"
//------------------------------------------------------------------ fps
static Uint32 fps_counter_fps_average;
static Uint32 fps_counter_frame_count;
static Uint32 fps_counter_calculate_fps_every_x_ms;
static Uint32 fps_counter_t0;

static Uint64 fps_counter_frequency;
static Uint64 fps_counter_time_at_start_of_frame;
static double fps_counter_delta_time_for_last_frame;
//------------------------------------------------------------------ init
static inline void fps_counter_init() {
	fps_counter_fps_average = 0;
	fps_counter_frame_count = 0;
	fps_counter_calculate_fps_every_x_ms = 1000;
	fps_counter_t0 = SDL_GetTicks();

	fps_counter_time_at_start_of_frame = SDL_GetPerformanceCounter();
	fps_counter_frequency = SDL_GetPerformanceFrequency();
}
//------------------------------------------------------------------ free
static inline void fps_counter_free() {
}
//------------------------------------------------------------------
static inline void fps_counter_before_frame() {
	fps_counter_frame_count++;
}
//------------------------------------------------------------------
static inline void fps_counter_after_frame() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - fps_counter_time_at_start_of_frame;
		fps_counter_time_at_start_of_frame = t1;
		fps_counter_delta_time_for_last_frame = (double) dt
				/ (double) fps_counter_frequency;

		if (fps_counter_delta_time_for_last_frame > .1)
			fps_counter_delta_time_for_last_frame = .1;

		if (fps_counter_delta_time_for_last_frame == 0)
			fps_counter_delta_time_for_last_frame = .00001; //? min double?

	}

	Uint32 t1 = SDL_GetTicks();

	Uint32 dt = t1 - fps_counter_t0;

	if (dt < fps_counter_calculate_fps_every_x_ms)
		return;

	if (dt != 0) {
		fps_counter_fps_average = fps_counter_frame_count * 1000 / dt;
	} else {
		fps_counter_fps_average = 0;
	}

	printf(" fps average: %d    dt: %f\n", fps_counter_fps_average,
			fps_counter_delta_time_for_last_frame);

	fps_counter_frame_count = 0;

	fps_counter_t0 = t1;

}
//------------------------------------------------------------------ fps
