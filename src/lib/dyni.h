#pragma once
#include<stdio.h>
#include<unistd.h>
//----------------------------------------------------------------------config

#define dyni_initial_capacity 8
#define dyni_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct dyni{
	int *data;
	unsigned count;
	unsigned cap;
}dyni;
#define dyni_def (dyni){0,0,0}

//--------------------------------------------------------------------- private

inline static void _dyni_insure_free_capcity(dyni*this,unsigned n){
	const unsigned rem=this->cap-this->count;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->cap*2;
		int *new_data=realloc(this->data,sizeof(int)*new_cap);
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
	this->cap=dyni_initial_capacity;
	this->data=malloc(sizeof(int)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dyni_add(dyni*this,int o){
	_dyni_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static int dyni_get(dyni*this,unsigned index){
#ifdef dyni_bounds_check
	if(index>=this->count){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %u    capacity: %u\n",
				__FILE__,__LINE__,index,this->cap);
		exit(-1);
	}
#endif
	int p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static int dyni_get_last(dyni*this){
	int p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dyni_size_in_bytes(dyni*this){
	return this->count*sizeof(int);
}

//-----------------------------------------------------------------------------

inline static void dyni_free(dyni*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dyni_add_list(dyni*this,/*copies*/const int*str,size_t n){
	//? optimize memcpy
	const int*p=str;
	while(n--){
		_dyni_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dyni_add_string(dyni*this,/*copies*/const int*str){
	//? optimize
	const int*p=str;
	while(*p){
		_dyni_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dyni_write_to_fd(dyni*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

inline static dyni dyni_from_file(const char*path){
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
	int*filedata=(int*)malloc((size_t)length+1);
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

	return (dyni){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(int),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------
