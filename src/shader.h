#pragma once
#include"sdl.h"
#include"SOIL/SOIL.h"

//--------------------------------------------------------------------- shader

char*vertex_shader_source =
		"#version 100                                                \n\
uniform mat4 umtx_mw;// model-to-world-matrix                        \n\
attribute vec3 apos;// positions  xyz                            \n\
attribute vec3 argb;// colors  rgb                             \n\
attribute vec3 anorm;// normals  xyz                            \n\
attribute vec2 atex;// texture uv                              \n\
varying vec3 vrgb;                                          \n\
varying vec3 vnorm;                                          \n\
varying vec2 vtex;                                          \n\
void main(){                                                 \n\
	gl_Position=umtx_mw*vec4(apos,1);                        \n\
	vrgb=argb;                                             \n\
	vnorm=anorm;                                             \n\
	vtex=atex;                                             \n\
}\n";

char*fragment_shader_source =
		"#version 100                              \n\
uniform sampler2D utex;                    \n\
varying mediump vec3 vrgb;                \n\
varying mediump vec3 vnorm;                \n\
varying mediump vec2 vtex;                \n\
void main(){             \n\
	                  \n\
//	mediump vec3 ambient_light_vector=;                  \n\
	                  \n\
	                  \n\
	mediump float al=dot(vec3(-1.0,0,0),vnorm);                  \n\
	gl_FragColor=texture2D(utex,vtex)+vec4(vrgb+al,1)*.00001;         \n\
}\n";

typedef struct {
	GLfloat position_xyz[3];
	GLfloat color_rgb[3];
	GLfloat normal_xyz[3];
	GLfloat texture_uv[2];
} vertex;

struct{
	GLuint program_id;
	GLuint position_slot;
	GLuint color_slot;
	GLuint normal_slot;
	GLuint texture_slot;
	GLuint model_to_world_matrix_slot;
	GLuint sampler_slot;
}shader;

inline static const char*get_shader_name_for_type(GLenum shader_type) {
	switch (shader_type){
	case GL_VERTEX_SHADER:return "vertex";
	case GL_FRAGMENT_SHADER:return "fragment";
	default:return "unknown";
	}
}
inline static GLuint compile_shader(GLenum shaderType, char *code) {
//	printf("\n ___| %s shader |__________________\n%s\n",
//			get_shader_name_for_type(shaderType),
//			code);
	GLuint handle=glCreateShader(shaderType);
	int length=(int)strlen(code);
	glShaderSource(handle,1,(const GLchar**)&code,&length);
	glCompileShader(handle);
	GLint ok;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &ok);
	if(!ok){
		GLchar messages[1024];
		glGetShaderInfoLog(handle,sizeof(messages),NULL,&messages[0]);
		printf("compiler error in %s shader:\n%s\n",
			get_shader_name_for_type(shaderType), messages
		);
		exit(7);
	}
	return handle;
}

inline static void load_program() {
	GLuint vertex=compile_shader(GL_VERTEX_SHADER,vertex_shader_source);
	GLuint fragment=compile_shader(GL_FRAGMENT_SHADER,fragment_shader_source);

	shader.program_id=glCreateProgram();
	glAttachShader(shader.program_id,vertex);
	glAttachShader(shader.program_id,fragment);
	glLinkProgram(shader.program_id);

	GLint ok;glGetProgramiv(shader.program_id,GL_LINK_STATUS,&ok);
	if(!ok){
		GLint len;glGetProgramiv(shader.program_id,GL_INFO_LOG_LENGTH,&len);

		GLchar msg[1024];
		if(len>(signed)sizeof msg){
			len=sizeof msg;
		}
		glGetProgramInfoLog(shader.program_id,len,NULL,&msg[0]);
		printf("program linking error: %s\n",msg);
		exit(8);
	}

	glUseProgram(shader.program_id);

	GLint slot;

	slot=glGetAttribLocation(shader.program_id,"apos");
	if(slot==-1){
		puts("could not find 'apos' in vertext shader");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		exit(9);
	}
	shader.position_slot=(GLuint)slot;

	slot=glGetAttribLocation(shader.program_id,"argb");
	if(slot==-1){
		puts("could not find attribute 'argb' in vertex shader");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		exit(10);
	}
	shader.color_slot=(GLuint)slot;

	slot=glGetAttribLocation(shader.program_id,"anorm");
	if(slot==-1){
		puts("could not find attribute 'anorm' in vertex shader");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		exit(10);
	}
	shader.normal_slot=(GLuint)slot;


	slot=glGetAttribLocation(shader.program_id,"atex");
	if(slot==-1){
		puts("could not find attribute 'atex' in vertex shader");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		exit(10);
	}
	shader.texture_slot=(GLuint)slot;


	slot=glGetUniformLocation(shader.program_id,"umtx_mw");
	if(slot==-1){
		puts("could not find uniform 'umtx_mw' in vertex shader");
		exit(10);
	}
	shader.model_to_world_matrix_slot=(GLuint)slot;


	slot=glGetUniformLocation(shader.program_id,"utex");
	if(slot==-1){
		puts("could not find uniform 'utex' in fragment shader");
		exit(10);
	}
	shader.sampler_slot=(GLuint)slot;
}

