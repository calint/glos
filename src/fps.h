#pragma once
#include "sdl.h"
//------------------------------------------------------------------ fps
static Uint32 fps_average_during_last_intervall;
static Uint32 fps_frame_count;
static Uint32 fps_calculation_intervall_in_ms;
static Uint32 fps_time_at_start_of_intervall_in_ms;

static double fps_dt;
static Uint64 fps_timer_frequency;
static Uint64 fps_timer_tick_at_start_of_frame;
//------------------------------------------------------------------ init
static inline void fps_init() {
	fps_average_during_last_intervall = 0;
	fps_frame_count = 0;
	fps_calculation_intervall_in_ms = 1000;
	fps_time_at_start_of_intervall_in_ms = SDL_GetTicks();

	fps_timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
	fps_timer_frequency = SDL_GetPerformanceFrequency();
}
//------------------------------------------------------------------ free
static inline void fps_free() {
}
//------------------------------------------------------------------
static inline void fps_counter_at_frame_start() {
	fps_frame_count++;
}
//------------------------------------------------------------------
static inline void fps_counter_at_frame_done() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - fps_timer_tick_at_start_of_frame;
		fps_timer_tick_at_start_of_frame = t1;
		fps_dt = (double) dt
				/ (double) fps_timer_frequency;

		if (fps_dt > .1)
			fps_dt = .1;

		if (fps_dt == 0)
			fps_dt = .00001; //? min double?

	}

	Uint32 t1 = SDL_GetTicks();

	Uint32 dt = t1 - fps_time_at_start_of_intervall_in_ms;

	if (dt < fps_calculation_intervall_in_ms)
		return;

	if (dt != 0) {
		fps_average_during_last_intervall = fps_frame_count * 1000 / dt;
	} else {
		fps_average_during_last_intervall = 0;
	}

	printf(" fps average: %d    dt: %f\n", fps_average_during_last_intervall,
			fps_dt);

	fps_frame_count = 0;

	fps_time_at_start_of_intervall_in_ms = t1;

}
//------------------------------------------------------------------ fps
