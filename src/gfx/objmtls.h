#pragma once
#include"../lib.h"
//----------------------------------------------------------------------config

#define objmtls_initial_capacity 128
#define objmtls_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct objmtls{
	objmtl* *data;
	unsigned count;
	unsigned cap;
}objmtls;
#define objmtls_def (objmtls){0,0,0}

//--------------------------------------------------------------------- private

inline static void _objmtls_insure_free_capcity(objmtls*this,unsigned n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		objmtl* *new_data=realloc(this->data,sizeof(objmtl*)*new_cap);
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
	this->cap=objmtls_initial_capacity;
	this->data=malloc(sizeof(objmtl*)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void objmtls_add(objmtls*this,objmtl* o){
	_objmtls_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static objmtl* objmtls_get(objmtls*this,unsigned index){
#ifdef objmtls_bounds_check
	if(index>=this->cap){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	objmtl* p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static objmtl* objmtls_get_last(objmtls*this){
	objmtl* p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t objmtls_size_in_bytes(objmtls*this){
	return this->count*sizeof(objmtl*);
}

//-----------------------------------------------------------------------------

inline static void objmtls_free(objmtls*this){
	if(!this->data)
		return;

	objmtl*it=*this->data;
	unsigned i=this->count;
	while(i--)
		free(it++);

	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void objmtls_add_list(objmtls*this,/*copies*/const objmtl**str,int n){
	//? optimize
	const objmtl**p=str;
	while(n--){
		_objmtls_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void objmtls_add_string(objmtls*this,/*copies*/const objmtl**str){
	//? optimize
	const objmtl**p=str;
	while(*p){
		_objmtls_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void objmtls_write_to_fd(objmtls*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

