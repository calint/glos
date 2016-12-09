#pragma once
//#include"sdl.h"
#include"metrics.h"
#include"dyni.h"
#include"mat4.h"
#include"dynp.h"

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
//	rgba=texture2D(utex,vtex)+vrgba;\n\
	rgba=vrgba;\n\
}\n";
#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_umtx_wvp 1
#define shader_utex 2




#define programs_cap 8
typedef struct program{
	GLuint gid;
	dyni attributes;
}program;
#define program_def (program){0,dyni_def}

static dynp programs=dynp_def;
//static program programs[programs_cap];

inline static const char*shader_name_for_type(GLenum shader_type);
inline static void gl_check_error(const char*op);
inline static void gl_print_string(const char *name, const GLenum s);

inline static GLuint shader_compile(GLenum shaderType,const char *code) {
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
			shader_name_for_type(shaderType), messages
		);
		exit(7);
	}
	return id;
}

inline static program*program_load_from_source(
		const char*vert_src,
		const char*frag_src,
		/*takes*/dyni attrs
){
	gl_check_error("enter shader_program_load");

	GLuint gid=glCreateProgram();

	program*p=malloc(sizeof(program));
	*p=program_def;

	p->gid=gid;

	p->attributes=/*gives*/attrs;

	GLuint vertex=shader_compile(GL_VERTEX_SHADER,vert_src);

	GLuint fragment=shader_compile(GL_FRAGMENT_SHADER,frag_src);

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
	dynp_add(&programs,p);
	gl_check_error("exit shader_program_load");
	return p;
}

inline static const char*gl_get_error_string(const GLenum error) {
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

inline static void gl_print_string(const char*name,const GLenum s){
	const char*v=(const char*)glGetString(s);
	printf("%s=%s\n",name,v);
}

inline static void gl_check_error(const char*op) {
	int err = 0;
	for (GLenum error = glGetError(); error; error = glGetError()) {
		printf("!!! %s   glerror %x   %s\n", op, error,
				gl_get_error_string(error));
		err = 1;
	}
	if (err)
		exit(11);
}
inline static const char*shader_name_for_type(GLenum shader_type) {
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
struct shader{
	gid active_program_ix;
}shader={0};

inline static void shader_load(){
	gl_check_error("enter shader_load");
	glGenBuffers(1, &shader_def_vtxbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER,shader_def_vtxbuf_id);
	glBufferData(GL_ARRAY_BUFFER,
			shader_def_vtxbuf_nbytes,
			shader_def_vtxbuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_vertex_data+=shader_def_vtxbuf_nbytes;

	glGenBuffers(1,&shader_def_ixbuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shader_def_ixbuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shader_def_ixbuf_nbytes,
			shader_def_ixbuf,
			GL_STATIC_DRAW
	);
	metrics.buffered_vertex_data+=shader_def_ixbuf_nbytes;

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
			(signed)shader_def_texbuf_wi,
			(signed)shader_def_texbuf_hi,
			0,GL_RGB,GL_FLOAT,
			shader_def_texbuf);
	metrics.buffered_texture_data+=shader_def_texbuf_nbytes;
	//----------------------------------------------

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	gl_check_error("exit shader_load");
}

inline static void shader_render_triangle_elements(
		GLuint vbufid,size_t vbufn,
		GLuint ixbufid,size_t ixbufn,
		GLuint texid,const float*mtx_mw
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ixbufid);
	glDrawElements(GL_TRIANGLES,(signed)ixbufn,GL_UNSIGNED_BYTE,0);

	glBindTexture(GL_TEXTURE_2D,0);
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
//
//inline static void shader_render_triangle_array(
//		GLuint vbufid,size_t vbufn,GLuint texid,const float*mtx_mw
//){
//	shader_prepare_for_render(vbufid,texid,mtx_mw);
//	glDrawArrays(GL_TRIANGLES,0,(int)vbufn);
//	glBindTexture(GL_TEXTURE_2D,0);
//}
//
inline static void shader_free() {
	//? free programs?
//	if(shader.program_id)
//		glDeleteProgram(shader.program_id);
}

inline static void shader_init() {
	gl_check_error("shader_init");

//	gl_print_context_profile_and_version();


	puts("");
	gl_print_string("GL_VERSION", GL_VERSION);
	gl_print_string("GL_VENDOR", GL_VENDOR);
	gl_print_string("GL_RENDERER", GL_RENDERER);
	gl_print_string("GL_SHADING_LANGUAGE_VERSION",GL_SHADING_LANGUAGE_VERSION);
	puts("");


//	without projection -z if farther
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepthf(-1);

	// with projection
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	glClearDepthf(1);


//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
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
	program_load_from_source(
			shader_vertex_source,
			shader_fragment_source,
			/*gives*/attrs);


	shader_load();

	gl_check_error("after shader_init");

	///----------------------------------------------
}


