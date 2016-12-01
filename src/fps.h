#pragma once
#include "lib.h"

//------------------------------------------------------------------------ fps

struct {

	Uint32 average_during_last_intervall;
	Uint32 frame_count;
	Uint32 calculation_intervall_in_ms;
	Uint32 time_at_start_of_intervall_in_ms;

	dt dt;
	Uint64 timer_frequency;
	Uint64 timer_tick_at_start_of_frame;

} fps;

//----------------------------------------------------------------------- init

static inline void fps_init() {
	fps.average_during_last_intervall = 0;
	fps.frame_count = 0;
	fps.calculation_intervall_in_ms = 1000;
	fps.time_at_start_of_intervall_in_ms = SDL_GetTicks();

	fps.timer_tick_at_start_of_frame = SDL_GetPerformanceCounter();
	fps.timer_frequency = SDL_GetPerformanceFrequency();
}

//----------------------------------------------------------------------- free

static inline void fps_free() {
}

//----------------------------------------------------------------------------

static inline void fps_at_frame_start() {
	fps.frame_count++;
}

//----------------------------------------------------------------------------

static inline void fps_at_frame_done() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - fps.timer_tick_at_start_of_frame;
		fps.timer_tick_at_start_of_frame = t1;
		fps.dt = (float) dt / (float) fps.timer_frequency;

		if (fps.dt > .1)
			fps.dt = .1f;

		if (fps.dt == 0)
			fps.dt = .00001f; //? min float?

	}

	Uint32 t1 = SDL_GetTicks();

	Uint32 dt = t1 - fps.time_at_start_of_intervall_in_ms;

	if (dt < fps.calculation_intervall_in_ms)
		return;

	if (dt != 0) {
		fps.average_during_last_intervall = fps.frame_count * 1000 / dt;
	} else {
		fps.average_during_last_intervall = 0;
	}

	printf(" fps average: %d    dt: %f\n", fps.average_during_last_intervall,
			fps.dt);

	fps.frame_count = 0;

	fps.time_at_start_of_intervall_in_ms = t1;

}

//----------------------------------------------------------------------------
