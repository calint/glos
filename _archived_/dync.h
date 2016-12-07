#pragma once
#include"typedefs.h"
//----------------------------------------------------------------------config

#define dync_initial_capacity 8
#define dync_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dync{
	char *data;
	unsigned count;
	unsigned cap;
}dync;
dync dync_def={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dync_insure_free_capcity(dync*this,indx n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		char *new_data=realloc(this->data,sizeof(char)*new_cap);
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
	this->cap=dync_initial_capacity;
	this->data=malloc(sizeof(char)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dync_add(dync*this,char o){
	_dync_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static char dync_get(dync*this,indx index){
#ifdef dync_bounds_check
	if(index>=this->cap){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	char p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static char dync_get_last(dync*this){
	char p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dync_size_in_bytes(dync*this){
	return this->count*sizeof(char);
}

//-----------------------------------------------------------------------------

inline static void dync_free(dync*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dync_add_list(dync*this,/*copies*/const char*str,size_t n){
	//? optimize memcpy
	const char*p=str;
	while(n--){
		_dync_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dync_add_string(dync*this,/*copies*/const char*str){
	//? optimize
	const char*p=str;
	while(*p){
		_dync_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dync_write_to_fd(dync*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

