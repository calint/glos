#pragma once
#include "mat4.h"

//-------------------------------------------------------------------- texture

#define drawable_count 1024
struct{

	float*vertex_buf;

	GLuint vertex_buf_id;

	size_t vertex_buf_size_in_bytes;

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

static inline void load_drawable(int index){
	GLuint vertex_buffer_id;

	glGenBuffers(1,&vertex_buffer_id);

	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_id);

	drawable[index].vertex_buf_id=vertex_buffer_id;

	glBufferData(GL_ARRAY_BUFFER,
			(signed)drawable[index].vertex_buf_size_in_bytes,
			drawable[index].vertex_buf,GL_STATIC_DRAW);
}

static inline void draw_drawable(int index){
	int stride=(signed)(
			drawable[index].vertex_buf_size_in_bytes/
			drawable[index].vertex_count
		);

	glBindBuffer(GL_ARRAY_BUFFER,drawable[index].vertex_buf_id);

	glVertexAttribPointer(
		shader.position_slot,3,GL_FLOAT,GL_FALSE,stride,0);

	glVertexAttribPointer(
		shader.color_slot,   4,GL_FLOAT,GL_FALSE,stride,
		(void*)(3*sizeof(float)));

	glVertexAttribPointer(
		shader.normal_slot,  3,GL_FLOAT,GL_FALSE,stride,
		(void*)((3+4)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,(signed)drawable[index].vertex_count);
}
