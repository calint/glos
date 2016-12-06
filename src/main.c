#include"sdl.h"
#include"lib.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"obj_file.h"
#include"object_alloc.h"
#include"glob.h"
#include"metrics.h"
#include"app/ninja.h"
//----------------------------------------------------------------------- init
inline static void main_init(){
	globs_load_obj_file(1,"obj/plane.obj");
	globs_load_obj_file(2,"obj/ico_sphere.obj");
	globs_load_obj_file(3,"obj/cylinder.obj");
	globs_load_obj_file(4,"obj/grid_8x8.obj");
	globs_load_obj_file(5,"obj/sphere.obj");
	globs_load_obj_file(6,"obj/torus.obj");

	object*o=object_alloc(&ninja_def);
	o->glob_id=6;
	const float vr=.5f;
	o->velocity=(velocity){
		random_range(-vr,vr),random_range(-vr,vr),0,0};

	const float ar=360;
	o->angular_velocity=(angular_velocity){
		0,0,random_range(-ar,ar),0};

//	const float sr=.2f;
//	o->scale=(scale){
//		random_range(-sr,sr),
//		random_range(-sr,sr),
//		random_range(-sr,sr),1};
	o->scale=(scale){.5f,.5f,.5f,1};
}
inline static void main_init2(){
	globs_load_obj_file(1,"obj/ico_sphere.obj");
	globs_load_obj_file(2,"obj/cylinder.obj");
	globs_load_obj_file(3,"obj/grid_8x8.obj");
	globs_load_obj_file(4,"obj/plane.obj");
	globs_load_obj_file(5,"obj/sphere.obj");

	for(int i=0;i<object_cap;i++){
		object*o=object_alloc(&ninja_def);

		o->glob_id=(gid)random_range(1,6);
		if(o->glob_id==6)o->glob_id=5;

		const float vr=.5f;
		o->velocity=(velocity){
			random_range(-vr,vr),random_range(-vr,vr),0,0};

		const float ar=360;
		o->angular_velocity=(angular_velocity){
			0,0,random_range(-ar,ar),0};

		const float sr=.02f;
		o->scale=(scale){
			random_range(-sr,sr),
			random_range(-sr,sr),
			random_range(-sr,sr),1};
	}
}
//------------------------------------------------------------------------ main
int main(int argc,char*argv[]){
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9s :\n","type",           "bytes");
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9ld :\n","part",sizeof(part));
	printf(": %15s : %-9ld :\n","object",sizeof(object));
	printf(": %15s : %-9ld :\n","objects",sizeof(objects));
	printf(": %15s : %-9ld :\n","glob",sizeof(glob));
	printf(": %15s : %-9ld :\n","globs",sizeof(globs));
	printf(":-%15s-:-%-9s-:\n","---------------","--------");

	sdl_init();
	window_init();
	shader_init();
	objects_init();
	globs_init();
//	fps_init();

	main_init();
	struct{
		GLclampf red;
		GLclampf green;
		GLclampf blue;
	}c={.11f,.37f,1};

	int draw_default=0,draw_objects=1;


	puts("");
	metrics_print_headers();

	for(int running=1;running;){
		metrics__at__frame_begin();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						running = 0;
						break;
					case SDLK_w:
						objects[0].velocity.y=.5f;
						c.red = 1;
						c.green = 0;
						c.blue = 0;
						break;
					case SDLK_a:
						objects[0].angular_velocity.z=90;
						c.red = 0;
						c.green = 1;
						c.blue = 0;
						break;
					case SDLK_s:
						objects[0].velocity.y=-.5f;
						c.red = 0;
						c.green = 0;
						c.blue = 1;
						break;
					case SDLK_d:
						objects[0].angular_velocity.z=-90;
						c.red = 1;
						c.green = 1;
						break;
					case SDLK_1:
						draw_default=1;
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_w:
						objects[0].velocity.y=0;
						break;
					case SDLK_a:
						objects[0].angular_velocity.z=0;
						break;
					case SDLK_s:
						objects[0].velocity.y=-0;
						break;
					case SDLK_d:
						objects[0].angular_velocity.z=0;
						break;
					case SDLK_1:
						draw_default=0;
						break;
					case SDLK_2:{
						object*o=object_at(0);
						o->glob_id++;
						if(o->glob_id>6){
							o->glob_id=1;
						}
						break;
					}
				}
				break;


			}
		}

		glClearColor(c.red,c.green,c.blue,1.0);

		glClear(GL_COLOR_BUFFER_BIT);

		objects_update(metrics.previous_frame_dt);

		if(draw_objects)objects_render();

		if(draw_default)shader_render();

		SDL_GL_SwapWindow(window.ref);

		metrics__at__update_frame_end();
	}
	//---------------------------------------------------------------------free
	//? early-hangup
//	fps_free();
	globs_free();
	objects_free();
	shader_free();
	window_free();
	sdl_free();
	metrics_print();
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
