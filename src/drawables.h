#pragma once
#include"mat4.h"
#include"dynf.h"
#include"obj_file.h"
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
	dynf buf=/*takes*/read_obj_file_from_path(
			file_path
		);
	//                   vertex, color, normal
	size_t vertex_size_in_bytes=(3+3+3+2)*sizeof(float);
	drawables[id].vertex_buf_size=buf.size*sizeof(float);
	drawables[id].vertex_count=buf.size/(3+3+3+2);

#ifdef GLOS_EMBEDDED
	drawable[id].vertex_buf=buf;
#else
	GLuint vertex_buffer_id;
	glGenBuffers(1,&vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_id);
	drawables[id].vertex_buf_gid=vertex_buffer_id;
	glBufferData(GL_ARRAY_BUFFER,
			(signed)drawables[id].vertex_buf_size,
			buf.data,
			GL_STATIC_DRAW);
//	free(buf); //?
#endif
//	load_drawable(index);
}
//----------------------------------------------------------------------------
static inline void drawables_draw(int id){
	int stride=(signed)(
			drawables[id].vertex_buf_size/
			drawables[id].vertex_count
		);
#ifdef GLOS_EMBEDDED
	glVertexAttribPointer(shader.position_slot,3,GL_FLOAT,GL_FALSE,stride,
			drawable[id].vertex_buf);

	glVertexAttribPointer(shader.color_slot,3,GL_FLOAT,GL_FALSE,stride,
			(void*)(drawable[id].vertex_buf+(3*sizeof(float))));

	glVertexAttribPointer(shader.normal_slot,3,GL_FLOAT,GL_FALSE,stride,
			(void*)(drawable[id].vertex_buf+((3+3)*sizeof(float))));

	glDrawArrays(GL_TRIANGLES,0,(signed)drawable[id].vertex_count);
#else
	glBindBuffer(GL_ARRAY_BUFFER,drawables[id].vertex_buf_gid);
	glVertexAttribPointer(shader.position_slot,3,GL_FLOAT,GL_FALSE,stride,0);

	glVertexAttribPointer(shader.color_slot,3,GL_FLOAT,GL_FALSE,stride,
		(void*)(3*sizeof(float)));

	glVertexAttribPointer(shader.normal_slot,3,GL_FLOAT,GL_FALSE,stride,
		(void*)((3+3)*sizeof(float)));


	glVertexAttribPointer(shader.texture_slot,2,GL_FLOAT,GL_FALSE,stride,
		(void*)((3+3+2)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,(signed)drawables[id].vertex_count);
#endif
	//	glDrawArrays(GL_LINES,0,(signed)drawable[id].vertex_count);
}
//----------------------------------------------------------------------------
