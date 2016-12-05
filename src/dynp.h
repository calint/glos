//------------------------------------------------------------------------ dynp
#pragma once
#define dynp_initial_capacity 8
#define dynp_bounds_check 1

//------------------------------------------------------------------------- def

typedef struct dynp{

	void* *data;

	size_t size;

	size_t capacity;

}dynp;
dynp _dynp_init_={NULL,0,0};

//--------------------------------------------------------------------- private

inline static void __dynp_insure_free_capcity(dynp*this,size_t n){
	const size_t rem=this->capacity-this->size;
	if(rem>=n)
		return;
	if(this->data){
		size_t new_cap=this->capacity*2;
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
inline static void dynp_add(dynp*this,void*ptr){
	__dynp_insure_free_capcity(this,1);
	*(this->data+this->size++)=ptr;
}

//-----------------------------------------------------------------------------

inline static void*dynp_get(dynp*this,size_t index){
#ifdef dynpvec_bounds_check
	if(index>=this->capacity){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->capacity);
		exit(-1);
	}
#endif
	void*p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static void*dynp_get_last(dynp*this){
	void*p=*(this->data+this->size-1);
	return p;
}

//-----------------------------------------------------------------------------
