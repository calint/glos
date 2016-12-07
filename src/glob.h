#pragma once
#include"sdl.h"
#include"dynf.h"
#include"metrics.h"
#include"obj_file.h"
#include"shader.h"

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

static vertex shader_def_vtxbuf[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{ 1,-1}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{ 1, 1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{-1, 1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{-1,-1}},
};
static GLubyte shader_def_ixbu[]={0,1,2,2,3,0};

#define shader_def_tex_wi 2
#define shader_def_tex_hi 2
static GLfloat shader_def_texbuf[]={
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
	glo glo;
}glob;
#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_umtx_wvp 1
#define shader_utex 2
static glob glob_def=(glob){
	.vbuf=shader_def_vtxbuf,
	.vbufn=sizeof(shader_def_vtxbuf)/sizeof(&shader_def_vtxbuf[0]),
	.vbufnbytes=sizeof(shader_def_vtxbuf),
	.vbufid=0,
	.ibuf=shader_def_ixbu,
	.ibufn=sizeof(shader_def_ixbu)/sizeof(GLubyte),
	.ibufnbytes=sizeof(shader_def_ixbu),
	.ibufid=0,
	.texbuf=shader_def_texbuf,
	.texbufid=0,
	.texwi=shader_def_tex_wi,
	.texhi=shader_def_tex_hi,
	.glo={{0,0,0},{0,0,0}},
};


inline static void glob_load_obj_file(glob*this,const char*path){
	glo g=/*takes*/read_obj_file_from_path(path);
	this->glo=g;
	this->vbufnbytes=(unsigned)dynf_size_in_bytes(&g.vtxbuf);
	this->vbufn=g.vtxbuf.count;

	// upload vertex buffer
	glGenBuffers(1,&this->vbufid);
	glBindBuffer(GL_ARRAY_BUFFER,this->vbufid);
	glBufferData(GL_ARRAY_BUFFER,
			(signed)dynf_size_in_bytes(&g.vtxbuf),
			g.vtxbuf.data,
			GL_STATIC_DRAW);

	// upload materials
	for(arrayix i=0;i<g.ranges.count;i++){
		material_range*mr=(material_range*)g.ranges.data[i];
		objmtl*m=(objmtl*)mr->material;
		if(m->map_Kd.count){// load texture
			glGenTextures(1,&m->gid_texture);

			printf(" * loading texture %u from '%s'\n",
					m->gid_texture,m->map_Kd.data);

			glBindTexture(GL_TEXTURE_2D,m->gid_texture);

			SDL_Surface*surface=IMG_Load(m->map_Kd.data);
			if(!surface){
				exit(-1);
			}

			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
					surface->w,surface->h,
					0,GL_RGB,GL_UNSIGNED_BYTE,
					surface->pixels);
			SDL_FreeSurface(surface);

			//----------------------------------------------
//			glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
//					glob_def.texwi,glob_def.texhi,
//					0,GL_RGB,GL_FLOAT,
//					glob_def.texbuf);
//			metrics.buffered_data+=(unsigned)(
//					glob_def.texhi*glob_def.texwi*(signed)sizeof(GL_FLOAT));
			//----------------------------------------------

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		}
	}

	metrics.buffered_data+=(unsigned)this->vbufnbytes;
}

inline static void glob_render(glob*this,const float*mtxmw){

	glUniformMatrix4fv(shader_umtx_mw,1,0,mtxmw);

	glBindBuffer(GL_ARRAY_BUFFER,this->vbufid);

	glVertexAttribPointer(shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);
	glVertexAttribPointer(shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	for(arrayix i=0;i<this->glo.ranges.count;i++){
		material_range*mr=(material_range*)this->glo.ranges.data[i];
		objmtl*m=mr->material;

		if(m->gid_texture){
			glUniform1i(shader_utex,0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m->gid_texture);
			glEnableVertexAttribArray(shader_atex);
		}else{
			glDisableVertexAttribArray(shader_atex);
		}

		glDrawArrays(GL_TRIANGLES,(signed)mr->begin,(signed)(mr->end-mr->begin));

		if(m->gid_texture){
			glBindTexture(GL_TEXTURE_2D,0);
		}
	}
}


































