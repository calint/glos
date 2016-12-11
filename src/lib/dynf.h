#pragma once
#include<stdio.h>
#include<unistd.h>
//----------------------------------------------------------------------config

#define dynf_initial_capacity 8
#define dynf_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dynf{
	float *data;
	unsigned count;
	unsigned cap;
}dynf;
#define dynf_def (dynf){0,0,0}

//--------------------------------------------------------------------- private

inline static void _dynf_insure_free_capcity(dynf*this,unsigned n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		float *new_data=realloc(this->data,sizeof(float)*new_cap);
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
	this->cap=dynf_initial_capacity;
	this->data=malloc(sizeof(float)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynf_add(dynf*this,float o){
	_dynf_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static float dynf_get(dynf*this,unsigned index){
#ifdef dynf_bounds_check
	if(index>=this->count){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	float p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static float dynf_get_last(dynf*this){
	float p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dynf_size_in_bytes(dynf*this){
	return this->count*sizeof(float);
}

//-----------------------------------------------------------------------------

inline static void dynf_free(dynf*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dynf_add_list(dynf*this,/*copies*/const float*str,size_t n){
	//? optimize memcpy
	const float*p=str;
	while(n--){
		_dynf_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_add_string(dynf*this,/*copies*/const float*str){
	//? optimize
	const float*p=str;
	while(*p){
		_dynf_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_write_to_fd(dynf*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

inline static dynf dynf_from_file(const char*path){
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
	float*filedata=(float*)malloc((size_t)length+1);
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

	return (dynf){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(float),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------
