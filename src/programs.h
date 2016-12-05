#pragma once
#include"sdl.h"
#include"globs.h"

#define programs_cap 1

typedef struct program{
	GLuint id;
}program;

static program programs[programs_cap];

inline static const char*_get_shader_name_for_type(GLenum shader_type);
inline static void _check_gl_error(const char*op);
inline static void _print_gl_string(const char *name, const GLenum s);

inline static GLuint _compile_shader(GLenum shaderType,const char *code) {
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
			_get_shader_name_for_type(shaderType), messages
		);
		exit(7);
	}
	return id;
}

inline static void programs_load(arrayix i,const char*vert_src,const char*frag_src) {
	_check_gl_error("enter shader_program_load");

	GLuint id=glCreateProgram();
	programs[i].id=id;

	GLuint vertex=_compile_shader(GL_VERTEX_SHADER,vert_src);

	GLuint fragment=_compile_shader(GL_FRAGMENT_SHADER,frag_src);

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

	_check_gl_error("exit shader_program_load");
}

inline static const char*_get_gl_error_string(const GLenum error) {
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

inline static void _print_gl_string(const char *name, const GLenum s){
	const char*v = (const char*) glGetString(s);
	printf("%s=%s\n", name, v);
}

inline static void _check_gl_error(const char*op) {
	int err = 0;
	for (GLenum error = glGetError(); error; error = glGetError()) {
		printf("!!! %s   glerror %x   %s\n", op, error,
				_get_gl_error_string(error));
		err = 1;
	}
	if (err)
		exit(11);
}
inline static const char*_get_shader_name_for_type(GLenum shader_type) {
	switch (shader_type){
	case GL_VERTEX_SHADER:return "vertex";
	case GL_FRAGMENT_SHADER:return "fragment";
	default:return "unknown";
	}
}
