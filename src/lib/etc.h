#pragma once
//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
