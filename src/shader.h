#pragma once
#include "sdl.h"

//--------------------------------------------------------------------- shader

char*vertex_shader_source =
		"#version 100                                                \n\
attribute vec3 apos;// vertices                              \n\
attribute vec4 argba;// colors                               \n\
varying vec4 vrgba;                                          \n\
void main(){                                                 \n\
	gl_Position=vec4(apos,1);                                        \n\
	vrgba=argba;                                             \n\
}\n";

char*fragment_shader_source =
		"#version 100                              \n\
uniform sampler2D utex;                    \n\
varying mediump vec4 vrgba;                \n\
void main(){                               \n\
	gl_FragColor=vrgba;                    \n\
}\n";

typedef struct {
	float position[3];
	float color[4];
} vertex;

static vertex vertices[]={
	{{ 1,-1, 0},{ 1, 0, 0,1}},
	{{ 1, 1, 0},{ 0, 1, 0,1}},
	{{-1, 1, 0},{ 0, 0, 1,1}},
	{{-1,-1, 0},{ 0, 0, 0,1}},
};

static GLubyte indices[]={0,1,2,2,3,0};

struct{
	GLuint program_id,vertex_buffer_id,index_buffer_id;
	GLuint position_slot,color_slot;
	int indices_count;
}shader;

inline static const char*get_shader_name_for_type(GLenum shader_type) {
	switch (shader_type) {
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	default:
		return "unknown";
	}
}
inline static GLuint compile_shader(GLenum shaderType, char *code) {
	printf("\n ___| %s shader |__________________\n%s\n",
			get_shader_name_for_type(shaderType),
			code);

	GLuint handle = glCreateShader(shaderType);

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

inline static void load_program(GLuint*pos_slot,GLuint *col_slot) {
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
		exit(9);
	}
	*pos_slot=(GLuint)slot;

	slot=glGetAttribLocation(shader.program_id,"argba");
	if(slot==-1){
		puts("could not find 'argba' in vertex shader");
		exit(10);
	}
	*col_slot=(GLuint)slot;

	glEnableVertexAttribArray(*pos_slot);
	glEnableVertexAttribArray(*col_slot);

	return;
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

inline static void create_geometry(GLuint *vertexBuffer, GLuint *indexBuffer,
		int *numIndices) {

	glGenBuffers(1, vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	GL_STATIC_DRAW);

	*numIndices = sizeof(indices) / sizeof(indices[0]);
	check_gl_error("generating buffers");
}

inline static void print_gl_string(const char *name, const GLenum s){
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}

inline static void init_shader() {
	check_gl_error("shader_init");
	gl_print_context_profile_and_version();
	puts("");
	print_gl_string("GL_VERSION", GL_VERSION);
	print_gl_string("GL_VENDOR", GL_VENDOR);
	print_gl_string("GL_RENDERER", GL_RENDERER);
	print_gl_string("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);

	create_geometry(
		&shader.vertex_buffer_id,
		&shader.index_buffer_id,
		&shader.indices_count
	);

	load_program(&shader.position_slot,&shader.color_slot);

	check_gl_error("after shader_init");
}

inline static void free_shader() {
	if(shader.program_id)
		glDeleteProgram(shader.program_id);
}

inline static void shader_render() {
#ifdef GLOS_EMBEDDED
	glVertexAttribPointer(
		position_slot,3, GL_FLOAT,GL_FALSE,sizeof(vertex),vertices
	);
	glVertexAttribPointer(
		color_slot,4,GL_FLOAT,GL_FALSE,sizeof(vertex),vertices);

	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, indices);

#else
	glBindBuffer(GL_ARRAY_BUFFER,shader.vertex_buffer_id);
	glVertexAttribPointer(shader.position_slot,3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shader.index_buffer_id);
	glVertexAttribPointer(shader.color_slot,4,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(GLvoid*)(3*sizeof(float)));

	glDrawElements(GL_TRIANGLES,shader.indices_count,GL_UNSIGNED_BYTE,0);
#endif
}
