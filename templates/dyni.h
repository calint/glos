//------------------------------------------------------------------------ dyni
#pragma once

#define dyni_initial_capacity 8
#define dyni_bounds_check 1

//------------------------------------------------------------------------- def

typedef struct dyni{

	int *data;

	size_t size;

	size_t capacity;

}dyni;

dyni dyni_default={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dyni_insure_free_capcity(dyni*this,size_t n){
	const size_t rem=this->capacity-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_cap=this->capacity*2;
		int *new_data=realloc(this->data,sizeof(int)*new_cap);
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
	this->capacity=dyni_initial_capacity;
	this->data=malloc(sizeof(int)*this->capacity);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dyni_add(dyni*this,int o){
	_dyni_insure_free_capcity(this,1);
	*(this->data+this->size++)=o;
}

//-----------------------------------------------------------------------------

inline static int dyni_get(dyni*this,size_t index){
#ifdef dynivec_bounds_check
	if(index>=this->capacity){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->capacity);
		exit(-1);
	}
#endif
	int p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static int dyni_get_last(dyni*this){
	int p=*(this->data+this->size-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static int dyni_size_in_bytes(dyni*this){
	return this->size*sizeof(int);
}

//-----------------------------------------------------------------------------

inline static void dyni_free(dyni*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dyni_add_string(dyni*this,/*copies*/const int*str){
	//? optimize
	int*p=str;
	while(*p){
		__dyni_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dyni_write_to_fd(dyni*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->size);
}

//-----------------------------------------------------------------------------
