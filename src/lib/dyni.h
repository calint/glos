#pragma once
//-----------------------------------------------------------------------------
// dyni dynamic array generated from template ... do not modify
//-----------------------------------------------------------------------------
#include<stdio.h>
#include<unistd.h>
#include"../lib/stacktrace.h"
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

inline static void _dyni_insure_free_capcity(dyni*o,unsigned n){
	const unsigned rem=o->cap-o->count;
	if(rem>=n)
		return;
	if(o->data){
		unsigned new_cap=o->cap*2;
		int *new_data=realloc(o->data,sizeof(int)*new_cap);
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
	o->cap=dyni_initial_capacity;
	o->data=malloc(sizeof(int)*o->cap);
	if(!o->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dyni_add(dyni*o,int oo){
	_dyni_insure_free_capcity(o,1);
	*(o->data+o->count++)=oo;
}

//-----------------------------------------------------------------------------

inline static int dyni_get(dyni*o,unsigned index){
#ifdef dyni_bounds_check
	if(index>=o->count){
		fprintf(stderr,"\n   index-out-of-bounds at %s:%u\n",__FILE__,__LINE__);
		fprintf(stderr,"        index: %d  in dynp: %p  size: %u  capacity: %u\n\n",
				index,(void*)o,o->count,o->cap);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
#endif
	int p=*(o->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static int dyni_get_last(dyni*o){
	int p=*(o->data+o->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dyni_size_in_bytes(dyni*o){
	return o->count*sizeof(int);
}

//-----------------------------------------------------------------------------

inline static void dyni_free(dyni*o){
	if(!o->data)
		return;
	free(o->data);
}

//-----------------------------------------------------------------------------

inline static void dyni_add_list(dyni*o,/*copies*/const int*str,unsigned n){
	//? optimize memcpy
	const int*p=str;
	while(n--){
		_dyni_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dyni_add_string(dyni*o,/*copies*/const int*str){
	//? optimize
	const int*p=str;
	while(*p){
		_dyni_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dyni_write_to_fd(dyni*o,int fd){
	if(!o->data)
		return;
	write(fd,o->data,o->count);
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
	int*filedata=(int*)malloc((unsigned)length+1);
	if(!filedata){
		fprintf(stderr,"\nout-of-memory\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	const size_t n=fread(filedata,1,(unsigned)length+1,f);
	if(n!=(unsigned)length){
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

inline static void dyni_clear(dyni*o){
	o->count=0;
}

//-----------------------------------------------------------------------------

inline static void dyni_setz(dyni*o,/*copies*/const int*s){
	//? optimize
	const int*p=s;
	o->count=0;
	while(*p){
		_dyni_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
	_dyni_insure_free_capcity(o,1);
	*(o->data+o->count++)=0;
}

//-----------------------------------------------------------------------------
#define dyni_foo(ls,body)dynp_foreach_all(ls,({void __fn__ (int o) body __fn__;}))
#define dyni_foa(ls,body)dynp_foreach_all(ls,({int __fn__ (int o) body __fn__;}))
//-----------------------------------------------------------------------------
inline static void dyni_foreach(dyni*o,int(*f)(int)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		int oo=o->data[i];
		if(f(oo))
			break;
	}
}
//-----------------------------------------------------------------------------
inline static void dyni_foreach_all(dyni*o,void(*f)(int)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		int oo=o->data[i];
		f(oo);
	}
}
//-----------------------------------------------------------------------------
