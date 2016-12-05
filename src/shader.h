#pragma once
#include "sdl.h"
//--------------------------------------------------------------------- shader
#define shader_program_cap 8
//---------------------------------------------------------------------

typedef struct shader_vertex{
	float position[3];
	float color[4];
	float normal[3];
	float texture[2];
}shader_vertex;

//static shader_vertex vertbuf[]={
//	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{1,0}},
//	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{1,1}},
//	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{0,1}},
//	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{0,0}},
//};
static shader_vertex vertbuf[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{ 1,-1}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{ 1, 1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{-1, 1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{-1,-1}},
};
static GLuint vertbufid;

static GLubyte ixbuf[]={0,1,2,2,3,0};
static GLuint ixbufid;
static ssize_t ixbufn=sizeof(ixbuf)/sizeof(ixbuf[0]);

static GLsizei texwi=2;
static GLsizei texhi=2;
static GLfloat texbuf[]={
		.7f, .7f, .7f,    .2f, .2f, .2f,
		.2f ,.2f ,.2f,    .7f, .7f, .7f
};
static GLuint texbufid;

typedef struct shader_program{
	GLuint id;
}shader_program;

static shader_program shader_programs[shader_program_cap];

//struct{}shader;

char*vertex_shader_source =
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
#define _shader_apos 0
#define _shader_argba 1
#define _shader_anorm 2
#define _shader_atex 3
#define _shader_umtx_mw 0

char*fragment_shader_source =
		"#version 130                              \n\
uniform sampler2D utex;                    \n\
varying mediump vec4 vrgba;                \n\
varying mediump vec3 vnorm;                \n\
varying mediump vec2 vtex;                \n\
void main(){                               \n\
	gl_FragColor=texture2D(utex,vtex)+vrgba*dot(vec4(1,0,0,1),vec4(vnorm,1));\n\
}\n";
#define _shader_utex 1

inline static const char*get_shader_name_for_type(GLenum shader_type);
inline static void check_gl_error(const char*op);

inline static GLuint compile_shader(GLenum shaderType,const char *code) {
//	printf("\n ___| %s shader |__________________\n%s\n",
//			get_shader_name_for_type(shaderType),
//			code);
	GLuint id=glCreateShader(shaderType);
	size_t length=strlen(code);
	glShaderSource(id,1,(const GLchar**)&code,(GLint*)&length);
	glCompileShader(id);
	GLint ok;glGetShaderiv(id,GL_COMPILE_STATUS,&ok);
	if(!ok){
		GLchar messages[1024];
		glGetShaderInfoLog(id,sizeof(messages),NULL,&messages[0]);
		printf("compiler error in %s shader:\n%s\n",
			get_shader_name_for_type(shaderType), messages
		);
		exit(7);
	}
	return id;
}

inline static void shader_program_load(int index,const char*vert_src,const char*frag_src) {
	check_gl_error("enter shader_program_load");

	GLuint id=glCreateProgram();
	shader_programs[index].id=id;

	GLuint vertex=compile_shader(GL_VERTEX_SHADER,vert_src);

	GLuint fragment=compile_shader(GL_FRAGMENT_SHADER,frag_src);

	glAttachShader(id,vertex);
	glAttachShader(id,fragment);
	glLinkProgram(id);

	GLint ok;glGetProgramiv(id,GL_LINK_STATUS,&ok);
	if(!ok){
		GLint len;glGetProgramiv(id,GL_INFO_LOG_LENGTH,&len);

		GLchar msg[1024];
		if(len>(signed)sizeof msg){
			len=sizeof msg;
		}
		glGetProgramInfoLog(id,len,NULL,&msg[0]);
		printf("program linking error: %s\n",msg);
		exit(8);
	}

	check_gl_error("exit shader_program_load");
}

inline static void shader_load(){
	check_gl_error("enter shader_load");
	glGenBuffers(1, &vertbufid);
	glBindBuffer(GL_ARRAY_BUFFER, vertbufid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertbuf),vertbuf,GL_STATIC_DRAW);

	glGenBuffers(1, &ixbufid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ixbufid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ixbuf),ixbuf,GL_STATIC_DRAW);

//	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texbufid);
	glBindTexture(GL_TEXTURE_2D,texbufid);

	SDL_Surface*surface=IMG_Load("logo.jpg");
//    GLenum data_fmt;
//    Uint8 test = SDL_MapRGB(surface->format, 0xAA,0xBB,0xCC)&0xFF;
//    if      (test==0xAA) data_fmt=         GL_RGB;
//    else if (test==0xCC) data_fmt=0x80E0;//GL_BGR;
//    else {
//        printf("Error: \"Loaded surface was neither RGB or BGR!\""); return;
//    }
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
			surface->w,surface->h,
			0,GL_RGB,GL_UNSIGNED_BYTE,
			surface->pixels);
	SDL_FreeSurface(surface);

