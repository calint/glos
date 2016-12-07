#pragma once
//#include"sdl.h"
#include"metrics.h"
#include"dyni.h"
#include"mat4.h"
#include"dynp.h"

#define programs_cap 8

typedef struct program{
	GLuint gid;
	dyni attributes;
}program;
#define program_def (program){0,dyni_def}

static dynp __programs=dynp_def;
//static program programs[programs_cap];

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

inline static program*program_from_source(
		const char*vert_src,
		const char*frag_src,
		/*takes*/dyni attrs
){
	_check_gl_error("enter shader_program_load");

	GLuint gid=glCreateProgram();

	program*p=malloc(sizeof(program));
	*p=program_def;

	p->gid=gid;

	p->attributes=/*gives*/attrs;

	GLuint vertex=_compile_shader(GL_VERTEX_SHADER,vert_src);

	GLuint fragment=_compile_shader(GL_FRAGMENT_SHADER,frag_src);

	glAttachShader(gid,vertex);
	glAttachShader(gid,fragment);
	glLinkProgram(gid);

	GLint ok;glGetProgramiv(gid,GL_LINK_STATUS,&ok);
	if(!ok){
		GLint len;glGetProgramiv(gid,GL_INFO_LOG_LENGTH,&len);

		GLchar msg[1024];
		if(len>(signed)sizeof msg){
			len=sizeof msg;
		}
		glGetProgramInfoLog(gid,len,NULL,&msg[0]);
		printf("program linking error: %s\n",msg);
		exit(8);
	}
	dynp_add(&__programs,p);
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

//--------------------------------------------------------------------- shader
typedef struct vertex{
	float position[3];
	float color[4];
	float normal[3];
	float texture[2];
}vertex;

static vertex shader_def_vtxbuf[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{ 1,-1}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{ 1, 1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{-1, 1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{-1,-1}},
};

static unsigned shader_def_vtxbuf_nbytes=sizeof(shader_def_vtxbuf);

static unsigned shader_def_vtxbuf_nelems=
		sizeof(shader_def_vtxbuf)/sizeof(vertex);

static unsigned shader_def_vtxbuf_id;

static GLubyte shader_def_ixbuf[]={0,1,2,2,3,0};

static unsigned shader_def_ixbuf_nbytes=sizeof(shader_def_ixbuf);

static unsigned shader_def_ixbuf_nelems=
		sizeof(shader_def_ixbuf)/sizeof(GLubyte);

static unsigned shader_def_ixbuf_id;

static unsigned shader_def_texbuf_wi=2;
static unsigned shader_def_texbuf_hi=2;
static GLfloat shader_def_texbuf[]={
		1,1,1,  0,1,1,
		1,1,0,  1,1,1,
};
static unsigned shader_def_texbuf_nbytes=sizeof(shader_def_texbuf);

static unsigned shader_def_texbuf_id;

//----------------------------------------------------------------------------
static char*shader_vertex_source =
"#version 130                                              \n\
uniform mat4 umtx_mw; // model-to-world-matrix              \n\
uniform mat4 umtx_wvp;// world-to-view-to-projection         \n\
in vec3 apos;                                                 \n\
in vec4 argba;                                                 \n\
in vec3 anorm;                                                  \n\
in vec2 atex;                                                    \n\
out vec4 vrgba;                                                   \n\
out vec3 vnorm;                                                    \n\
out vec2 vtex;                                                      \n\
void main(){                                                         \n\
	gl_Position=umtx_wvp*umtx_mw*vec4(apos,1);                        \n\
	vrgba=argba;                                            \n\
	vnorm=anorm;                                            \n\
	vtex=atex;                                              \n\
}\n";
static char*shader_fragment_source =
		"#version 130                              \n\
uniform sampler2D utex;                    \n\
in vec4 vrgba;                \n\
in vec3 vnorm;                \n\
in vec2 vtex;                \n\
out vec4 rgba;                \n\
void main(){                               \n\
	rgba=texture2D(utex,vtex);\n\
}\n";
#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_umtx_wvp 1
#define shader_utex 2

struct shader{
	gid active_program_ix;
}shader={0};

inline static void shader_load(){
	_check_gl_error("enter shader_load");
	glGenBuffers(1, &shader_def_vtxbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER,shader_def_vtxbuf_id);
	glBufferData(GL_ARRAY_BUFFER,
			shader_def_vtxbuf_nbytes,
			shader_def_vtxbuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_data+=shader_def_vtxbuf_nbytes;

	glGenBuffers(1,&shader_def_ixbuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shader_def_ixbuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shader_def_ixbuf_nbytes,
			shader_def_ixbuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_data+=shader_def_ixbuf_nbytes;

//	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&shader_def_texbuf_id);
	glBindTexture(GL_TEXTURE_2D,shader_def_texbuf_id);

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
			shader_def_texbuf_wi,
			shader_def_texbuf_hi,
			0,GL_RGB,GL_FLOAT,
			shader_def_texbuf);
	metrics.buffered_data+=shader_def_texbuf_nbytes;
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

	glUniformMatrix4fv(shader_umtx_mw,1,0,mtx_mw);

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
			shader_def_vtxbuf_id,
			shader_def_texbuf_id,
			mat4_ident
		);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ixbufid);
	glDrawElements(GL_TRIANGLES,(signed)ixbufn,GL_UNSIGNED_BYTE,0);
	_shader_after_render();
}

inline static void shader_render(){
	const float mtx_wvp[]={
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
	};
	shader_render_triangle_elements(
			shader_def_vtxbuf_id,
			shader_def_vtxbuf_nelems,
			shader_def_ixbuf_id,
			shader_def_ixbuf_nelems,
			shader_def_texbuf_id,
			mtx_wvp);
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
//	glCullFace(GL_FRONT);
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
	program_from_source(
			shader_vertex_source,
			shader_fragment_source,
			/*gives*/attrs);


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


