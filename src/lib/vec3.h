#pragma once

inline static void vec3_minus(vec4*ret,const vec4*lh,const vec4*rh){
	ret->x=lh->x-rh->x;
	ret->y=lh->y-rh->y;
	ret->z=lh->z-rh->z;
}

inline static void vec3_normalize(vec4*this){
	float len=sqrtf(this->x*this->x + this->y*this->y + this->z*this->z);
	float olen=1/len;
	this->x*=olen;
	this->y*=olen;
	this->z*=olen;
}

inline static void vec3_cross(vec4*ret,const vec4*lh,const vec4*rh){
	float u1=lh->x;float u2=lh->y;float u3=lh->z;
	float v1=rh->x;float v2=rh->y;float v3=rh->z;

	ret->x=u2*v3-u3*v2;
	ret->y=u3*v1-u1*v3;
	ret->z=u1*v2-u2*v1;
}

inline static void vec3_negate(vec4*this){
	this->x=-this->x;
	this->y=-this->y;
	this->z=-this->z;
}

inline static void vec3_scale(vec4*this,float s){
	this->x*=s;
	this->y*=s;
	this->z*=s;
}

inline static float vec3_dot(const vec4*lh,const vec4*rh){
	return lh->x*rh->x+lh->y*rh->y+lh->z*rh->z;

}

inline static void vec3_inc_with_vec3_over_dt(vec4*this,vec4*other,dt dt){
	this->x+=other->x*dt;
	this->y+=other->y*dt;
	this->z+=other->z*dt;
}
