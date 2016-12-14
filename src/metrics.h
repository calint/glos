#pragma once
#include"sdl.h"
//------------------------------------------------------------------------ fps

//--------------------------------------------------------------------- metrics
struct{
	struct{
		uint32_t average_during_last_intervall;
		uint32_t frame_count;
		uint32_t calculation_intervall_ms;
		uint32_t _time_at_start_of_intervall_in_ms;
		dt dt;
		uint64_t _timer_frequency;
		uint64_t _timer_tick_at_start_of_frame;
	}fps;

	unsigned tick;
	size_t buffered_vertex_data;
	unsigned buffered_texture_data;
	unsigned objects_allocated;
	unsigned glos_allocated;
	unsigned objects_updated_prv_frame;
	unsigned parts_updated_prv_frame;
	unsigned objects_rendered_prv_frame;
	unsigned parts_rendered_prv_frame;
	unsigned average_fps;
	unsigned triangles_rendered_prv_frame;
	float net_lag_prv_frame;
	unsigned collision_detections_prv_frame;
}metrics;

inline static void metrics_init(){}

inline static void metrics_free(){}

inline static void metrics_reset_timer(){
	metrics.fps._time_at_start_of_intervall_in_ms=SDL_GetTicks();
	metrics.fps._timer_tick_at_start_of_frame=SDL_GetPerformanceCounter();
	metrics.fps._timer_frequency=SDL_GetPerformanceFrequency();
}

inline static void metrics_print_headers(FILE*f){
	fprintf(f," %6s  %6s  %4s  %7s  %6s  %5s  %6s  %6s  %6s  %6s  %8s  %5s  %8s  %8s\n",
			"ms","dt","fps","netlag","nobj","cold","upd","rend","pupd","prend","gtri",
			"nglo","arrbufs","texbufs"
		);
}

inline static void metrics_print(FILE*f){
	fprintf(f," %06u  %0.4f  %04d  %0.5f  %06u  %05u  %06u  %06u  %06u  %06u  %08u"
			  "  %05u  %08lu  %08u\n",
			metrics.tick,
			metrics.fps.dt,
			metrics.average_fps,
			metrics.net_lag_prv_frame,
			metrics.objects_allocated,
			metrics.collision_detections_prv_frame,
			metrics.objects_updated_prv_frame,
			metrics.objects_rendered_prv_frame,
			metrics.parts_updated_prv_frame,
			metrics.parts_rendered_prv_frame,
			metrics.triangles_rendered_prv_frame,
			metrics.glos_allocated,
			metrics.buffered_vertex_data,
			metrics.buffered_texture_data
		);
}

//----------------------------------------------------------------------------

inline static void metrics__at__frame_begin(){
	if(!metrics.fps.frame_count)
		metrics_reset_timer();
	metrics.fps.frame_count++;
	metrics.objects_rendered_prv_frame=0;
	metrics.objects_updated_prv_frame=0;
	metrics.parts_rendered_prv_frame=0;
	metrics.parts_updated_prv_frame=0;
	metrics.triangles_rendered_prv_frame=0;
	metrics.tick=SDL_GetTicks();
	metrics.net_lag_prv_frame=0;
	metrics.collision_detections_prv_frame=0;
}

//----------------------------------------------------------------------------

static inline void metrics__at__frame_end(FILE*f) {
	{
		Uint64 t1=SDL_GetPerformanceCounter();
		Uint64 dt_ticks=t1-metrics.fps._timer_tick_at_start_of_frame;
		metrics.fps._timer_tick_at_start_of_frame=t1;
		metrics.fps.dt=(float)dt_ticks/(float)metrics.fps._timer_frequency;

		if(metrics.fps.dt>.1)
			metrics.fps.dt=.1f;//? magicnumber

		if(metrics.fps.dt==0)
			metrics.fps.dt=.00001f;//? magicnumber

	}

	Uint32 t1=SDL_GetTicks();
	Uint32 dt=t1-metrics.fps._time_at_start_of_intervall_in_ms;

	if(dt<metrics.fps.calculation_intervall_ms)
		return;

	if(dt!=0){
		metrics.fps.average_during_last_intervall=
				metrics.fps.frame_count*1000/dt;
	}else{
		metrics.fps.average_during_last_intervall=0;
	}

	metrics.average_fps=metrics.fps.average_during_last_intervall;
	metrics.fps._time_at_start_of_intervall_in_ms=t1;
	metrics.fps.frame_count=0;

	metrics_print(f);
}

//----------------------------------------------------------------------------
