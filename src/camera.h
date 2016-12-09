#pragma once

struct{
	position eye;
	float mxwvp[16];
	position lookat;
	vec4 up;
	float znear,zfar;
	float wi,hi;
}camera={
		.eye={0,0,0,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=-10000,
		.zfar=100000,
		.wi=2,
		.hi=2,
};
inline static void camera_update_matrix_wvp(){
	float Mt[16];
	position Pt={0,0,-1,0};
	mat4_set_translation(Mt,&Pt);
	mat4_assign(camera.mxwvp,Mt);

	float Mp[16];
	mat4_set_identity(Mp);
	mat4_set_ortho_projection(Mp,-camera.wi,camera.wi,
			-camera.hi,camera.hi, camera.znear,camera.zfar);


	float Mtp[16];
	mat4_multiply(Mtp,Mp,Mt);


	mat4_assign(camera.mxwvp,Mtp);


//	mat4_assign(camera.mxwvp);
//	vec4_increase_with_vec4_over_dt(&camera.eye,&(vec4){-1,0,0,0},
//			metrics.previous_frame_dt);

//		float mtx_lookat[16];
//		mat4_set_look_at(mtx_lookat,&camera.eye,&camera.lookat,&camera.up);
//		memcpy(camera.mxwvp,mtx_lookat,4*4*sizeof(float));
//		return;
//
//	vec4 t=camera.eye;
//	vec4_negate(&t);
//	float mtx_trans[16];
//	mat4_set_translation(camera.mxwvp,&t);
//	return;
//
//	float mtx_proj[16];
//	mat4_set_ortho_projection(mtx_proj,-camera.wi,camera.wi,
//			-camera.hi,camera.hi, camera.znear,camera.zfar);
//
//	mat4_multiply(camera.mxwvp, mtx_proj, mtx_trans);
}
