#include "formats/obj/obj_file.h"
#include "sdl.h"
#include "window.h"
#include "shader.h"
#include "objects.h"
#include "textures.h"
#include "lib.h"
#include "fps.h"
#include "drawables.h"

#include "app/ninja.h"

//----------------------------------------------------------------------- init

inline static void main_init(){
	load_drawable(1,"arts/obj/sphere.obj");


	alloc(&default_ninja);

	object*o=alloc(&default_ninja);
	o->position.x=-.5f;
	o->scale=(scale){.2f,.2f,.2f,1};
}

//-------------------------------------------------------------background_color

struct{
	GLclampf red;
	GLclampf green;
	GLclampf blue;
}background_color={0,0,1};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	sdl_init();

	window_init();

	textures_init();

	fps_init();

	shader_init();

	objects_init();

	drawables_init();

	main_init();

	printf(": %10s  : %4s :\n","type","size");
	printf(": %10s  : %4ld :\n","object",sizeof(object));
	//-------------------------------------------------------------------- loop

	for(int running=1;running;){

		fps__at__frame_begin();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			//-------------------------------------------------------------keys
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				case SDLK_w:
					background_color.red = 1;
					background_color.green = 0;
					background_color.blue = 0;
					break;
				case SDLK_a:
					objects[0].angular_velocity.z=90;
					background_color.red = 0;
					background_color.green = 1;
					background_color.blue = 0;
					break;
				case SDLK_s:
					objects[0].angular_velocity.z=0;
					background_color.red = 0;
					background_color.green = 0;
					background_color.blue = 1;
					break;
				case SDLK_d:
					objects[0].angular_velocity.z=-90;
					background_color.red = 1;
					background_color.green = 1;
					break;
				}
			}
			//-------------------------------------------------------------keys
		}

		//---------------------------------------------------------------- draw

		glClearColor(
			background_color.red,
			background_color.green,
			background_color.blue, 1.0
		);

		glClear(GL_COLOR_BUFFER_BIT);

//		shader_render();

		objects_update(fps.dt);

		objects_render();


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
