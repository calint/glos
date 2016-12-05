#pragma once
#include "sdl.h"

//--------------------------------------------------------------------- shader
typedef struct shader_program{
	GLuint id;
}shader_program;
static shader_program shader_programs[8];

typedef struct {
	float position[3];
	float color[4];
	float normal[3];
	float texture[2];
} vertex;

static vertex vertices[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{1,0}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{1,1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{0,1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{0,0}},
};

static GLubyte indices[]={0,1,2,2,3,0};

static GLsizei texture_width=2;
static GLsizei texture_height=2;
static GLfloat texture_pixels[]={
		1.0f, 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f
};

struct{
	GLuint program_id,vertex_buffer_id,index_buffer_id,texture_id;
	GLuint 	position_slot,
			color_slot,
			normal_slot,
			texture_sampler_slot,
			model_to_world_matrix_slot,
			texture_slot;
	int indices_count;
}shader;

#define _shader_apos 0
#define _shader_argba 1
#define _shader_anorm 2
#define _shader_atex 3
#define _shader_umtx_mw 0
#define _shader_utex 1
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

char*fragment_shader_source =
		"#version 130                              \n\
uniform sampler2D utex;                    \n\
varying mediump vec4 vrgba;                \n\
varying mediump vec3 vnorm;                \n\
varying mediump vec2 vtex;                \n\
void main(){                               \n\
	gl_FragColor=texture2D(utex,vtex)+.00001*vrgba+0.00001*vec4(vnorm,1)+vec4(vtex,0,1);  \n\
}\n";

inline static const char*get_shader_name_for_type(GLenum shader_type) {
	switch (shader_type){
	case GL_VERTEX_SHADER:return "vertex";
	case GL_FRAGMENT_SHADER:return "fragment";
	default:return "unknown";
	}
}
inline static GLuint compile_shader(GLenum shaderType,const char *code) {
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

inline static void check_gl_error(const char*op);
inline static void shader_program_load(int index,const char*vert_src,const char*frag_src) {
	check_gl_error("enter shader_program_load");

	GLuint programid=glCreateProgram();
	shader_programs[index].id=programid;

	GLuint vertex=compile_shader(GL_VERTEX_SHADER,vert_src);
	GLuint fragment=compile_shader(GL_FRAGMENT_SHADER,frag_src);


	glAttachShader(programid,vertex);
	glAttachShader(programid,fragment);
	glLinkProgram(programid);

	GLint ok;glGetProgramiv(programid,GL_LINK_STATUS,&ok);
	if(!ok){
		GLint len;glGetProgramiv(programid,GL_INFO_LOG_LENGTH,&len);

		GLchar msg[1024];
		if(len>(signed)sizeof msg){
			len=sizeof msg;
		}
		glGetProgramInfoLog(programid,len,NULL,&msg[0]);
		printf("program linking error: %s\n",msg);
		exit(8);
	}

	check_gl_error("exit shader_program_load");
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
	int err = 0;
	for (GLenum error = glGetError(); error; error = glGetError()) {
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

inline static void shader_load(){
	check_gl_error("enter shader_load");
	glGenBuffers(1, &shader.vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, shader.vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &shader.index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader.index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	GL_STATIC_DRAW);

	shader.indices_count = sizeof(indices) / sizeof(indices[0]);

//	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&shader.texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,shader.texture_id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
			texture_width,texture_height,
			0,GL_RGB,GL_FLOAT,
			texture_pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	check_gl_error("exit shader_load");
}

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

	shader_program_load(1,vertex_shader_source,fragment_shader_source);
	glUseProgram(shader_programs[1].id);

	shader_load();

	check_gl_error("after shader_init");
}

inline static void shader_render() {
	glEnableVertexAttribArray(_shader_apos);//position
	glEnableVertexAttribArray(_shader_argba);//color
	glEnableVertexAttribArray(_shader_anorm);//normal
	glEnableVertexAttribArray(_shader_atex);//texture

	// Set the active texture unit to texture unit 0.
//	glActiveTexture(GL_TEXTURE0);
	// Bind the texture to this unit.
//	glBindTexture(GL_TEXTURE_2D,shader.texture_id);
	// Tell the texture uniform sampler to use this texture in the shader by binding to texture unit 0.
	float mtxident[]={1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};
	glUniformMatrix4fv(0,1,0,mtxident);
	glUniform1i(_shader_umtx_mw,0);// texture sampler on texture0

	glBindBuffer(GL_ARRAY_BUFFER,shader.vertex_buffer_id);
	glVertexAttribPointer(_shader_apos, 3, GL_FLOAT, GL_FALSE,
			sizeof(vertex),0);
	glVertexAttribPointer(_shader_argba, 4, GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)(3*sizeof(float)));
	glVertexAttribPointer(_shader_anorm, 3, GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4)*sizeof(float)));
	glVertexAttribPointer(_shader_atex, 2, GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shader.index_buffer_id);
	glDrawElements(GL_TRIANGLES,shader.indices_count,GL_UNSIGNED_BYTE,0);
}

inline static void shader_free() {
	if(shader.program_id)
		glDeleteProgram(shader.program_id);
}
