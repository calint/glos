#pragma once
#include<SDL.h>
#include<SDL_opengles2.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<unistd.h>
//#define GLOS_EMBEDDED

//------------------------------------------------------------------------ sdl

static struct sdl{

	TTF_Font*font;

	Mix_Music*music;

}sdl;

//----------------------------------------------------------------------- lib

inline static void play_mp3(const char*path){
	Mix_OpenAudio(22050,AUDIO_S16SYS,2,640);
	sdl.music=Mix_LoadMUS(path);
//	Mix_PlayMusic(sdl.music,-1); // loop
	Mix_PlayMusic(sdl.music,1); // play once
//	while(!SDL_QuitRequested()){
//		SDL_Delay(250);
//	}
//	Mix_FreeMusic(music);
}

//----------------------------------------------------------------------- init

static inline void sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)) {
		printf("%s %d: %s\n",__FILE__,__LINE__,IMG_GetError());
		exit(1);
	}

//	if(MIX_INIT_MP3!=Mix_Init(MIX_INIT_MP3)) {
//		printf("%s %d: %s\n",__FILE__,__LINE__,IMG_GetError());
//		printf("Mix_Init: %s\n",Mix_GetError());
//		exit(20);
//	}
//
//	play_mp3("arts/mp3/Commercial DEMO - 02.mp3");

	if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)){
		printf("%s %d: %s\n",__FILE__,__LINE__,IMG_GetError());
		exit(16);
	}

//	if(TTF_Init()<0){
//		printf("%s %d: %s\n",__FILE__,__LINE__,IMG_GetError());
//		exit(17);
//	}

//	sdl.font=TTF_OpenFont("arts/ttf/amadeus.ttf", 25);
//	if(!sdl.font){
//		puts("tried to load font");
//		printf("%s %d: %s\n",__FILE__,__LINE__,IMG_GetError());
//		exit(18);
//	}
}

//------------------------------------------------------------------------free

static inline void sdl_free(){
	Mix_FreeMusic(sdl.music);
	TTF_CloseFont(sdl.font);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//----------------------------------------------------------------------------

