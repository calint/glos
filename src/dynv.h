#pragma once
#include<stdlib.h>

#define dynv_initial_cap 8
#define dynv_realloc_strategy 'a'
#define dynv_bounds_check 1

typedef struct dynv{
	void* *data;/*owns*//*owns*/
	size_t size;
	size_t cap;
}dynv;
dynv _dynv_init_={NULL,0,0};

// private
inline static void __dynv_insure_free_capcity(dynv*this,size_t n){
	const size_t rem=this->cap-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_cap;
		switch(dynv_realloc_strategy){
		case 'a':
			new_cap=this->cap*2;
			break;
		default:
			fprintf(stderr,"\nunknown-strategy");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}

		printf("   re-allocating vector %p\n",this->data);
		void* *new_data=realloc(this->data,sizeof(void*)*new_cap);
		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=this->data){ // re-locate
			printf("   re-allocated vector %p to %p\n",this->data,new_data);
			this->data=new_data;
		}
		this->cap=new_cap;
		return;
	}
	// initialize data
	this->cap=dynv_initial_cap;
	this->data=malloc(sizeof(void*)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

// public
inline static void dynv_add(dynv*this,void*ptr){
	__dynv_insure_free_capcity(this,1);
	*(this->data+this->size++)=ptr;
}
inline static void*dynv_get(dynv*this,size_t index){
#ifdef dynpvec_bounds_check
	if(index>=this->cap){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	void*p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static void dynv_free(dynv*this){
	if(!this->data)
		return;
	void*p=this->data;
	size_t i=this->size;
	while(i--){
		free(p);
		p++;
	}
	free(this->data);
}

//-----------------------------------------------------------------------------
