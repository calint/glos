#pragma once
#include<math.h>

inline static void vec3_minus(vec4*ret,const vec4*lh,const vec4*rh){
	ret->x=lh->x-rh->x;
	ret->y=lh->y-rh->y;
	ret->z=lh->z-rh->z;
}

inline static void vec3_normalize(vec4*o){
	float len=sqrtf(o->x*o->x + o->y*o->y + o->z*o->z);
	float olen=1/len;
	o->x*=olen;
	o->y*=olen;
	o->z*=olen;
}

inline static void vec3_cross(vec4*ret,const vec4*lh,const vec4*rh){
	float u1=lh->x;float u2=lh->y;float u3=lh->z;
	float v1=rh->x;float v2=rh->y;float v3=rh->z;

	ret->x=u2*v3-u3*v2;
	ret->y=u3*v1-u1*v3;
	ret->z=u1*v2-u2*v1;
}

inline static void vec3_negate(vec4*o){
	o->x=-o->x;
	o->y=-o->y;
	o->z=-o->z;
}

inline static void vec3_scale(vec4*o,float s){
	o->x*=s;
	o->y*=s;
	o->z*=s;
}

inline static float vec3_dot(const vec4*lh,const vec4*rh){
	return lh->x*rh->x+lh->y*rh->y+lh->z*rh->z;

}

inline static void vec3_inc_with_vec3_over_dt(vec4*o,vec4*other,dt dt){
	o->x+=other->x*dt;
	o->y+=other->y*dt;
	o->z+=other->z*dt;
}

inline static void vec3_print(vec4*o){
	printf(" | %f  %f  %f |",o->x,o->y,o->z);
}

inline static bool vec3_equals(vec4*o,vec4*oo){
	return o->x==oo->x && o->y==oo->y && o->z==oo->z;
}
