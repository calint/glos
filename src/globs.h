#pragma once
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
static vertex shader_def_vertbuf[]={
	{{ .5,-.5, 0},{ 1, 0, 0,1},{0,0,1},{ 1,-1}},
	{{ .5, .5, 0},{ 0, 1, 0,1},{0,0,1},{ 1, 1}},
	{{-.5, .5, 0},{ 0, 0, 1,1},{0,0,1},{-1, 1}},
	{{-.5,-.5, 0},{ 0, 0, 0,1},{0,0,1},{-1,-1}},
};
static GLubyte shader_def_ixbuf[]={0,1,2,2,3,0};

#define shader_def_texwi 2
#define shader_def_texhi 2
static GLfloat shader_def_texbuf[]={
		.7f, .7f, .7f,    .2f, .2f, .2f,
		.2f ,.2f ,.2f,    .7f, .7f, .7f
};

static float shader_def_mtx_mw[]={
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
	.vbufn=sizeof(shader_def_vertbuf)/sizeof(&shader_def_vertbuf[0]),
	.vbufnbytes=sizeof(shader_def_vertbuf),
	.ibuf=shader_def_ixbuf,
	.ibufn=sizeof(shader_def_ixbuf)/sizeof(GLubyte),
	.ibufnbytes=sizeof(shader_def_ixbuf),
	.texbufid=0,
	.mtx_mw=0,
	.texbuf=shader_def_texbuf,
	.texbufid=0,
	.texwi=shader_def_texwi,
	.texhi=shader_def_texhi,
	.mtx_mw=shader_def_mtx_mw,
	.vbuf=shader_def_vertbuf,
};

static glob globs[globs_cap];

inline static void shader_render_glob(glob*sr){
	shader_render_triangle_array(
			sr->vbufid,
			sr->vbufn,
			sr->texbufid,
			sr->mtx_mw
	);
}

inline static void globs_init(){}

inline static void globs_free(){}

inline static void globs_render_id(size_t id){
	glob*sr=(glob*)&globs[id];
	shader_render_triangle_array(
			sr->vbufid,
			sr->vbufn,
			sr->texbufid,
			sr->mtx_mw
	);
}

inline static void globs_render(){
	glob*g=globs;
	int n=globs_cap;
	while(n--){
		shader_render_glob(g++);
	}
}
