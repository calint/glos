#include"sdl.h"
#include"lib.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"obj_file.h"
#include"object_alloc.h"
#include"metrics.h"
#include"app/ninja.h"
//----------------------------------------------------------------------- init
inline static void main_init(){
	globs_load_obj_file(1,"obj/ico_sphere.obj");

//	for(int i=0;i<object_cap;i++){
	for(int i=0;i<1;i++){

		object*o=object_alloc(&ninja_def);

		o->glob_id=1;

		o->velocity=(velocity){
			random_range(-1,1),random_range(-1,1),0,0};

		o->angular_velocity=(angular_velocity){
			0,0,random_range(-360*8,360*8),0};

		o->scale=(scale){
			random_range(-.05f,.05f),
			random_range(-.05f,.05f),
			random_range(-.05f,.05f),1};
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
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
