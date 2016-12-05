#pragma once
#include"mat4.h"
#include"dynf.h"
#include"obj_file.h"
#include"shader.h"
#include"globs.h"
//-------------------------------------------------------------------- texture
#define drawables_count 1024
struct{
	float*vertex_buf;
	GLuint vertex_buf_gid;
	size_t vertex_buf_size;
	size_t vertex_count;
}drawables[drawables_count];
//----------------------------------------------------------------------- init
static inline void drawables_init(){}
//----------------------------------------------------------------------- free
static inline void drawables_free(){
	for (int i=0;i<drawables_count;i++){
		if(drawables[i].vertex_buf)free(drawables[i].vertex_buf);
		if(drawables[i].vertex_buf_gid)
			glDeleteBuffers(1,&drawables[i].vertex_buf_gid);
	}
}
//----------------------------------------------------------------------------
static void drawables_load_file_in_slot(int id,const char*file_path){
	dynf buf=/*takes*/read_obj_file_from_path(file_path);
	drawables[id].vertex_buf_size=buf.size*sizeof(float);
	drawables[id].vertex_count=buf.size/(3+3+3+2);

	GLuint vertex_buffer_id;
	glGenBuffers(1,&vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_id);
	drawables[id].vertex_buf_gid=vertex_buffer_id;
	glBufferData(GL_ARRAY_BUFFER,
			(signed)drawables[id].vertex_buf_size,
			buf.data,
			GL_STATIC_DRAW);
	dynf_free(/*gives*/&buf);
//	load_drawable(index);
}
//----------------------------------------------------------------------------
static inline void drawables_draw(int id){
	float mtxident[]={1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};
	shader_render_triangle_array(
			drawables[id].vertex_buf_gid,
			drawables[id].vertex_count,
			glob_def.texbufid,
			mtxident
		);
}
//----------------------------------------------------------------------------
