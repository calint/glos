#pragma once
#include"lib.h"
#include"metrics.h"

//------------------------------------------------------------------------ fps

struct{
	uint32_t average_during_last_intervall;
	uint32_t frame_count;
	uint32_t calculation_intervall_in_ms;
	uint32_t _time_at_start_of_intervall_in_ms;
	dt dt;
	uint64_t _timer_frequency;
	uint64_t _timer_tick_at_start_of_frame;
}fps;

//----------------------------------------------------------------------- init

static inline void fps_init(){
	fps.average_during_last_intervall=0;
	fps.frame_count=0;
	fps.calculation_intervall_in_ms=1000;
	fps._time_at_start_of_intervall_in_ms=SDL_GetTicks();
	fps._timer_tick_at_start_of_frame=SDL_GetPerformanceCounter();
	fps._timer_frequency=SDL_GetPerformanceFrequency();
}

//----------------------------------------------------------------------- free

inline static void fps_free(){}

//-------------------------__timer_tick_at_start_of_frame---------------------------------------------------

inline static void fps__at__frame_begin() {
	fps.frame_count++;
	metrics.objects_rendered_last_frame=0;
	metrics.objects_updated_last_frame=0;
	metrics.parts_rendered_last_frame=0;
	metrics.parts_updated_last_frame=0;
}

//----------------------------------------------------------------------------

static inline void fps__at__update_frame_end() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - fps._timer_tick_at_start_of_frame;
		fps._timer_tick_at_start_of_frame = t1;
		fps.dt=(float)dt/(float)fps._timer_frequency;
		metrics.previous_frame_dt=fps.dt;

		if(fps.dt>.1)
			fps.dt=.1f;

		if(fps.dt==0)
			fps.dt=.00001f;

	}

	Uint32 t1=SDL_GetTicks();
	Uint32 dt=t1-fps._time_at_start_of_intervall_in_ms;
	if(dt<fps.calculation_intervall_in_ms)
		return;
	if(dt!=0){
		fps.average_during_last_intervall=fps.frame_count*1000/dt;
	}else{
		fps.average_during_last_intervall=0;
	}
	metrics.average_fps=fps.average_during_last_intervall;
	fps._time_at_start_of_intervall_in_ms = t1;
	fps.frame_count = 0;

	metrics_print(stdout);
}

//----------------------------------------------------------------------------
