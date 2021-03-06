#pragma once

struct{
	position eye;
	float mxwvp[16];
	position lookat;
//	position*follow_ptr;
	vec4 up;
	float znear,zfar;
	float wi,hi;
	int type;
}camera={
		.eye={0,.5f,30,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=-1,// -.6     1
		.zfar=-2,//-2        2
		.wi=1024,
		.hi=1024,
		.type=1,
};

static float camera_lookangle_y=0;
static float camera_lookangle_x=0;
inline static void camera_update_matrix_wvp(){
	const float oc=15;
	if(camera.type==0){
		mat4 Mt;
//		mat4_set_identity(Mt);
		position Pt=camera.eye;
		vec3_negate(&Pt);
		mat4_set_translation(Mt,&Pt);

		mat4 Ml;
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});

		mat4 Mlt;
		mat4_multiply(Mlt,Ml,Mt);

		mat4 Mp;
		mat4_set_ortho_projection(Mp,-oc,oc, -oc,oc, -oc,oc);

		mat4 Mptl;
		mat4_multiply(Mptl,Mp,Mlt);

		mat4_assign(camera.mxwvp,Mptl);

	}else if(camera.type==1){
//		vec3_print(&camera.lookat);puts("");

		mat4 Mt;
		position Pt=camera.eye;
		vec3_negate(&Pt);
		mat4_set_translation(Mt,&Pt);
//		mat4_set_identity(Mt);

		mat4 Ml;
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});

		mat4 Mtl;
		mat4_multiply(Mtl,Ml,Mt);

		mat4 Mp;
		perspective_vertical(Mp,40,camera.wi/camera.hi,.6f,3);

		mat4 Mplt;
		mat4_multiply(Mplt,Mp,Mtl);

		mat4_assign(camera.mxwvp,Mplt);

	}else if(camera.type==2){
		position Pt=camera.eye;
		vec3_negate(&Pt);

		float Mt[16];
		mat4_set_translation(Mt,&Pt);

		float My[16];
		mat4_set_rotation_y(My,camera_lookangle_y);

		float Myt[16];
		mat4_multiply(Myt,My,Mt);

		float Mp[16];
		perspective_vertical(Mp,20,camera.wi/camera.hi,1,10);

		float Mpyt[16];
		mat4_multiply(Mpyt,Mp,Myt);

		mat4_assign(camera.mxwvp,Mpyt);
	}
}
