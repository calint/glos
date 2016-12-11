#pragma once

struct{
	position eye;
	float mxwvp[16];
	position lookat;
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
		.type=2,
};


static float look_angle_y=DEG_TO_RAD(0);
static float look_angle_x=0;
inline static void camera_update_matrix_wvp(){
	const float oc=15;
	if(camera.type==1){
		mat4 Ml;
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});

		mat4 Mt;
//		mat4_set_identity(Mt);
		position Pt=camera.eye;
		vec3_negate(&Pt);
		mat4_set_translation(Mt,&Pt);

		mat4 Mtl;
		mat4_multiply(Mtl,Mt,Ml);

		mat4 Mp;
		mat4_set_ortho_projection(Mp,-oc,oc, -oc,oc, -oc,oc);

		mat4 Mptl;
		mat4_multiply(Mptl,Mp,Mtl);

		mat4_assign(camera.mxwvp,Mptl);
	}else if(camera.type==2){
		float Ml[16];
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});

		position Pt=camera.eye;
		vec3_negate(&Pt);

		float Mt[16];
		mat4_set_translation(Mt,&Pt);

		float Mlt[16];
		mat4_multiply(Mlt,Ml,Mt);

		mat4 Mp;
		perspective_vertical(Mp,20,camera.wi/camera.hi,1,10);

		float Mptl[16];
		mat4_multiply(Mptl,Mp,Mlt);

		mat4_assign(camera.mxwvp,Mptl);
	}else if(camera.type==3){
		mat4 Ml;
		mat4_set_look_at(Ml,&camera.eye,&camera.lookat,&(vec4){0,1,0,0});

		position Pt=camera.eye;
		vec3_negate(&Pt);

		mat4 Mt;
		mat4_set_translation(Mt,&Pt);

		mat4 Mtl;
		mat4_multiply(Mtl,Mt,Ml);

		mat4 Mp;
		perspective_vertical(Mp,20,camera.wi/camera.hi,1,10);

		mat4 Mptl;
		mat4_multiply(Mptl,Mp,Mtl);

		mat4_assign(camera.mxwvp,Mptl);
	}else if(camera.type==0){
		position Pt=camera.eye;
		vec3_negate(&Pt);

		float Mt[16];
		mat4_set_translation(Mt,&Pt);

		float My[16];
		mat4_set_rotation_y(My,look_angle_y);

		float Myt[16];
		mat4_multiply(Myt,My,Mt);

		float Mp[16];
		perspective_vertical(Mp,20,camera.wi/camera.hi,1,10);

		float Mpyt[16];
		mat4_multiply(Mpyt,Mp,Myt);

		mat4_assign(camera.mxwvp,Mpyt);
	}
}
