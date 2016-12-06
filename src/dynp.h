#pragma once
#include"typedefs.h"
//----------------------------------------------------------------------config

#define dynp_initial_capacity 8
#define dynp_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dynp{
	void* *data;
	unsigned size;
	unsigned capacity;
}dynp;
dynp dynp_def={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dynp_insure_free_capcity(dynp*this,arrayix n){
	const unsigned rem=this->capacity-this->size;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->capacity*2;
		void* *new_data=realloc(this->data,sizeof(void*)*new_cap);
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
	this->capacity=dynp_initial_capacity;
	this->data=malloc(sizeof(void*)*this->capacity);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynp_add(dynp*this,void* o){
	_dynp_insure_free_capcity(this,1);
	*(this->data+this->size++)=o;
}

//-----------------------------------------------------------------------------

inline static void* dynp_get(dynp*this,arrayix index){
#ifdef dynp_bounds_check
	if(index>=this->capacity){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->capacity);
		exit(-1);
	}
#endif
	void* p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static void* dynp_get_last(dynp*this){
	void* p=*(this->data+this->size-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dynp_size_in_bytes(dynp*this){
	return this->size*sizeof(void*);
}

//-----------------------------------------------------------------------------

inline static void dynp_free(dynp*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dynp_add_list(dynp*this,/*copies*/const void**str,int n){
	//? optimize
	const void**p=str;
	while(n--){
		_dynp_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynp_add_string(dynp*this,/*copies*/const void**str){
	//? optimize
	const void**p=str;
	while(*p){
		_dynp_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynp_write_to_fd(dynp*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->size);
}

//-----------------------------------------------------------------------------

