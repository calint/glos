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
		.eye={0,0,1,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=-.6,
		.zfar=-2,
		.wi=2,
		.hi=2,
		.ortho=0,
};
inline static void camera_update_matrix_wvp(){
	float Ml[16];
	if(camera.ortho){
		mat4_set_look_at(Ml,&camera.eye,&(vec4){0,0,0,0},&(vec4){0,1,0,0});
	}else{
		mat4_set_look_at(Ml,&camera.eye,&(vec4){0,0,0,0},&(vec4){0,1,0,0});
//		mat4_set_identity(Ml);
	}

	float Mt[16];
	position Pt=camera.eye;
	vec4_negate(&Pt);
	mat4_set_translation(Mt,&Pt);

	float Mtl[16];
	mat4_multiply(Mtl,Mt,Ml);

	float Mp[16];
	if(camera.ortho){
		mat4_set_ortho_projection(Mp,
					-camera.wi,camera.wi,
					-camera.hi,camera.hi,
					1,-2);
	}else{
		mat4_set_perpective_projection(Mp,
					camera.znear,camera.zfar,
					PI/4,1);
	}

	float Mptl[16];
	mat4_multiply(Mptl,Mp,Mtl);

	mat4_assign(camera.mxwvp,Mptl);
}
