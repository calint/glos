#pragma once
#include<stdlib.h>

#define dynpvec_initial_cap 10000
#define dynv_realloc_strategy 'a'
#define dynpvec_bounds_check 1

typedef void* array_of_voids;
typedef void** ptr_to_array_of_voids;


typedef struct dynv{
	void* *data;
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
	this->cap=dynpvec_initial_cap;
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

inline static void*dynv_get_last(dynv*this){
	void*p=*(this->data+this->size-1);
	return p;
}

//inline static void dynpvec_add_all(dynpvec*this){}
//inline static void dynpvec_remove_ptr(dynpvec*this,void*ptr){}
//inline static void dynpvec_remove_at_index(dynpvec*this,size_t index){}
//inline static void dynpvec_insert_before(dynpvec*this,void*ptr){}
//inline static void dynpvec_insert_after(dynpvec*this,void*ptr){}
//inline static void dynpvec_insert_before(dynpvec*this,size_t index){}
//inline static void dynpvec_insert_after(dynpvec*this,size_t index){}




//while(argc--)puts(*argv++);
//{
//	dynpvec v=_dynpvec_init_;
//	int a=1,b=2,c=3;
//	dynpvec_add(&v,&a);
//	dynpvec_add(&v,&b);
//	dynpvec_add(&v,&c);
//
//	printf(" %d  %d   %d\n",
//			*((int*)dynpvec_get(&v,0)),
//			*((int*)dynpvec_get(&v,1)),
//			*((int*)dynpvec_get(&v,2))
//		);
//
//	int*p=(int*)dynpvec_get(&v,0);
//	if(*p!=a){exit(-1);}
//	if(p!=&a){exit(-1);}
//	p=dynpvec_get(&v,2);
//	if(p!=&c){exit(-1);}
//	puts("ok");
//	exit(0);
//}

