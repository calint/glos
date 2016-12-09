#pragma once

struct{
	position eye;
	float mxwvp[16];
	position lookat;
	vec4 up;
	float znear,zfar;
	float wi,hi;
	int ortho;
}camera={
		.eye={0,0,2,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=1,
		.zfar=100,
		.wi=2,
		.hi=2,
		.ortho=0,
};
inline static void camera_update_matrix_wvp(){
	if(camera.ortho){
		float Ml[16];
		mat4_set_look_at(Ml,&camera.eye,&(vec4){0,0,0,0},&(vec4){0,1,0,0});

		float Mt[16];
		position Pt=camera.eye;
		vec4_negate(&Pt);

		mat4_set_translation(Mt,&Pt);
	//	mat4_set_identity(Mt);

		float Mtl[16];
		mat4_multiply(Mtl,Mt,Ml);

		float Mp[16];
		mat4_set_ortho_projection(Mp,
					-camera.wi,camera.wi,
					-camera.hi,camera.hi,
					camera.znear,camera.zfar);

		float Mptl[16];
		mat4_multiply(Mptl,Mp,Mtl);

		mat4_assign(camera.mxwvp,Mptl);
		return;
	}

	float Mt[16];
	position Pt=camera.eye;
	vec4_negate(&Pt);

	mat4_set_translation(Mt,&Pt);
//	mat4_set_identity(Mt);

	float Mp[16];

	mat4_set_perpective_projection(Mp,
				-camera.wi,camera.wi,
				-camera.hi,camera.hi,
				camera.znear,camera.zfar);

	float Mpt[16];
	mat4_multiply(Mpt,Mp,Mt);

	mat4_assign(camera.mxwvp,Mpt);
	return;
}
