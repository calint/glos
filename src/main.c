#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"

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

inline static void main_init(){

	load_texture(1,"arts/wintertileset/png/BG/BG.png");
	object_alloc(&default_background);

	for(int i=0;i<10;i++){
		char str[256];
		sprintf(str,"arts/ninja/Idle__%03d.png",i);
		load_texture(10+i,str);
	}
	object_alloc(&default_ninja);

	for(int i=1;i<17;i++){
		char str[256];
		sprintf(str,"arts/santa/Idle (%d).png",i);
		load_texture(19+i,str);
	}
	object_alloc(&default_santa);

}

//-------------------------------------------------------------background_color

struct {
	GLclampf red;
	GLclampf green;
	GLclampf blue;
} background_color={1,1,0};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	//-------------------------------------------------------------------- init

	sdl_init();

	window_init();

	textures_init();

//	sprites_init();

	fps_init();

	shader_init();

	objects_init();

	main_init();

	//-------------------------------------------------------------------- loop

	for(int running = 1; running; ) {

		fps_at_frame_start();

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
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

//		sprites_update(fps.dt);

//		sprites_render(window.renderer);

		objects_update(fps.dt);

		objects_render();

		SDL_GL_SwapWindow(window.ptr);

		fps_at_frame_done();

	}

	//---------------------------------------------------------------------free

	objects_free();

	shader_free();

	fps_free();

//	sprites_free();

	textures_free();

	window_free();

	sdl_free();

	return 0;
}

//----------------------------------------------------------------------------
