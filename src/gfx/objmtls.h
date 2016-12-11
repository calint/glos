#pragma once
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
		objmtl* *new_data=realloc(o->data,sizeof(objmtl*)*new_cap);
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
	o->data=malloc(sizeof(objmtl*)*o->cap);
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

inline static void objmtls_add_list(objmtls*o,/*copies*/const objmtl**str,size_t n){
	//? optimize memcpy
	const objmtl**p=str;
	while(n--){
		_objmtls_insure_free_capcity(o,1);
		*(o->data+o->count++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void objmtls_add_string(objmtls*o,/*copies*/const objmtl**str){
	//? optimize
	const objmtl**p=str;
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
	objmtl**filedata=(objmtl**)malloc((size_t)length+1);
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

	return (objmtls){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(objmtl*),
		.cap=(unsigned)length+1
	};
}

//-----------------------------------------------------------------------------
