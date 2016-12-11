#pragma once
#include"sdl.h"
//------------------------------------------------------------------------ fps

//--------------------------------------------------------------------- metrics
struct{
	struct{
		uint32_t average_during_last_intervall;
		uint32_t frame_count;
		uint32_t calculation_intervall_in_ms;
		uint32_t _time_at_start_of_intervall_in_ms;
		dt dt;
		uint64_t _timer_frequency;
		uint64_t _timer_tick_at_start_of_frame;
	}fps;
	size_t buffered_vertex_data;
	size_t buffered_texture_data;
	unsigned objects_allocated;
	unsigned objects_updated_prv_frame;
	unsigned parts_updated_prv_frame;
	unsigned objects_rendered_prv_frame;
	unsigned parts_rendered_prv_frame;
	unsigned average_fps;
	unsigned vertices_rendered;
	dt dt_prv_frame;
}metrics;

inline static void metrics_reset(){
	metrics.fps.calculation_intervall_in_ms=1000;
	metrics.fps._time_at_start_of_intervall_in_ms=SDL_GetTicks();
	metrics.fps._timer_tick_at_start_of_frame=SDL_GetPerformanceCounter();
	metrics.fps._timer_frequency=SDL_GetPerformanceFrequency();
}

inline static void metrics_print_headers(FILE*f){
	fprintf(f," %4s   %6s   %8s  %8s  %6s  %6s  %6s  %6s  %6s\n",
			"fps","dt","bufsarr","bufstex","nobj","upd","rend","pupd","prend"
		);
}

inline static void metrics_print(FILE*f){
	fprintf(f," %04d   %0.4f   %08lu  %08lu  %06u  %06u  %06u  %06u  %06u\n",
			metrics.average_fps,
			metrics.dt_prv_frame,
			metrics.buffered_vertex_data,
			metrics.buffered_texture_data,
			metrics.objects_allocated,
			metrics.objects_updated_prv_frame,
			metrics.objects_rendered_prv_frame,
			metrics.parts_updated_prv_frame,
			metrics.parts_rendered_prv_frame
		);
}

//----------------------------------------------------------------------------

inline static void metrics__at__frame_begin(){
	if(!metrics.fps.frame_count)
		metrics_reset();
	metrics.fps.frame_count++;
	metrics.objects_rendered_prv_frame=0;
	metrics.objects_updated_prv_frame=0;
	metrics.parts_rendered_prv_frame=0;
	metrics.parts_updated_prv_frame=0;
	metrics.vertices_rendered=0;
}

//----------------------------------------------------------------------------

static inline void metrics__at__update_frame_end() {
	{
		Uint64 t1 = SDL_GetPerformanceCounter();
		Uint64 dt = t1 - metrics.fps._timer_tick_at_start_of_frame;
		metrics.fps._timer_tick_at_start_of_frame = t1;
		metrics.fps.dt=(float)dt/(float)metrics.fps._timer_frequency;
		metrics.dt_prv_frame=metrics.fps.dt;

		if(metrics.fps.dt>.1)
			metrics.fps.dt=.1f;

		if(metrics.fps.dt==0)
			metrics.fps.dt=.00001f;

	}

	Uint32 t1=SDL_GetTicks();
	Uint32 dt=t1-metrics.fps._time_at_start_of_intervall_in_ms;
	if(dt<metrics.fps.calculation_intervall_in_ms)
		return;
	if(dt!=0){
		metrics.fps.average_during_last_intervall=
				metrics.fps.frame_count*1000/dt;
	}else{
		metrics.fps.average_during_last_intervall=0;
	}
	metrics.average_fps=metrics.fps.average_during_last_intervall;
	metrics.fps._time_at_start_of_intervall_in_ms = t1;
	metrics.fps.frame_count = 0;

	metrics_print(stdout);
}

//----------------------------------------------------------------------------
