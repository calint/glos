#pragma once
#include<stdio.h>
#include<unistd.h>
//----------------------------------------------------------------------config

#define str_initial_capacity 8
#define str_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct str{
	char *data;
	unsigned count;
	unsigned cap;
}str;
#define str_def (str){0,0,0}

//--------------------------------------------------------------------- private

inline static void _str_insure_free_capcity(str*this,unsigned n){
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
	this->cap=str_initial_capacity;
	this->data=malloc(sizeof(char)*this->cap);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void str_add(str*this,char o){
	_str_insure_free_capcity(this,1);
	*(this->data+this->count++)=o;
}

//-----------------------------------------------------------------------------

inline static char str_get(str*this,unsigned index){
#ifdef str_bounds_check
	if(index>=this->count){
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

inline static char str_get_last(str*this){
	char p=*(this->data+this->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t str_size_in_bytes(str*this){
	return this->count*sizeof(char);
}

//-----------------------------------------------------------------------------

inline static void str_free(str*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void str_add_list(str*this,/*copies*/const char*str,size_t n){
	//? optimize memcpy
	const char*p=str;
	while(n--){
		_str_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void str_add_string(str*this,/*copies*/const char*str){
	//? optimize
	const char*p=str;
	while(*p){
		_str_insure_free_capcity(this,1);
		*(this->data+this->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void str_write_to_fd(str*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->count);
}

//-----------------------------------------------------------------------------

inline static str str_from_file(const char*path){
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
	char*filedata=(char*)malloc((size_t)length+1);
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

	return (str){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(char),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------
