#pragma once
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
	return (float)rand()*(high-low)+low;
}

//----------------------------------------------------------------------------

inline static float bounding_radius_for_scale(scale*s){
	return sqrtf(s->x*s->x+s->y*s->y+s->z*s->z);//?
}

//----------------------------------------------------------------------------

