#pragma once
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"
//------------------------------------------------------------------------ lib

inline static int bits_is_bit_set(bits*b,int bit_number_starting_at_zero){
	return(*b&(1<<bit_number_starting_at_zero))!=0;
}

//----------------------------------------------------------------------------

inline static void bits_set_bit(bits*b,int bit_number_starting_at_zero){
	*b|=(bits)(1<<bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static void bits_clear(bits*b,int bit_number_starting_at_zero){
	*b&=(bits)~(1<<bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static void vec4_increase_with_vec4_over_dt(vec4*this,vec4*other,dt dt){
	this->x+=other->x*dt;
	this->y+=other->y*dt;
	this->z+=other->z*dt;
}

//----------------------------------------------------------------------------

inline static float rnd(){return(float)rand()/(float)RAND_MAX;}

//----------------------------------------------------------------------------

inline static float random_range(float low,float high){
	return (float)rnd()*(high-low)+low;
}

//----------------------------------------------------------------------------

inline static float bounding_radius_for_scale(scale*s){
	return sqrtf(s->x*s->x+s->y*s->y+s->z*s->z);//?
}

//----------------------------------------------------------------------------

void stacktrace_print(){
	void*array[10];
	int size=backtrace(array,10);
	char**strings=backtrace_symbols(array,size);
	fprintf(stderr,"stacktrace %d frames:\n",size);

	for(int i=0;i<size;i++)
		fprintf(stderr,"%s\n",strings[i]);

	free (strings);
}

inline static void vec4_minus(vec4*ret,const vec4*lh,const vec4*rh){
	ret->x=lh->x-rh->x;
	ret->y=lh->y-rh->y;
	ret->z=lh->z-rh->z;
}

inline static void vec4_normalize(vec4*this){
	float len=sqrtf(this->x*this->x + this->y*this->y + this->z*this->z);
	this->x/=len;
	this->y/=len;
	this->z/=len;
}

inline static void vec4_cross(vec4*ret,const vec4*lh,const vec4*rh){
	float u1=lh->x;float u2=lh->y;float u3=lh->z;
	float v1=rh->x;float v2=rh->y;float v3=rh->z;

	ret->x=u2*v3-u3*v2;
	ret->y=u3*v1-u1*v3;
	ret->z=u1*v2-u2*v1;
}

inline static void vec4_negate(vec4*this){
	this->x=-this->x;
	this->y=-this->y;
	this->z=-this->z;
}
