#pragma once
//-----------------------------------------------------------------------------
// dynf dynamic array generated from template ... do not modify
//-----------------------------------------------------------------------------
#include<stdio.h>
#include<unistd.h>
#include"../lib/stacktrace.h"
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

inline static void _dynf_insure_free_capcity(dynf*o,unsigned n){
	const unsigned rem=o->cap-o->count;
	if(rem>=n)
		return;
	if(o->data){
		unsigned new_cap=o->cap*2;
		float *new_data=realloc(o->data,sizeof(float)*new_cap);
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
	o->cap=dynf_initial_capacity;
	o->data=malloc(sizeof(float)*o->cap);
	if(!o->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dynf_add(dynf*o,float oo){
	_dynf_insure_free_capcity(o,1);
	*(o->data+o->count++)=oo;
}

//-----------------------------------------------------------------------------

inline static float dynf_get(dynf*o,unsigned index){
#ifdef dynf_bounds_check
	if(index>=o->count){
		fprintf(stderr,"\n   index-out-of-bounds at %s:%u\n",__FILE__,__LINE__);
		fprintf(stderr,"        index: %d  in dynp: %p  size: %u  capacity: %u\n\n",
				index,(void*)o,o->count,o->cap);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
#endif
	float p=*(o->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static float dynf_get_last(dynf*o){
	float p=*(o->data+o->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dynf_size_in_bytes(dynf*o){
	return o->count*sizeof(float);
}

//-----------------------------------------------------------------------------

inline static void dynf_free(dynf*o){
	if(!o->data)
		return;
	free(o->data);
}

//-----------------------------------------------------------------------------

inline static void dynf_add_list(dynf*o,/*copies*/const float*str,unsigned n){
	//? optimize memcpy
	const float*p=str;
	while(n--){
		_dynf_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_add_string(dynf*o,/*copies*/const float*str){
	//? optimize
	const float*p=str;
	while(*p){
		_dynf_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dynf_write_to_fd(dynf*o,int fd){
	if(!o->data)
		return;
	write(fd,o->data,o->count);
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
	float*filedata=(float*)malloc((unsigned)length+1);
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

	return (dynf){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(float),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------

inline static void dynf_clear(dynf*o){
	o->count=0;
}

//-----------------------------------------------------------------------------

inline static void dynf_setz(dynf*o,/*copies*/const float*s){
	//? optimize
	const float*p=s;
	o->count=0;
	while(*p){
		_dynf_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
	_dynf_insure_free_capcity(o,1);
	*(o->data+o->count++)=0;
}

//-----------------------------------------------------------------------------
#define dynf_foa(ls,body)dynp_foreach_all(ls,({void __fn__ (float o) body __fn__;}))
#define dynf_foac(ls,body)dynp_foreach_all_count(ls,({void __fn__ (float o,unsigned i) body __fn__;}))
#define dynf_fou(ls,body)dynp_foreach(ls,({int __fn__ (float o) body __fn__;}))
#define dynf_foar(ls,body)dynp_foreach_all_rev(ls,({void __fn__ (float o) body __fn__;}))
//-----------------------------------------------------------------------------
inline static void dynf_foreach(dynf*o,int(*f)(float)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		float oo=o->data[i];
		if(f(oo))
			break;
	}
}
//-----------------------------------------------------------------------------
inline static void dynf_foreach_all(dynf*o,void(*f)(float)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		float oo=o->data[i];
		f(oo);
	}
}
//-----------------------------------------------------------------------------
inline static void dynf_foreach_all_count(dynf*o,void(*f)(float,unsigned)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		float oo=o->data[i];
		f(oo,i);
	}
}
//-----------------------------------------------------------------------------
inline static void dynf_foreach_all_rev(dynf*o,void(*f)(float)){
	if(!o->count)
		return;
	for(int i=(signed)o->count-1;i!=0;i--){
		float oo=o->data[i];
		f(oo);
	}
}
//-----------------------------------------------------------------------------
// returns count if not found otherwise index
inline static unsigned dynf_find_index(dynf*o,float oo){
	for(unsigned i=0;i<o->count;i++){
		if(dynf_get(o,i)==oo)
			return i;
	}
	return o->count;
}
//-----------------------------------------------------------------------------
inline static unsigned dynf_has(dynf*o,float oo){
	const unsigned i=dynf_find_index(o,oo);
	if(i==o->count)
		return 0;
	return 1;
}
//-----------------------------------------------------------------------------
