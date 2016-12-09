#pragma once

struct{
	position eye;
	float mxwvp[16];
	position lookat;
	vec4 up;
	float znear,zfar;
	float wi,hi;
}camera={
		.eye={0,0,1,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=0,
		.zfar=10,
		.wi=2,
		.hi=2,
};
inline static void camera_update_matrix_wvp(){
	float Ml[16];
	mat4_set_look_at(Ml,&camera.eye,&(vec4){0,0,0,0},&(vec4){0,1,0,0});

	float Mt[16];
	position Pt=camera.eye;
	vec4_negate(&Pt);
	mat4_set_translation(Mt,&Pt);

	float Mtl[16];
	mat4_multiply(Mtl,Mt,Ml);

	float Mp[16];
	mat4_set_ortho_projection(Mp,
			-camera.wi,camera.wi,
			-camera.hi,camera.hi,
			camera.znear,camera.zfar);


	float Mtlp[16];
	mat4_multiply(Mtlp,Mp,Mtl);


	mat4_assign(camera.mxwvp,Mtlp);

//		float mtx_lookat[16];
//		mat4_set_look_at(mtx_lookat,&camera.eye,&camera.lookat,&camera.up);
//		memcpy(camera.mxwvp,mtx_lookat,4*4*sizeof(float));
//		return;
//
}
