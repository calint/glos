#pragma once
//-----------------------------------------------------------------------------
// objmtls dynamic array generated from template ... do not modify
//-----------------------------------------------------------------------------
#include<stdio.h>
#include<unistd.h>
#include"../lib/stacktrace.h"
//----------------------------------------------------------------------config

#define objmtls_initial_capacity 8
#define objmtls_bounds_check 1

//------------------------------------------------------------------------ def

typedef struct objmtls{
	objmtl* *data;
	unsigned count;
	unsigned cap;
}objmtls;
#define objmtls_def (objmtls){0,0,0}

//--------------------------------------------------------------------- private

inline static void _objmtls_insure_free_capcity(objmtls*o,unsigned n){
	const unsigned rem=o->cap-o->count;
	if(rem>=n)
		return;
	if(o->data){
		unsigned new_cap=o->cap*2;
		objmtl* *new_data=(objmtl**)realloc(o->data,sizeof(objmtl*)*new_cap);
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
	o->cap=objmtls_initial_capacity;
	o->data=(objmtl**)malloc(sizeof(objmtl*)*o->cap);
	if(!o->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void objmtls_add(objmtls*o,objmtl* oo){
	_objmtls_insure_free_capcity(o,1);
	*(o->data+o->count++)=oo;
}

//-----------------------------------------------------------------------------

inline static objmtl* objmtls_get(objmtls*o,unsigned index){
#ifdef objmtls_bounds_check
	if(index>=o->count){
		fprintf(stderr,"\n   index-out-of-bounds at %s:%u\n",__FILE__,__LINE__);
		fprintf(stderr,"        index: %d  in dynp: %p  size: %u  capacity: %u\n\n",
				index,(void*)o,o->count,o->cap);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}
#endif
	objmtl* p=*(o->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static objmtl* objmtls_get_last(objmtls*o){
	objmtl* p=*(o->data+o->count-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t objmtls_size_in_bytes(objmtls*o){
	return o->count*sizeof(objmtl*);
}

//-----------------------------------------------------------------------------

inline static void objmtls_free(objmtls*o){
	if(!o->data)
		return;
	free(o->data);
}

//-----------------------------------------------------------------------------

inline static void objmtls_add_list(objmtls*o,/*copies*/objmtl* const*str,unsigned n){
	//? optimize memcpy
	objmtl* const*p=str;
	while(n--){
		_objmtls_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void objmtls_add_string(objmtls*o,/*copies*/objmtl* const*str){
	//? optimize
	objmtl* const*p=str;
	while(*p){
		_objmtls_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void objmtls_write_to_fd(objmtls*o,int fd){
	if(!o->data)
		return;
	write(fd,o->data,o->count);
}

//-----------------------------------------------------------------------------

inline static objmtls objmtls_from_file(const char*path){
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
	objmtl**filedata=(objmtl**)malloc((unsigned)length+1);
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

	return (objmtls){
		.data=filedata,
		.count=(unsigned)((length+1)/sizeof(objmtl*)),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------

inline static void objmtls_clear(objmtls*o){
	o->count=0;
}

//-----------------------------------------------------------------------------

inline static void objmtls_setz(objmtls*o,/*copies*/objmtl* const*s){
	//? optimize
	objmtl* const*p=s;
	o->count=0;
	while(*p){
		_objmtls_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
	_objmtls_insure_free_capcity(o,1);
	*(o->data+o->count++)=0;
}

//-----------------------------------------------------------------------------
#define objmtls_foa(ls,body)dynp_foreach_all(ls,({void __fn__ (objmtl* o) body __fn__;}))
#define objmtls_foac(ls,body)dynp_foreach_all_count(ls,({void __fn__ (objmtl* o,unsigned i) body __fn__;}))
#define objmtls_fou(ls,body)dynp_foreach(ls,({int __fn__ (objmtl* o) body __fn__;}))
#define objmtls_foar(ls,body)dynp_foreach_all_rev(ls,({void __fn__ (objmtl* o) body __fn__;}))
//-----------------------------------------------------------------------------
inline static void objmtls_foreach(objmtls*o,int(*f)(objmtl*)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		objmtl* oo=o->data[i];
		if(f(oo))
			break;
	}
}
//-----------------------------------------------------------------------------
inline static void objmtls_foreach_all(objmtls*o,void(*f)(objmtl*)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		objmtl* oo=o->data[i];
		f(oo);
	}
}
//-----------------------------------------------------------------------------
inline static void objmtls_foreach_all_count(objmtls*o,void(*f)(objmtl*,unsigned)){
	if(!o->count)
		return;
	for(unsigned i=0;i<o->count;i++){
		objmtl* oo=o->data[i];
		f(oo,i);
	}
}
//-----------------------------------------------------------------------------
inline static void objmtls_foreach_all_rev(objmtls*o,void(*f)(objmtl*)){
	if(!o->count)
		return;
	for(int i=(signed)o->count-1;i!=0;i--){
		objmtl* oo=o->data[i];
		f(oo);
	}
}
//-----------------------------------------------------------------------------
// returns count if not found otherwise index
inline static unsigned objmtls_find_index(objmtls*o,objmtl* oo){
	for(unsigned i=0;i<o->count;i++){
		if(objmtls_get(o,i)==oo)
			return i;
	}
	return o->count;
}
//-----------------------------------------------------------------------------
inline static unsigned objmtls_has(objmtls*o,objmtl* oo){
	const unsigned i=objmtls_find_index(o,oo);
	if(i==o->count)
		return 0;
	return 1;
}
//-----------------------------------------------------------------------------
inline static/*gives*/objmtls objmtls_from_string(objmtl* const*s){
	objmtls o=objmtls_def;
	objmtls_add_string(&o,s);
	objmtls_add(&o,0);
	return o;
}
//-----------------------------------------------------------------------------
