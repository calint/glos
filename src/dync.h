//------------------------------------------------------------------ dync
#pragma once
#include<stdlib.h>

#define dync_initial_cap 8
#define dync_realloc_strategy 'a'
#define dync_bounds_check 1

typedef struct dync{
	char*data;/*owns*/
	size_t size;
	size_t cap;
}dync;
dync _dync_init_={NULL,0,0};

//--------------------------------------------------------------------- private

inline static void __dync_insure_free_capcity(dync*this,size_t n){
	const size_t rem=this->cap-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_cap;
		switch(dync_realloc_strategy){
		case 'a':
			new_cap=this->cap*2;
			break;
		default:
			fprintf(stderr,"\nunknown-strategy");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}

		printf("   re-allocating vector %p\n",this->data);
		void* *new_data=realloc(this->data,sizeof(float)*new_cap);
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
	this->cap=dync_initial_cap;
	this->data=malloc(sizeof(float)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}


//---------------------------------------------------------------------- public

inline static void dync_add(dync*this,float f){
	__dync_insure_free_capcity(this,1);
	*(this->data+this->size++)=f;
}

//-----------------------------------------------------------------------------

inline static float dync_get(dync*this,size_t index){
#ifdef dync_bounds_check
	if(index>=this->cap){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	return*(this->data+index);
}

//-----------------------------------------------------------------------------

inline static size_t dync_size_in_bytes(dync*this){
	return this->size*sizeof(float);
}

//-----------------------------------------------------------------------------

inline static void dync_free(dync*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------
