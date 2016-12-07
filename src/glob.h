#pragma once
#include"sdl.h"
#include"dynf.h"
#include"metrics.h"
#include"obj_file.h"
#include"shader.h"

#define glob_count 128
//----------------------------------------------------------------------- calls

typedef struct glob{
	glo glo;
}glob;

static glob glob_def=(glob){
	.glo={{0,0,0},{0,0,0}},
};

#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_umtx_wvp 1
#define shader_utex 2


inline static void glob_load_obj_file(glob*this,const char*path){
	this->glo=/*takes*/read_obj_file_from_path(path);

	// upload vertex buffer
	glGenBuffers(1,&this->glo.vtxbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER,this->glo.vtxbuf_id);
	glBufferData(GL_ARRAY_BUFFER,
			(signed)dynf_size_in_bytes(&this->glo.vtxbuf),
			this->glo.vtxbuf.data,
			GL_STATIC_DRAW);

	// upload materials
	for(indx i=0;i<this->glo.ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->glo.ranges.data[i];
		objmtl*m=(objmtl*)mr->material;
		if(m->map_Kd.count){// load texture
			glGenTextures(1,&m->gid_texture);

			printf(" * loading texture %u from '%s'\n",
					m->gid_texture,m->map_Kd.data);

			glBindTexture(GL_TEXTURE_2D,m->gid_texture);

			SDL_Surface*surface=IMG_Load(m->map_Kd.data);
			if(!surface)exit(-1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
					surface->w,surface->h,
					0,GL_RGB,GL_UNSIGNED_BYTE,
					surface->pixels);
			SDL_FreeSurface(surface);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		}
	}

	metrics.buffered_data+=dynf_size_in_bytes(&this->glo.vtxbuf);
}

inline static void glob_render(glob*this,const float*mtxmw){

	glUniformMatrix4fv(shader_umtx_mw,1,0,mtxmw);

	glBindBuffer(GL_ARRAY_BUFFER,this->glo.vtxbuf_id);

	glVertexAttribPointer(shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);
	glVertexAttribPointer(shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	for(indx i=0;i<this->glo.ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->glo.ranges.data[i];
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


































