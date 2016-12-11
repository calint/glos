#pragma once
#include<stdio.h>
//----------------------------------------------------------------------config

#define dynp_initial_capacity 8
#define dynp_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dynp{
	void* *data;
	unsigned count;
	unsigned cap;
}dynp;
#define dynp_def (dynp){0,0,0}

//--------------------------------------------------------------------- private

inline static void _dynp_insure_free_capcity(dynp*this,unsigned n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		void* *new_data=realloc(this->data,sizeof(void*)*new_cap);
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
	this->cap=dynp_initial_capacity;
	this->data=malloc(sizeof(void*)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynp_add(dynp*this,void* o){
	_dynp_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static void* dynp_get(dynp*this,unsigned index){
#ifdef dynp_bounds_check
	if(index>=this->count){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	void* p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static void* dynp_get_last(dynp*this){
	void* p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dynp_size_in_bytes(dynp*this){
	return this->count*sizeof(void*);
}

//-----------------------------------------------------------------------------

inline static void dynp_free(dynp*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dynp_add_list(dynp*this,/*copies*/const void**str,size_t n){
	//? optimize memcpy
	const void**p=str;
	while(n--){
		_dynp_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynp_add_string(dynp*this,/*copies*/const void**str){
	//? optimize
	const void**p=str;
	while(*p){
		_dynp_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynp_write_to_fd(dynp*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

inline static dynp dynp_from_file(const char*path){
	FILE*f=fopen(path,"rb");
	if(!f){
		perror("\ncannot open");
		fprintf(stderr,"\t%s\n\n%s %d\n",path,__FILE__,__LINE__);
		exit(-1);
	}
	long sk=fseek(f,0,SEEK_END);
	if(sk<0){
		fprintf(stderr,"\nwhile fseek\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	long length=ftell(f);
	if(length<0){
		fprintf(stderr,"\nwhile ftell\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	rewind(f);
	void**filedata=(void**)malloc((size_t)length+1);
	if(!filedata){
		fprintf(stderr,"\nout-of-memory\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	size_t n=fread(filedata,1,(size_t)length+1,f);
	if(n!=(size_t)length){
		fprintf(stderr,"\nnot-a-full-read\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	fclose(f);
	filedata[length]=0;

	return (dynp){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(void*),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------
