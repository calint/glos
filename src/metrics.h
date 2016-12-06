#pragma once

struct{
	size_t buffered_data;
	unsigned objects_allocated;
	unsigned objects_updated_last_frame;
	unsigned parts_updated_last_frame;
	unsigned objects_rendered_last_frame;
	unsigned parts_rendered_last_frame;
	unsigned average_fps;
	unsigned vertices_rendered;
	dt previous_frame_dt;
}metrics;

inline static void metrics_print_headers(){
	printf(" %4s   %6s   %8s  %6s  %6s  %6s  %6s  %6s\n",
			"fps","dt","bufs","nobjs","obup","obre","ptup","ptre"
		);
}

inline static void metrics_print(){
	printf(" %04d   %0.4f   %08lu  %06u  %06u  %06u  %06u  %06u\n",
			metrics.average_fps,
			metrics.previous_frame_dt,
			metrics.buffered_data,
			metrics.objects_allocated,
			metrics.objects_updated_last_frame,
			metrics.objects_rendered_last_frame,
			metrics.parts_updated_last_frame,
			metrics.parts_rendered_last_frame
		);
}

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

//----------------------------------------------------------------------------

inline static void metrics__at__frame_begin(){
	if(!fps.frame_count){
		fps.calculation_intervall_in_ms=1000;
		fps._time_at_start_of_intervall_in_ms=SDL_GetTicks();
		fps._timer_tick_at_start_of_frame=SDL_GetPerformanceCounter();
		fps._timer_frequency=SDL_GetPerformanceFrequency();
	}
	fps.frame_count++;
	metrics.objects_rendered_last_frame=0;
	metrics.objects_updated_last_frame=0;
	metrics.parts_rendered_last_frame=0;
	metrics.parts_updated_last_frame=0;
	metrics.vertices_rendered=0;
}

//----------------------------------------------------------------------------

static inline void metrics__at__update_frame_end() {
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
