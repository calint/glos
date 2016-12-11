#pragma once
#include"../sdl.h"
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
