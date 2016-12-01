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
#include "objects/santa.h"


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
	texture[n].ptr=tex;

	SDL_QueryTexture(tex,NULL,NULL,&texture[n].width, &texture[n].height);

	SDL_FreeSurface(loaded_surface);

}

//----------------------------------------------------------------------- init

inline static void load_textures(){
	load_texture(0,"logo.bmp");
	load_texture(1,"sdl_logo.bmp");
	load_texture(10,"arts/ninja/Idle__000.png");
	load_texture(11,"arts/ninja/Idle__001.png");
	load_texture(12,"arts/ninja/Idle__002.png");
	load_texture(13,"arts/ninja/Idle__003.png");
	load_texture(14,"arts/ninja/Idle__004.png");
	load_texture(15,"arts/ninja/Idle__005.png");
	load_texture(16,"arts/ninja/Idle__006.png");
	load_texture(17,"arts/ninja/Idle__007.png");
	load_texture(18,"arts/ninja/Idle__008.png");
	load_texture(19,"arts/ninja/Idle__009.png");

	load_texture(20,"arts/santa/Idle (1).png");
	load_texture(21,"arts/santa/Idle (2).png");
	load_texture(22,"arts/santa/Idle (3).png");
	load_texture(23,"arts/santa/Idle (4).png");
	load_texture(24,"arts/santa/Idle (5).png");
	load_texture(25,"arts/santa/Idle (6).png");
	load_texture(26,"arts/santa/Idle (7).png");
	load_texture(27,"arts/santa/Idle (8).png");
	load_texture(28,"arts/santa/Idle (9).png");
	load_texture(30,"arts/santa/Idle (10).png");
	load_texture(31,"arts/santa/Idle (11).png");
	load_texture(32,"arts/santa/Idle (12).png");
	load_texture(33,"arts/santa/Idle (13).png");
	load_texture(34,"arts/santa/Idle (14).png");
	load_texture(35,"arts/santa/Idle (15).png");
	load_texture(36,"arts/santa/Idle (16).png");
}

inline static void init(){

	load_textures();

//	object_alloc(&default_logo_1);
//	object_alloc(&default_logo_2);
//	object_alloc(&default_logo_3);
//	object_alloc(&default_logo_4);
	object_alloc(&default_ninja);
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

	sprites_free();

	textures_free();

	window_free();

	sdl_free();

	return 0;
}

//----------------------------------------------------------------------------
