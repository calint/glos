#pragma once
#include"sdl.h"
#include"dynf.h"
#define glob_count 128
//----------------------------------------------------------------------- calls

inline static void shader_render_triangle_array(
	GLuint vbufid,size_t vbufn,GLuint texid,const float*mtx_mw
);

static/*gives*/dynf read_obj_file_from_path(const char*path);

//---------------------------------------------------------------------

typedef struct vertex{
	float position[3];
	float color[4];
	float normal[3];
	float texture[2];
}vertex;

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
		1,1,1,  0,1,1,
		1,1,0,  1,1,1,
};

typedef struct
glob{
	vertex*vbuf;
	GLuint vbufn;
	GLsizeiptr vbufnbytes;
	GLuint vbufid;
	GLubyte*ibuf;//? void* and ibuf_type_size
	GLsizei ibufn;
	GLsizeiptr ibufnbytes;
	GLuint ibufid;
	GLfloat*texbuf;
	GLuint texbufid;
	GLsizei texwi;
	GLsizei texhi;
	bits*ptr_bits;
//	float*mtx_mw;
}glob;

static glob glob_def=(glob){
	.vbuf=glob_def_vertbuf,
	.vbufn=sizeof(glob_def_vertbuf)/sizeof(&glob_def_vertbuf[0]),
	.vbufnbytes=sizeof(glob_def_vertbuf),
	.vbufid=0,
	.ibuf=glob_def_ixbuf,
	.ibufn=sizeof(glob_def_ixbuf)/sizeof(GLubyte),
	.ibufnbytes=sizeof(glob_def_ixbuf),
	.ibufid=0,
	.texbuf=glob_def_texbuf,
	.texbufid=0,
	.texwi=glob_def_texwi,
	.texhi=glob_def_texhi,
};

inline static void glob_render(glob*this,const float*mat4_model_to_world){
	shader_render_triangle_array(
			this->vbufid,
			this->vbufn,
			this->texbufid,
			mat4_model_to_world
	);
}

inline static void glob_load_obj_file(glob*this,const char*path){
	dynf buf=/*takes*/read_obj_file_from_path(path);

	this->vbufn=buf.size;
	this->vbufnbytes=(GLsizeiptr)(buf.size*sizeof(float));
	this->texbufid=glob_def.texbufid;//?

	glGenBuffers(1,&this->vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,this->vbufid);
	glBufferData(GL_ARRAY_BUFFER,
			(signed)this->vbufnbytes,
			buf.data,
			GL_STATIC_DRAW);

	dynf_free(/*gives*/&buf);
}

