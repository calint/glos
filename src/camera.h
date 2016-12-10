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
		.znear=-1,// -.6     1
		.zfar=-2,//-2        2
		.wi=1024,
		.hi=1024,
		.ortho=0,
};
inline static void camera_update_matrix_wvp(){
	float Ml[16];
	if(camera.ortho){
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});
	}else{
//		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});
		mat4_set_identity(Ml);
	}

	position Pt=camera.eye;
	vec4_negate(&Pt);

	float Mt[16];
	mat4_set_translation(Mt,&Pt);

	float Mtl[16];
	mat4_multiply(Mtl,Mt,Ml);

//	mat4_set_identity(Mtl);

	float Mp[16];
	if(camera.ortho){
		mat4_set_ortho_projection(Mp,
					-2,2,
					-2,2,
					-2,2);
	}else{
//		mat4_set_identity(Mp);
//		mat4_set_perpective_projection(Mp,
//					camera.znear,camera.zfar,
//					PI/4,camera.wi/camera.hi);
//		mat4_set_perpective_projection2(Mp,
//				1024,1024,.1f,5);
//		gluPerspective(Mp,PI/4, camera.wi/camera.hi,.01f,5);

//		glFrustum(Mp,-2,2, -2,2, 1,10);
//		perspective(Mp, -2,2, -2,2, 1, 10);
		perspective_vertical(Mp,DEG_TO_RAD(40),camera.wi/camera.hi,1,10);
	}

	float Mptl[16];
	mat4_multiply(Mptl,Mp,Mtl);

	mat4_assign(camera.mxwvp,Mptl);
}
