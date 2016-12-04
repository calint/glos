#pragma once
#include<stdlib.h>

#define dynpvec_initial_cap 2
#define dynpvec_realloc_strategy 'a'
#define dynpvec_bounds_check 1

typedef void* array_of_voids;
typedef void** ptr_to_array_of_voids;


typedef struct dynpvec{
	void* *data;
	size_t size;
	size_t cap;
}dynpvec;
dynpvec _dynpvec_init_={NULL,0,0};

// private
inline static void __dynpvec_insure_free_capcity(dynpvec*this,size_t n){
	const size_t rem=this->cap-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_size;
		switch(dynpvec_realloc_strategy){
		case 'a':
			new_size=this->cap*2;
			break;
		default:
			fprintf(stderr,"\nunknown-strategy");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}

//		void* *new_data=realloc(this->data,sizeof(void*)*new_size);
		void* *new_data=malloc(sizeof(void*)*new_size);

		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=this->data){ // re-locate
			memcpy(new_data,this->data,this->size*sizeof(void*));
			this->data=new_data;
		}
		this->cap=new_size;
		// realloc
		return;
	}
	// initialize data
	this->cap=dynpvec_initial_cap;
	this->data=malloc(sizeof(void*)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

// public
inline static void dynpvec_add(dynpvec*this,void*ptr){
	__dynpvec_insure_free_capcity(this,1);
	*(this->data+this->size++)=ptr;
}
inline static void*dynpvec_get(dynpvec*this,size_t index){
#ifdef dynpvec_bounds_check
	if(index>=this->cap){
		perror("\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	void*p=*(this->data+index);
	return p;
}

//inline static void dynpvec_add_all(dynpvec*this){}
//inline static void dynpvec_remove_ptr(dynpvec*this,void*ptr){}
//inline static void dynpvec_remove_at_index(dynpvec*this,size_t index){}
//inline static void dynpvec_insert_before(dynpvec*this,void*ptr){}
//inline static void dynpvec_insert_after(dynpvec*this,void*ptr){}
//inline static void dynpvec_insert_before(dynpvec*this,size_t index){}
//inline static void dynpvec_insert_after(dynpvec*this,size_t index){}
