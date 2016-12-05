//------------------------------------------------------------------------ dynf
#pragma once

#define dynf_initial_capacity 8
#define dynf_bounds_check 1

//------------------------------------------------------------------------- def

typedef struct dynf{

	float *data;

	size_t size;

	size_t capacity;

}dynf;

dynf dynf_default={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dynf_insure_free_capcity(dynf*this,size_t n){
	const size_t rem=this->capacity-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_cap=this->capacity*2;
		float *new_data=realloc(this->data,sizeof(float)*new_cap);
		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=this->data){
			this->data=new_data;
		}
		this->capacity=new_cap;
		return;
	}
	this->capacity=dynf_initial_capacity;
	this->data=malloc(sizeof(float)*this->capacity);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynf_add(dynf*this,float o){
	_dynf_insure_free_capcity(this,1);
	*(this->data+this->size++)=o;
}

//-----------------------------------------------------------------------------

inline static float dynf_get(dynf*this,size_t index){
#ifdef dynfvec_bounds_check
	if(index>=this->capacity){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->capacity);
		exit(-1);
	}
#endif
	float p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static float dynf_get_last(dynf*this){
	float p=*(this->data+this->size-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static float dynf_size_in_bytes(dynf*this){
	return this->size*sizeof(float);
}

//-----------------------------------------------------------------------------

inline static void dynf_free(dynf*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dynf_add_string(dynf*this,/*copies*/const float*str){
	//? optimize
	const float*p=str;
	while(*p){
		_dynf_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_write_to_fd(dynf*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->size);
}

//-----------------------------------------------------------------------------
