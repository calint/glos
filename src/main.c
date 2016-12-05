#include"sdl.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"textures.h"
#include"lib.h"
#include"fps.h"
#include"drawables.h"
#include"app/ninja.h"
#include"alloc.h"
#include"obj_file.h"
#include"dync.h"
#include"dynp.h"
#include"dyni.h"
//----------------------------------------------------------------------- init
inline static void main_init(){
//	drawables_load_file_in_slot(6,"obj/plane.obj");
	drawables_load_file_in_slot(6,"obj/ico_sphere.obj");
//	drawables_load_file_in_slot(6,"obj/sphere.obj");
//	drawables_load_file_in_slot(6,"obj/cylinder.obj");


//	drawables_load_file_in_slot(6,"obj/blender_monkey.obj");
//	drawables_load_file_in_slot(6,"obj/cube.obj");
//	drawables_load_file_in_slot(6,"obj/grid_8x8.obj");

	object*o=alloc(&ninja_def);
	o->velocity=(vec4){0,0,0,0};
	o->angular_velocity=(vec4){0,0,0,0};
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
	printf(":-%14s : %8s-:\n","--------------","--------");
	printf(": %14s : %8s :\n","type","size");
	printf(":-%14s : %8s-:\n","--------------","--------");
	printf(": %14s : %8ld :\n","part",sizeof(part));
	printf(": %14s : %8ld :\n","object",sizeof(object));
	printf(": %14s : %8ld :\n","objects",sizeof(objects));
	printf(": %14s : %8ld :\n","drawables",sizeof(drawables));
	printf(": %14s : %8ld :\n","textures",sizeof(texture));
	printf(":-%14s-:-%8s-:\n","--------------","--------");

	sdl_init();
	window_init();

//	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
//
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_BLEND); // transparency rgba
//
//	puts("");
//	printf(":-%10s-:-%7s-:\n","----------","-------");
//	printf(": %10s :-%7s-:\n","feature","");
//	printf(":-%10s-:-%7s-:\n","----------","-------");
//	printf(": %10s : %-7s :\n","cull face",glIsEnabled(GL_CULL_FACE)?"yes":"no");
//	printf(": %10s : %-7s :\n","blend",glIsEnabled(GL_BLEND)?"yes":"no");
//	printf(":-%10s-:-%7s-:\n","----------","-------");
//	puts("");
//

	textures_init();
	fps_init();
	shader_init();
	objects_init();
	drawables_init();
	main_init();

	struct{
		GLclampf red;
		GLclampf green;
		GLclampf blue;
	}c={0,0,1};

	int draw_default=0;
	int draw_objects=1;

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
						objects[0].angular_velocity.z=0;
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
					case SDLK_1:
						draw_default=0;
						break;
				}
			}


		}

		glClearColor(c.red,c.green,c.blue,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		objects_update(fps.dt);
		if(draw_objects)objects_render();
		if(draw_default)shader_render();
		SDL_GL_SwapWindow(window.ref);
		fps__at__update_frame_end();
	}
	//---------------------------------------------------------------------free
	//? early-hangup
	drawables_free();
	objects_free();
	shader_free();
	fps_free();
	textures_free();
	window_free();
	sdl_free();
	return 0;
}
//----------------------------------------------------------------------------
