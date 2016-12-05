#pragma once
#include"dynf.h"

static/*gives*/dynf read_obj_file_from_path(const char*path);

#define globs_cap 1
//----------------------------------------------------------------------- calls
inline static void shader_render_triangle_elements(
		GLuint vbufid,size_t vbufn,
		GLuint ixbufid,size_t ixbufn,
		GLuint texid,
		float*mtx_mw
);
inline static void shader_render_triangle_array(
		GLuint vbufid,size_t vbufn,GLuint texid,float*mtx_mw
);
//---------------------------------------------------------------------

typedef struct vertex{
	float position[3];
	float color[4];
	float normal[3];
	float texture[2];
}vertex;

//static shader_vertex vertbuf[]={
//	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{1,0}},
//	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{1,1}},
//	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{0,1}},
//	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{0,0}},
//};
static vertex glob_def_vertbuf[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{ 1,-1}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{ 1, 1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{-1, 1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{-1,-1}},
};
static GLubyte glob_def_ixbuf[]={0,1,2,2,3,0};

#define glob_def_texwi 2
#define glob_def_texhi 2
static GLfloat glob_def_texbuf[]={
		.7f, .7f, .7f,    .2f, .2f, .2f,
		.2f ,.2f ,.2f,    .7f, .7f, .7f
};

static float glob_def_mtx_mw[]={
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
};

typedef struct
glob{
	vertex*vbuf;
	GLuint ibufid;
	GLuint vbufn;
	GLuint vbufnbytes;
	GLubyte*ibuf;
	GLuint vbufid;
	ssize_t ibufn;
	ssize_t ibufnbytes;
	GLfloat*texbuf;
	GLuint texbufid;
	GLsizei texwi;
	GLsizei texhi;
	float*mtx_mw;
}glob;

static glob glob_def=(glob){
	.vbufn=sizeof(glob_def_vertbuf)/sizeof(&glob_def_vertbuf[0]),
	.vbufnbytes=sizeof(glob_def_vertbuf),
	.ibuf=glob_def_ixbuf,
	.ibufn=sizeof(glob_def_ixbuf)/sizeof(GLubyte),
	.ibufnbytes=sizeof(glob_def_ixbuf),
	.texbufid=0,
	.mtx_mw=0,
	.texbuf=glob_def_texbuf,
	.texbufid=0,
	.texwi=glob_def_texwi,
	.texhi=glob_def_texhi,
	.mtx_mw=glob_def_mtx_mw,
	.vbuf=glob_def_vertbuf,
};

static glob globs[globs_cap];

inline static void glob_render(glob*this){
	shader_render_triangle_array(
			this->vbufid,
			this->vbufn,
			this->texbufid,
			this->mtx_mw
	);
}

inline static void glob_load_obj(glob*this,const char*path){
	dynf buf=/*takes*/read_obj_file_from_path(path);
	this->vbufn=buf.size;
	this->vbufnbytes=buf.size*sizeof(float);

	glGenBuffers(1,&this->vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,this->vbufid);
	glBufferData(GL_ARRAY_BUFFER,(signed)this->vbufnbytes,buf.data,
			GL_STATIC_DRAW);

	dynf_free(/*gives*/&buf);
}

inline static void globs_init(){}

inline static void globs_free(){}

inline static void globs_render(){
	glob*g=globs;
	int n=globs_cap;
	while(n--){
		glob_render(g++);
	}
}

//inline static void globs_render_id(size_t id){
//	glob*sr=(glob*)&globs[id];//? bounds
//	shader_render_triangle_array(
//			sr->vbufid,
//			sr->vbufn,
//			sr->texbufid,
//			sr->mtx_mw
//	);
//}

//inline static glob*get_glob(size_t id){
//	return&globs[id];//? bounds
//}