//	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
//			texwi,texhi,
//			0,GL_RGB,GL_FLOAT,
//			texbuf);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	check_gl_error("exit shader_load");
}

inline static void shader_render() {
	glEnableVertexAttribArray(_shader_apos);//position
	glEnableVertexAttribArray(_shader_argba);//color
	glEnableVertexAttribArray(_shader_anorm);//normal
	glEnableVertexAttribArray(_shader_atex);//texture
	glUseProgram(shader_programs[0].id);
	//? enabled


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texbufid);
	glUniform1i(_shader_utex,0);

	float mtxident[]={1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};
	glUniformMatrix4fv(0,1,0,mtxident);

	glBindBuffer(GL_ARRAY_BUFFER,vertbufid);
	glVertexAttribPointer(_shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(shader_vertex),0);
	glVertexAttribPointer(_shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(_shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(_shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ixbufid);
	glDrawElements(GL_TRIANGLES,(signed)ixbufn,GL_UNSIGNED_BYTE,0);


	//? reset
	glBindTexture(GL_TEXTURE_2D,0);
	glDisableVertexAttribArray(_shader_apos);//position
	glDisableVertexAttribArray(_shader_argba);//color
	glDisableVertexAttribArray(_shader_anorm);//normal
	glDisableVertexAttribArray(_shader_atex);//texture

}

inline static void shader_render_triangle_array(
		GLuint vbufid,size_t vbufn,GLuint texid,float*mtx_mw
){
	glEnableVertexAttribArray(_shader_apos);//position
	glEnableVertexAttribArray(_shader_argba);//color
	glEnableVertexAttribArray(_shader_anorm);//normal
	glEnableVertexAttribArray(_shader_atex);//texture
	glUseProgram(shader_programs[0].id);
	//? enabled

	glUniformMatrix4fv(0,1,0,mtx_mw);
	glUniform1i(_shader_utex,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texid);

	glBindBuffer(GL_ARRAY_BUFFER,vbufid);
	glVertexAttribPointer(_shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(shader_vertex),0);
	glVertexAttribPointer(_shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(_shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(_shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(shader_vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,(int)vbufn);


	//? reset
	glBindTexture(GL_TEXTURE_2D,0);
	glDisableVertexAttribArray(_shader_apos);//position
	glDisableVertexAttribArray(_shader_argba);//color
	glDisableVertexAttribArray(_shader_anorm);//normal
	glDisableVertexAttribArray(_shader_atex);//texture

}


















inline static void print_gl_string(const char *name, const GLenum s);
inline static void shader_init() {
	check_gl_error("shader_init");

//	gl_print_context_profile_and_version();


	puts("");
	print_gl_string("GL_VERSION", GL_VERSION);
	print_gl_string("GL_VENDOR", GL_VENDOR);
	print_gl_string("GL_RENDERER", GL_RENDERER);
	print_gl_string("GL_SHADING_LANGUAGE_VERSION",GL_SHADING_LANGUAGE_VERSION);
	puts("");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s :-%7s-:\n","feature","");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	printf(": %10s : %-7s :\n","cull face",glIsEnabled(GL_CULL_FACE)?"yes":"no");
	printf(": %10s : %-7s :\n","blend",glIsEnabled(GL_BLEND)?"yes":"no");
	printf(":-%10s-:-%7s-:\n","----------","-------");
	puts("");

	puts("");

	shader_program_load(0,vertex_shader_source,fragment_shader_source);
//	glUseProgram(shader_programs[0].id);

	shader_load();

	check_gl_error("after shader_init");
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

inline static void print_gl_string(const char *name, const GLenum s){
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}

inline static void shader_free() {
	//? free programs?
//	if(shader.program_id)
//		glDeleteProgram(shader.program_id);
}

inline static void check_gl_error(const char*op) {
	int err = 0;
	for (GLenum error = glGetError(); error; error = glGetError()) {
		printf("!!! %s   glerror %x   %s\n", op, error,
				get_gl_error_string(error));
		err = 1;
	}
	if (err)
		exit(11);
}
inline static const char*get_shader_name_for_type(GLenum shader_type) {
	switch (shader_type){
	case GL_VERTEX_SHADER:return "vertex";
	case GL_FRAGMENT_SHADER:return "fragment";
	default:return "unknown";
	}
}
