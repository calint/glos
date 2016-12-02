#pragma once
#include "sdl.h"

//--------------------------------------------------------------------- shader

char*shader_source_vertex =
		"#version 100                                                \n\
attribute vec3 apos;// vertices                              \n\
attribute vec4 argba;// colors                               \n\
varying vec4 vrgba;                                          \n\
void main(){                                                 \n\
	gl_Position=vec4(apos,1);                                        \n\
	vrgba=argba;                                             \n\
}\n";

char*shader_source_fragment =
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

static GLuint glid_program, glid_vertex_buffer, glid_index_buffer;

static vertex vertices[] = { { { 1, -1, 0 }, { 1, 0, 0, 1 } }, { { 1, 1, 0 }, {
		0, 1, 0, 1 } }, { { -1, 1, 0 }, { 0, 0, 1, 1 } }, { { -1, -1, 0 }, { 0,
		0, 0, 1 } } };

static GLubyte indices[] = { 0, 1, 2, 2, 3, 0 };

static int indices_count;

static GLuint position_slot, color_slot;

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
	printf("%s", code);
	GLuint handle = glCreateShader(shaderType);

	int length = (int) strlen(code);
	glShaderSource(handle, 1, (const GLchar**) &code, &length);

	glCompileShader(handle);

	GLint compileSuccess;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE) {
		GLchar messages[1024];
		glGetShaderInfoLog(handle, sizeof(messages), NULL, &messages[0]);
		printf("compiler error in %s shader:\n%s\n",
				get_shader_name_for_type(shaderType), messages);
		exit(7);
	}

	return handle;
}

inline static void load_program(GLuint *pos_slot, GLuint *col_slot) {
	GLuint vertex = compile_shader(GL_VERTEX_SHADER, shader_source_vertex);
	GLuint fragment = compile_shader(GL_FRAGMENT_SHADER,
			shader_source_fragment);

	glid_program = glCreateProgram();
	glAttachShader(glid_program, vertex);
	glAttachShader(glid_program, fragment);
	glLinkProgram(glid_program);

	GLint result;
	glGetProgramiv(glid_program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		GLint len;
		glGetProgramiv(glid_program, GL_INFO_LOG_LENGTH, &len);

		GLchar messages[256];
		glGetProgramInfoLog(glid_program,
				(unsigned)len < sizeof messages ? len : sizeof messages, NULL,
				&messages[0]);
		printf("program linking error: %s\n", messages);
		exit(8);
	}

	glUseProgram(glid_program);

	*pos_slot = glGetAttribLocation(glid_program, "apos");
	if (*pos_slot == -1) {
		puts("could not find 'apos' in vertext shader");
		exit(9);
	}
	*col_slot = glGetAttribLocation(glid_program, "argba");
	if (*col_slot == -1) {
		puts("could not find 'argba' in vertex shader");
		exit(10);
	}
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
		str = "GL_STACK_OVERFLOW";
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

inline static void print_gl_string(const char *name, const GLenum s) {
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}

inline static void init_shader() {
	check_gl_error("shader_init");
	print_gl_string("GL_VERSION", GL_VERSION);
	print_gl_string("GL_VENDOR", GL_VENDOR);
	print_gl_string("GL_RENDERER", GL_RENDERER);
	print_gl_string("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);
	create_geometry(&glid_vertex_buffer, &glid_index_buffer, &indices_count);
	load_program(&position_slot, &color_slot);
	check_gl_error("after shader_init");
}

inline static void shader_free() {
	if (glid_program)
		glDeleteProgram(glid_program);
}

inline static void shader_render() {
#ifdef GLOS_EMBEDDED
	glVertexAttribPointer(position_slot, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			vertices);
	glVertexAttribPointer(color_slot, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
			vertices);
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, indices);

#else
	glBindBuffer(GL_ARRAY_BUFFER, glid_vertex_buffer);
	glVertexAttribPointer(position_slot, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glid_index_buffer);
	glVertexAttribPointer(color_slot, 4, GL_FLOAT, GL_FALSE, sizeof(vertex),
			(GLvoid*) (3 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, 0);
#endif
}
