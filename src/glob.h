#pragma once
#include"sdl.h"
#include"dynf.h"
#include"metrics.h"
#include"obj_file.h"
#define glob_count 128
//----------------------------------------------------------------------- calls

inline static void shader_render_triangle_array(
	GLuint vbufid,size_t vbufn,GLuint texid,const float*mtx_mw
);

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
	dynp material_ranges;
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
	.material_ranges=dynp_def,
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
	glo*g=/*takes*/read_obj_file_from_path(path);

	this->vbufn=g->vertex_buffer.count;
	this->vbufnbytes=(GLsizeiptr)(this->vbufn*sizeof(float));
	this->texbufid=glob_def.texbufid;//?
	this->material_ranges=g->render_ranges;

	glGenBuffers(1,&this->vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,this->vbufid);
	glBufferData(GL_ARRAY_BUFFER,
			(signed)this->vbufnbytes,
			g->vertex_buffer.data,
			GL_STATIC_DRAW);

	metrics.buffered_data+=(unsigned)this->vbufnbytes;
}