inline static const char*get_gl_error_string(const GLenum error) {
	const char*str;
	switch (error) {
	case GL_NO_ERROR:
		str = "GL_NO_ERROR";
		break;
	case GL_INVALID_ENUM:
		str = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		str = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		str = "GL_INVALID_OPERATION";
		break;
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
		str = "GL_OUT_OF_MEMORY";
		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
		str = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
		str = "GL_STACK_OVERFLOW";-Wunused-variable
		break;
		case GL_STACK_UNDERFLOW:
		str = "GL_STACK_UNDERFLOW";
		break;
		case GL_TABLE_TOO_LARGE:
		str = "GL_TABLE_TOO_LARGE";
		break;
#endif
	default:
		str = "(ERROR: Unknown Error Enum)";
		break;
	}
	return str;
}

inline static void check_gl_error(const char*op) {
	int err=0;
	for(GLenum error = glGetError(); error; error = glGetError()) {
		printf("!!! %s   glerror %x   %s\n", op, error,
				get_gl_error_string(error));
		err = 1;
	}
	if (err)
		exit(11);
}

inline static void print_gl_string(const char *name, const GLenum s){
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}


//------------------------------------------------------------------ renderable
//static vertex __shader_vertbuf[]={
////	   x  y  z    r  g  b    nx ny nz    u  v
//	{{ 1,-1, 0}, {1, 0, 0}, {0, 0,-1}, { 1,-1}},
//	{{ 1, 1, 0}, {0, 1, 0}, {0, 0,-1}, { 1, 1}},
//	{{-1, 1, 0}, {0, 0, 1}, {0, 0,-1}, {-1, 1}},
//	{{-1,-1, 0}, {1, 1, 0}, {0, 0,-1}, {-1,-1}},
//};

static vertex __shader_vertbuf[]={
//	    x   y  z    r  g  b    nx ny nz    u  v
	{{ .5,-.5, 0}, {1, 0, 0}, {0, 0, 1}, { 1, 0}},
	{{ .5, .5, 0}, {0, 1, 0}, {0, 0, 1}, { 1, 1}},
	{{-.5, .5, 0}, {0, 0, 1}, {0, 0, 1}, { 0, 1}},
	{{-.5,-.5, 0}, {1, 1, 0}, {0, 0, 1}, { 0, 0}},
};
static GLuint  __shader_vertbufid;
static GLuint  __shader_vertbufnbytes=sizeof(__shader_vertbuf);
static GLubyte __shader_ixbuf[]={0,1,2,2,3,0};
static GLuint  __shader_ixbufid;
static GLsizei __shader_ixbufnbytes=sizeof(__shader_ixbuf);
static GLsizei __shader_ixbufn=sizeof(__shader_ixbuf);
static GLsizei __shader_texwidth=2;
static GLsizei __shader_texheight=2;
static GLfloat __shader_texpixels[]={
		0.0f, 1.0f, 0.0f,   0.0f, 0.0f, .5f,
		0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f
};
static GLuint __shader_texid;
//-----------------------------------------------------------------------------

inline static void shader_render();
inline static void shader_init() {
	check_gl_error("before shader_init");
	gl_print_context_profile_and_version();
	print_gl_string("GL_VERSION", GL_VERSION);
	print_gl_string("GL_VENDOR", GL_VENDOR);
	print_gl_string("GL_RENDERER", GL_RENDERER);
	print_gl_string("GL_SHADING_LANGUAGE_VERSION",GL_SHADING_LANGUAGE_VERSION);
	puts("");

	load_program();

	check_gl_error("after load_program");

	// shader data
	glGenBuffers(1,&__shader_vertbufid);
	glBindBuffer(GL_ARRAY_BUFFER,__shader_vertbufid);
	glBufferData(GL_ARRAY_BUFFER,
			__shader_vertbufnbytes,
			__shader_vertbuf,
			GL_STATIC_DRAW);

	glGenBuffers(1,&__shader_ixbufid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,__shader_ixbufid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			__shader_ixbufnbytes,
			__shader_ixbuf,
			GL_STATIC_DRAW);

	glGenTextures(1,&__shader_texid);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,__shader_texid);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
			__shader_texwidth,__shader_texheight,
			0,GL_RGB,GL_FLOAT,
			__shader_texpixels);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	check_gl_error("generating buffers");






	// init for render
	glUniform1i((signed)shader.sampler_slot,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,__shader_texid);

	glEnableVertexAttribArray(shader.position_slot);
	glEnableVertexAttribArray(shader.color_slot);
	glEnableVertexAttribArray(shader.normal_slot);
	glEnableVertexAttribArray(shader.texture_slot);

	check_gl_error("after shader_init");

	// try the shader render

//	glClearColor(.5f,.5f,0,1.0);
//	glClear(GL_COLOR_BUFFER_BIT);
//	shader_render();
//	SDL_GL_SwapWindow(window.ref);
//
//	check_gl_error("after shader render");
//
//	sleep(5);
//	exit(0);
}

inline static void shader_render() {

	check_gl_error("entering shader_render");

	glBindBuffer(GL_ARRAY_BUFFER,__shader_vertbufid);

	glVertexAttribPointer(shader.position_slot,3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);

	glVertexAttribPointer(shader.color_slot,3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(void*)(3*sizeof(float)));

	glVertexAttribPointer(shader.normal_slot,3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(void*)((3+3)*sizeof(float)));

	glVertexAttribPointer(shader.texture_slot,2,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(void*)((3+3+3)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,3);

//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,__shader_ixbufid);
//
//	glDrawElements(GL_TRIANGLES,__shader_ixbufn,GL_UNSIGNED_BYTE,0);
	check_gl_error("after draw elements");
}

inline static void shader_free() {
	if(shader.program_id)
		glDeleteProgram(shader.program_id);
}

