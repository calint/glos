#pragma once
#include "mat4.h"

//-------------------------------------------------------------------- texture

#define drawable_count 1024
struct{

	float*vertex_buf;

	GLuint vertex_buf_gid;

	size_t vertex_buf_size;

	size_t vertex_count;

}drawable[drawable_count];

int drawable_next_index;

//----------------------------------------------------------------------- init

static inline void init_drawables(){}

//----------------------------------------------------------------------- free

static inline void free_drawables(){
	for (int i=0;i<drawable_count;i++){
		if(drawable[i].vertex_buf)
			free(drawable[i].vertex_buf);
	}
}

//----------------------------------------------------------------------------

static void load_drawable(int id,const char*file_path){
	size_t buf_size_in_bytes;
	float*buf=/*takes*/read_obj_file_from_path(
			file_path,
			&buf_size_in_bytes
		);

	//                   vertex, color, normal
	size_t vertex_size_in_bytes=(3+3+3)*sizeof(float);

	drawable[id].vertex_buf=buf;
	drawable[id].vertex_buf_size=buf_size_in_bytes;
	drawable[id].vertex_count=(unsigned)(buf_size_in_bytes/vertex_size_in_bytes);

	GLuint vertex_buffer_id;
	glGenBuffers(1,&vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_id);
	drawable[id].vertex_buf_gid=vertex_buffer_id;
	glBufferData(GL_ARRAY_BUFFER,(signed)drawable[id].vertex_buf_size,
			drawable[id].vertex_buf,GL_STATIC_DRAW);
	free(buf);
//	load_drawable(index);
}

static inline void draw_drawable(int id){
	int stride=(signed)(
			drawable[id].vertex_buf_size/
			drawable[id].vertex_count
		);

	glBindBuffer(GL_ARRAY_BUFFER,drawable[id].vertex_buf_gid);

	glVertexAttribPointer(shader.position_slot,3,GL_FLOAT,GL_FALSE,stride,0);

	glVertexAttribPointer(shader.color_slot,3,GL_FLOAT,GL_FALSE,stride,
		(void*)(3*sizeof(float)));

	glVertexAttribPointer(shader.normal_slot,3,GL_FLOAT,GL_FALSE,stride,
		(void*)((3+3)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,(signed)drawable[id].vertex_count);
//	glDrawArrays(GL_LINES,0,(signed)drawable[id].vertex_count);
}

