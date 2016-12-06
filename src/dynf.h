#pragma once
#include"typedefs.h"
//----------------------------------------------------------------------config

#define dynf_initial_capacity 8
#define dynf_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dynf{
	float *data;
	unsigned count;
	unsigned cap;
}dynf;
dynf dynf_def={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dynf_insure_free_capcity(dynf*this,arrayix n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		float *new_data=realloc(this->data,sizeof(float)*new_cap);
		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=this->data){
			this->data=new_data;
		}
		this->cap=new_cap;
		return;
	}
	this->cap=dynf_initial_capacity;
	this->data=malloc(sizeof(float)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynf_add(dynf*this,float o){
	_dynf_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static float dynf_get(dynf*this,arrayix index){
#ifdef dynf_bounds_check
	if(index>=this->cap){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	float p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static float dynf_get_last(dynf*this){
	float p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dynf_size_in_bytes(dynf*this){
	return this->count*sizeof(float);
}

//-----------------------------------------------------------------------------

inline static void dynf_free(dynf*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dynf_add_list(dynf*this,/*copies*/const float*str,int n){
	//? optimize
	const float*p=str;
	while(n--){
		_dynf_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_add_string(dynf*this,/*copies*/const float*str){
	//? optimize
	const float*p=str;
	while(*p){
		_dynf_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_write_to_fd(dynf*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

