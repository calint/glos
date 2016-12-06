#include"sdl.h"
#include"lib.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"obj_file.h"
#include"object_alloc.h"
#include"fps.h"
#include"app/ninja.h"
//----------------------------------------------------------------------- init
inline static void main_init(){
//	drawables_load_file_in_slot(6,"obj/plane.obj");
//	drawables_load_file_in_slot(6,"obj/ico_sphere.obj");
//	drawables_load_file_in_slot(6,"obj/sphere.obj");
//	drawables_load_file_in_slot(6,"obj/cylinder.obj");


//	drawables_load_file_in_slot(6,"obj/blender_monkey.obj");
//	drawables_load_file_in_slot(6,"obj/cube.obj");
//	drawables_load_file_in_slot(6,"obj/grid_8x8.obj");

//	glob*g=glob_alloc(&glob_def);
//	glob_load_obj_file(g,"obj/ico_sphere.obj");
//
	globs_load_obj_file(1,"obj/ico_sphere.obj");


	//	object_at(object_count-1);
	//	object_at(object_count);
	//		object_at_const(object_count-1);
	//		object_at_const(object_count);

	for(int i=0;i<object_cap;i++){
		object*o=object_alloc(&ninja_def);
//		object_free(o);
//		object_free(o);
//		object_at(object_cap-1);
//		object_at(object_cap);

		o->glob_id=1;

		o->velocity=(velocity){
			random_range(-1,1),random_range(-1,1),0,0};

		o->angular_velocity=(angular_velocity){
			0,0,random_range(-360*8,360*8),0};

		o->scale=(scale){
			random_range(-.05f,.05f),
			random_range(-.05f,.05f),
			random_range(-.05f,.05f),1}
		;

	}
//
//	object*o=new(&_ninja_);
//	o->position.x=-.5f;
//	o->scale=(scale){.2f,.2f,.2f,1};
//
//	o=new(&_ninja_);
//	o->position.y=.5f;
//	o->scale=(scale){.1f,.1f,.1f,1};
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
	main_init();
	globs_init();
	fps_init();

	struct{
		GLclampf red;
		GLclampf green;
		GLclampf blue;
	}c={.11f,.37f,1};

	int draw_default=0,draw_objects=1;

	for(int running=1;running;){
		fps__at__frame_begin();

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
		objects_update(fps.dt);
		if(draw_objects)objects_render();
		if(draw_default)shader_render();
//
//		globs_render();

		SDL_GL_SwapWindow(window.ref);
		fps__at__update_frame_end();
	}
	//---------------------------------------------------------------------free
	//? early-hangup
	fps_free();
	globs_free();
	objects_free();
	shader_free();
	window_free();
	sdl_free();
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
