#pragma once
#include"sdl.h"
#include"globs.h"
#include"programs.h"
#include"metrics.h"
//--------------------------------------------------------------------- shader
//----------------------------------------------------------------------------
static char*shader_vertex_source =
"#version 130                                                \n\
uniform mat4 umtx_mw;// model-to-world-matrix                        \n\
in vec3 apos;// vertices                              \n\
in vec4 argba;// colors                               \n\
in vec3 anorm;                               \n\
in vec2 atex;                               \n\
out vec4 vrgba;                                          \n\
out vec3 vnorm;                                          \n\
out vec2 vtex;                                          \n\
void main(){                                                 \n\
	gl_Position=umtx_mw*vec4(apos,1);                                        \n\
	vrgba=argba;                                             \n\
	vnorm=anorm;                                             \n\
	vtex=atex;                                             \n\
}\n";
static char*shader_fragment_source =
		"#version 130                              \n\
uniform sampler2D utex;                    \n\
in vec4 vrgba;                \n\
in vec3 vnorm;                \n\
in vec2 vtex;                \n\
out vec4 rgba;                \n\
void main(){                               \n\
	rgba=texture2D(utex,vtex)+vrgba*dot(vec4(1,0,0,1),vec4(vnorm,1));\n\
}\n";
#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_utex 1

struct shader{
	gid active_program_ix;
}shader={0};

inline static void shader_load(){
	_check_gl_error("enter shader_load");
	glGenBuffers(1, &glob_def.vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,glob_def.vbufid);
	glBufferData(GL_ARRAY_BUFFER,
			glob_def.vbufnbytes,
			glob_def.vbuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_data+=(unsigned)glob_def.vbufnbytes;

	glGenBuffers(1,&glob_def.ibufid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glob_def.ibufid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			glob_def.ibufnbytes,
			glob_def.ibuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_data+=(unsigned)glob_def.ibufnbytes;

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
	metrics.buffered_data+=(unsigned)(
			glob_def.texhi*glob_def.texwi*(signed)sizeof(GL_FLOAT));
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
	glBindTexture(GL_TEXTURE_2D,0);
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
	const float m[]={
			1,0,0,0,
			0,1,0,0,
			0,0,1,.5f,
			0,0,0,1,
	};
	shader_render_triangle_elements(
			glob_def.vbufid,glob_def.vbufn,
			glob_def.ibufid,(unsigned)glob_def.ibufn,
			glob_def.texbufid,m
	);
}

inline static void shader_render_triangle_array(
		GLuint vbufid,size_t vbufn,GLuint texid,const float*mtx_mw
){
	_shader_prepare_for_render(vbufid,texid,mtx_mw);
	glDrawArrays(GL_TRIANGLES,0,(int)vbufn);
	_shader_after_render();
}

inline static void shader_free() {
	//? free programs?
//	if(shader.program_id)
//		glDeleteProgram(shader.program_id);
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
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GREATER);
	glEnable(GL_CULL_FACE);
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s : %-7s :\n","feature","y/n");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s : %-7s :\n","cull face",glIsEnabled(GL_CULL_FACE)?"yes":"no");
	printf(": %10s : %-7s :\n","blend",glIsEnabled(GL_BLEND)?"yes":"no");
	printf(": %10s : %-7s :\n","zbuffer",glIsEnabled(GL_DEPTH_TEST)?"yes":"no");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	puts("");

	dyni attrs=dyni_def;
	int e[]={shader_apos,shader_argba,shader_anorm,shader_atex};
	dyni_add_list(&attrs,e,4);
//
//	dyni_add(&attrs,shader_apos);
//	dyni_add(&attrs,shader_argba);
//	dyni_add(&attrs,shader_anorm);
//	dyni_add(&attrs,shader_atex);

	programs_load(0,shader_vertex_source,shader_fragment_source,/*gives*/attrs);


	//	glUseProgram(shader_programs[0].id);

	shader_load();

	_check_gl_error("after shader_init");

	///----------------------------------------------


//	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//	GLuint FramebufferName = 0;
//	glGenFramebuffers(1, &FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//
//	// The texture we're going to render to
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, renderedTexture);
//
//	// Give an empty image to OpenGL ( the last "0" )
//	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
//
//	// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	// The depth buffer
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//	// Set "renderedTexture" as our colour attachement #0
//	_check_gl_error("in make texture for render  1");
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//	_check_gl_error("in make texture for render");
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


}


