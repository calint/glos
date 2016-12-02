#pragma once
#include "lib.h"

//------------------------------------------------------------------------ fps

struct{

	Uint32 average_during_last_intervall;

	Uint32 frame_count;

	Uint32 calculation_intervall_in_ms;

	Uint32 time_at_start_of_intervall_in_ms;

	dt_in_seconds dt;

	Uint64 timer_frequency;

	Uint64 timer_tick_at_start_of_frame;

}fps;

//----------------------------------------------------------------------- init

static inline void init_fps(){
	fps.average_during_last_intervall=0;
	fps.frame_count=0;
	fps.calculation_intervall_in_ms=1000;
	fps.time_at_start_of_intervall_in_ms=SDL_GetTicks();
	fps.timer_tick_at_start_of_frame=SDL_GetPerformanceCounter();
	fps.timer_frequency=SDL_GetPerformanceFrequency();
}

//----------------------------------------------------------------------- free

inline static void free_fps(){}

//----------------------------------------------------------------------------

inline static void at_frame_begin_call_fps() {fps.frame_count++;}

//----------------------------------------------------------------------------

static inline void at_frame_end_call_fps() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - fps.timer_tick_at_start_of_frame;
		fps.timer_tick_at_start_of_frame = t1;
		fps.dt=(float)dt/(float)fps.timer_frequency;

		if(fps.dt>.1)
			fps.dt=.1f;

		if(fps.dt==0)
			fps.dt=.00001f; //? min float?
	}

	Uint32 t1=SDL_GetTicks();
	Uint32 dt=t1-fps.time_at_start_of_intervall_in_ms;
	if(dt<fps.calculation_intervall_in_ms)
		return;
	if(dt!=0){
		fps.average_during_last_intervall=fps.frame_count*1000/dt;
	}else{
		fps.average_during_last_intervall=0;
	}
	fps.time_at_start_of_intervall_in_ms = t1;
	fps.frame_count = 0;

	printf(" %04d   %6.6f\n",
			fps.average_during_last_intervall,
			fps.dt);

}

//----------------------------------------------------------------------------
