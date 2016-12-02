#include "sdl.h"
#include "window.h"
#include "shader.h"
#include "objects.h"
#include "textures.h"
#include "lib.h"
#include "fps.h"

//-------------------------------------------------------------------- include

#include "objects/ninja.h"

#include "objects/santa.h"

#include "objects/background.h"

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

	load_texture(1,"arts/wintertileset/png/BG/BG.png");
	alloc(&background);

	for(int i=0;i<10;i++){
		char str[256];
		sprintf(str,"arts/ninja/Idle__%03d.png",i);
		load_texture(10+i,str);
	}
	alloc(&ninja);

	for(int i=1;i<17;i++){
		char str[256];
		sprintf(str,"arts/santa/Idle (%d).png",i);
		load_texture(19+i,str);
	}
	alloc(&santa);

}

//-------------------------------------------------------------background_color

struct{

	GLclampf red;

	GLclampf green;

	GLclampf blue;

}background_color={1,1,0};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	//-------------------------------------------------------------------- init

	init_sdl();

	init_window();

	init_textures();

//	sprites_init();

	init_fps();

	init_shader();

	init_objects();

	init_main();

	//-------------------------------------------------------------------- loop

	for(int running=1;running;){

		at_fram_start_call_fps();

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
					background_color.red = 0;
					background_color.green = 1;
					background_color.blue = 0;
					break;
				case SDLK_s:
					background_color.red = 0;
					background_color.green = 0;
					background_color.blue = 1;
					break;
				case SDLK_d:
					background_color.red = 1;
					background_color.green = 1;
					break;
				}
			}
		}

		//---------------------------------------------------------------- draw

		glClearColor( background_color.red, background_color.green,
				background_color.blue, 1.0);

		glClear(GL_COLOR_BUFFER_BIT);

//		shader_render();

		update_objects(fps.dt);

		render_objects();

		SDL_GL_SwapWindow(window.ptr);

		at_frame_done_call_fps();

	}

	//---------------------------------------------------------------------free

	free_objects();

	shader_free();

	fps_free();

	free_textures();

	window_free();

	sdl_free();

	return 0;
}

//----------------------------------------------------------------------------
