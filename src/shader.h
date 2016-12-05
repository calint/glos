#pragma once
#include"sdl.h"
#include"globs.h"
#include"programs.h"
//--------------------------------------------------------------------- shader
//----------------------------------------------------------------------------
static char*shader_vertex_source =
"#version 130                                                \n\
uniform mat4 umtx_mw;// model-to-world-matrix                        \n\
attribute vec3 apos;// vertices                              \n\
attribute vec4 argba;// colors                               \n\
attribute vec3 anorm;                               \n\
attribute vec2 atex;                               \n\
varying vec4 vrgba;                                          \n\
varying vec3 vnorm;                                          \n\
varying vec2 vtex;                                          \n\
void main(){                                                 \n\
	gl_Position=umtx_mw*vec4(apos,1);                                        \n\
	vrgba=argba;                                             \n\
	vnorm=anorm;                                             \n\
	vtex=atex;                                             \n\
}\n";
static char*shader_fragment_source =
		"#version 130                              \n\
uniform sampler2D utex;                    \n\
varying mediump vec4 vrgba;                \n\
varying mediump vec3 vnorm;                \n\
varying mediump vec2 vtex;                \n\
void main(){                               \n\
	gl_FragColor=texture2D(utex,vtex)+vrgba*dot(vec4(1,0,0,1),vec4(vnorm,1));\n\
}\n";
#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_utex 1

inline static void shader_load(){
	_check_gl_error("enter shader_load");
	glGenBuffers(1, &glob_def.vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,glob_def.vbufid);
	glBufferData(GL_ARRAY_BUFFER,
			glob_def.vbufnbytes,
			glob_def.vbuf,
			GL_STATIC_DRAW
	);

	glGenBuffers(1,&glob_def.ibufid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glob_def.ibufid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			glob_def.ibufnbytes,
			glob_def.ibuf,
			GL_STATIC_DRAW
	);

//	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&glob_def.texbufid);
	glBindTexture(GL_TEXTURE_2D,glob_def.texbufid);

//----------------------------------------------
//	SDL_Surface*surface=IMG_Load("logo.jpg");
//	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
//			surface->w,surface->h,
//			0,GL_RGB,GL_UNSIGNED_BYTE,
//			surface->pixels);
//	SDL_FreeSurface(surface);
//	//----------------------------------------------

	//----------------------------------------------
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
			glob_def.texwi,glob_def.texhi,
			0,GL_RGB,GL_FLOAT,
			glob_def.texbuf);
	//----------------------------------------------

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	_check_gl_error("exit shader_load");
}


inline static void _shader_prepare_for_render(
		GLuint vbufid,GLuint texid,const float*mtx_mw
){
	//? ifprevdiff
	glEnableVertexAttribArray(shader_apos);//position
	glEnableVertexAttribArray(shader_argba);//color
	glEnableVertexAttribArray(shader_anorm);//normal
	glEnableVertexAttribArray(shader_atex);//texture
	glUseProgram(programs[0].id);
	//? enabled

	glUniformMatrix4fv(0,1,0,mtx_mw);
	glUniform1i(shader_utex,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texid);

	glBindBuffer(GL_ARRAY_BUFFER,vbufid);
	glVertexAttribPointer(shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);
	glVertexAttribPointer(shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4+3)*sizeof(float)));
}

inline static void _shader_after_render(){
	//? ifnextnotsametype
	glBindTexture(GL_TEXTURE_2D,0);
	glDisableVertexAttribArray(shader_apos);//position
	glDisableVertexAttribArray(shader_argba);//color
	glDisableVertexAttribArray(shader_anorm);//normal
	glDisableVertexAttribArray(shader_atex);//texture
}

inline static void shader_render_triangle_elements(
		GLuint vbufid,size_t vbufn,
		GLuint ixbufid,size_t ixbufn,
		GLuint texid,const float*mtx_mw
){
	_shader_prepare_for_render(
			glob_def.vbufid,
			glob_def.texbufid,
			mat4_ident
		);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ixbufid);
	glDrawElements(GL_TRIANGLES,(signed)ixbufn,GL_UNSIGNED_BYTE,0);
	_shader_after_render();
}

inline static void shader_render(){
	shader_render_triangle_elements(
			glob_def.vbufid,glob_def.vbufn,
			glob_def.ibufid,(unsigned)glob_def.ibufn,
			glob_def.texbufid,mat4_ident
	);
}

inline static void shader_render_triangle_array(
		GLuint vbufid,size_t vbufn,GLuint texid,float*mtx_mw
){
	_shader_prepare_for_render(vbufid,texid,mtx_mw);
	glDrawArrays(GL_TRIANGLES,0,(int)vbufn);
	_shader_after_render();
}

inline static void shader_init() {
	_check_gl_error("shader_init");

//	gl_print_context_profile_and_version();


	puts("");
	_print_gl_string("GL_VERSION", GL_VERSION);
	_print_gl_string("GL_VENDOR", GL_VENDOR);
	_print_gl_string("GL_RENDERER", GL_RENDERER);
	_print_gl_string("GL_SHADING_LANGUAGE_VERSION",GL_SHADING_LANGUAGE_VERSION);
	puts("");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s :-%7s-:\n","feature","");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s : %-7s :\n","cull face",glIsEnabled(GL_CULL_FACE)?"yes":"no");
	printf(": %10s : %-7s :\n","blend",glIsEnabled(GL_BLEND)?"yes":"no");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	puts("");

	puts("");

	programs_load(0,shader_vertex_source,shader_fragment_source);
//	glUseProgram(shader_programs[0].id);

	shader_load();

	_check_gl_error("after shader_init");
}

inline static void shader_free() {
	//? free programs?
//	if(shader.program_id)
//		glDeleteProgram(shader.program_id);
}

