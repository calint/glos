#pragma once
#include<stdio.h>
#include<unistd.h>
#include"../lib/etc.h"
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

inline static void _str_insure_free_capcity(str*o,unsigned n){
	const unsigned rem=o->cap-o->count;
	if(rem>=n)
		return;
	if(o->data){
		unsigned new_cap=o->cap*2;
		char *new_data=realloc(o->data,sizeof(char)*new_cap);
		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=o->data){
			o->data=new_data;
		}
		o->cap=new_cap;
		return;
	}
	o->cap=str_initial_capacity;
	o->data=malloc(sizeof(char)*o->cap);
	if(!o->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void str_add(str*o,char oo){
	_str_insure_free_capcity(o,1);
	*(o->data+o->count++)=oo;
}

//-----------------------------------------------------------------------------

inline static char str_get(str*o,unsigned index){
#ifdef str_bounds_check
	if(index>=o->count){
		fprintf(stderr,"\nindex-out-of-bounds at %s:%u\n",__FILE__,__LINE__);
		fprintf(stderr,"     index: %d  in dynp: %p  size: %u  capacity: %u\n",
				index,(void*)o,o->count,o->cap);
		stacktrace_print();
		exit(-1);
	}
#endif
	char p=*(o->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static char str_get_last(str*o){
	char p=*(o->data+o->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t str_size_in_bytes(str*o){
	return o->count*sizeof(char);
}

//-----------------------------------------------------------------------------

inline static void str_free(str*o){
	if(!o->data)
		return;
	free(o->data);
}

//-----------------------------------------------------------------------------

inline static void str_add_list(str*o,/*copies*/const char*str,size_t n){
	//? optimize memcpy
	const char*p=str;
	while(n--){
		_str_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void str_add_string(str*o,/*copies*/const char*str){
	//? optimize
	const char*p=str;
	while(*p){
		_str_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void str_write_to_fd(str*o,int fd){
	if(!o->data)
		return;
	write(fd,o->data,o->count);
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
