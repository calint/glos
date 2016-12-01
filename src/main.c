#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"

//-------------------------------------------------------------------- include

#include "objects/logo_1.h"
#include "objects/logo_2.h"
#include "objects/logo_3.h"
#include "objects/logo_4.h"
#include "objects/ninja.h"


//------------------------------------------------------------------------ lib

//inline static void load_texture_bmp(int n,const char*path){
//	SDL_Surface*bmp=SDL_LoadBMP(path);
//	if (!bmp){
//		perror("could not load path:");
//		printf("%s %d: %s",__FILE__,__LINE__,path);
//		exit(4);
//	}
//	SDL_Texture*tex=SDL_CreateTextureFromSurface(window.renderer,bmp);
//	if(!tex){
//		perror("could not load path:");
//		printf("%s %d: %s",__FILE__,__LINE__,path);
//		exit(5);
//	}
//	SDL_FreeSurface(bmp);
//	texture[n]=tex;
//}


inline static void load_texture(int n,const char*path){

	SDL_Surface*loaded_surface=IMG_Load(path);
	if(!loaded_surface){
		perror("could not load texture");
		puts(path);
		exit(12);
	}
	SDL_Texture*tex=SDL_CreateTextureFromSurface(
			window.renderer,
			loaded_surface
		);
	if(!tex){
		perror("could not create texture");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		exit(13);
	}
	texture[n]=tex;
	SDL_FreeSurface(loaded_surface);

}

//----------------------------------------------------------------------- init

inline static void load_textures(){
	load_texture(0,"logo.bmp");
	load_texture(1,"sdl_logo.bmp");
	load_texture(10,"arts/ninja/Idle__000.png");
}

inline static void init(){

	load_textures();

	object_alloc(&default_logo_1);
	object_alloc(&default_logo_2);
	object_alloc(&default_logo_3);
	object_alloc(&default_logo_4);
	object_alloc(&default_ninja);

}

//-------------------------------------------------------------background_color

struct {
	GLclampf red;
	GLclampf green;
	GLclampf blue;
} background_color = {0,0,0};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	//-------------------------------------------------------------------- init

	sdl_init();

	window_init();

	textures_init();

	sprites_init();

	fps_init();

	shader_init();

	objects_init();

	init();

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

		sprites_update(fps.dt);

		sprites_render(window.renderer);

		objects_update(fps.dt);

		objects_render();

		SDL_GL_SwapWindow(window.ptr);

		fps_at_frame_done();

	}

	//---------------------------------------------------------------------free

	objects_free();

	shader_free();

	fps_free();

	sprites_free();

	textures_free();

	window_free();

	sdl_free();

	return 0;
}

//----------------------------------------------------------------------------
