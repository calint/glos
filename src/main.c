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

//------------------------------------------------------------------------ lib

inline static void load_texture(int n,const char*path){
	SDL_Surface*loaded_surface=IMG_Load(path);
	if(!loaded_surface){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(12);
	}
	SDL_Texture*tex=SDL_CreateTextureFromSurface(
			window.renderer,
			loaded_surface);
	if(!tex){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(13);
	}
	texture[n].ptr=tex;

	SDL_QueryTexture(tex,NULL,NULL,&texture[n].width,&texture[n].height);
	SDL_FreeSurface(loaded_surface);
}

//----------------------------------------------------------------------- init

inline static void init_main(){
	drawables_load(alloc(&default_ninja)->drawable_id=1,"arts/obj/sphere.obj");
}

//-------------------------------------------------------------background_color

struct{

	GLclampf red;

	GLclampf green;

	GLclampf blue;

}background_color={1,1,0};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	init_sdl();

	init_window();

	init_textures();

	init_fps();

	init_shader();

	init_objects();

	init_main();

	//-------------------------------------------------------------------- loop

	for(int running=1;running;){

		at_frame_begin_call_fps();

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

		update_objects(fps.dt);

		render_objects();


		SDL_GL_SwapWindow(window.ref);

		at_frame_end_call_fps();

	}

	//---------------------------------------------------------------------free

	//? early-hangup

	free_objects();

	free_shader();

	free_fps();

	free_textures();

	free_window();

	free_sdl();

	return 0;
}

//----------------------------------------------------------------------------
