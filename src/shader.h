#pragma once
#include "sdl.h"

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

	int length = strlen(code);
	glShaderSource(handle, 1, (const GLchar**) &code, &length);

	glCompileShader(handle);

	GLint compileSuccess;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE) {
		GLchar messages[1024];
		glGetShaderInfoLog(handle, sizeof(messages), NULL, &messages[0]);
		printf("compiler error in %s shader:\n%s\n",
				get_shader_name_for_type(shaderType), messages);
		exit(1);
	}

	return handle;
}

inline static void loadShaders(GLuint *_positionSlot, GLuint *_colorSlot) {
	GLuint vertex = compile_shader(GL_VERTEX_SHADER, shader_source_vertex);
	GLuint fragment = compile_shader(GL_FRAGMENT_SHADER,
			shader_source_fragment);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	GLint linkSuccess;
	glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess == GL_FALSE) {
		GLint info_len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);

		GLchar messages[256];
		glGetProgramInfoLog(program, info_len, NULL, &messages[0]);
		printf("program linking error: %s\n", messages);
		exit(1);
	}

	glUseProgram(program);

	*_positionSlot = glGetAttribLocation(program, "apos");
	if (*_positionSlot==-1) {
		puts("could not find 'apos' in vertext shader");
		exit(6);
	}
	*_colorSlot = glGetAttribLocation(program, "argba");
	if (*_colorSlot==-1) {
		puts("could not find 'argba' in vertex shader");
		exit(6);
	}
	glEnableVertexAttribArray(*_positionSlot);
	glEnableVertexAttribArray(*_colorSlot);

	return;
}

typedef struct {
	float Position[3];
	float Color[4];
} Vertex;

static GLuint vertexBuffer, indexBuffer;
static int numIndices;
GLuint _positionSlot, _colorSlot;

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
		exit(5);
}

inline static void createGeometry(GLuint *vertexBuffer, GLuint *indexBuffer,
		int *numIndices) {
	const Vertex Vertices[] = { { { 1, -1, 0 }, { 1, 0, 0, 1 } }, { { 1, 1, 0 },
			{ 0, 1, 0, 1 } }, { { -1, 1, 0 }, { 0, 0, 1, 1 } }, { { -1, -1, 0 },
			{ 0, 0, 0, 1 } } };

	glGenBuffers(1, vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	const GLubyte Indices[] = { 0, 1, 2, 2, 3, 0 };

	glGenBuffers(1, indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices,
	GL_STATIC_DRAW);

	*numIndices = sizeof(Indices) / sizeof(Indices[0]);
	check_gl_error("generating buffers");
}

inline static void print_gl_string(const char *name, const GLenum s) {
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}

inline static void shader_init() {
	check_gl_error("init");
	print_gl_string("GL_VERSION", GL_VERSION);
	print_gl_string("GL_VENDOR", GL_VENDOR);
	print_gl_string("GL_RENDERER", GL_RENDERER);
	//	    printGLString("Extensions",GL_EXTENSIONS);
	print_gl_string("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);
	check_gl_error("after opengl info");

	createGeometry(&vertexBuffer, &indexBuffer, &numIndices);

	// Compile shaders
	loadShaders(&_positionSlot, &_colorSlot);
}

inline static void shader_render() {
	// Tell OpenGL about the structure of our data
	glEnableVertexAttribArray(_positionSlot);
	glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			0);
	glEnableVertexAttribArray(_colorSlot);
	glVertexAttribPointer(_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*) (sizeof(float) * 3));

	// Draw it!
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, 0);
}
