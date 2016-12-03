#pragma once
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include"obj_o.h"
// ----------------------------------------------------------------------- str

typedef struct str{
	char*ptr;
	size_t size;
}str;

// ---------------------------------------------------------------------- init

inline static void str_initfromfile(str*this,const char*filepath){

	FILE*f=fopen(filepath,"rb");

	fseek(f,0,SEEK_END);

	long s=ftell(f);
	if(s<0){
		perror("error while getting file size");
		exit(-1);
	}

	this->size=(unsigned)s;

	fseek(f,0,SEEK_SET);

	this->ptr=malloc(this->size);

	fread(this->ptr,this->size,1,f);

	fclose(f);

}

// --------------------------------------------------------------------- write

inline static void str_write(str*this,int fd){
	write(fd,this->ptr,this->size);
}

// ---------------------------------------------------------------------- free

inline static void str_free(str*this){
	//	if(!this->ptr);//?
	free(this->ptr);
}

// ---------------------------------------------------------------------------

typedef struct span{
	char*begin;
	char*end;
}span;

typedef struct token{
	char*begin;
	char*content;
	char*content_end;
	char*end;
}token;

inline static void write_token(token*this,int fd){
	write(fd,this->begin,(size_t)(this->end-this->begin));
}

inline static void foreach_token_in_str(
		str*s,
		void(*f)(token*,void*),
		void*context
){
	char*p=s->ptr;
	size_t n=s->size;
	token t;
	t.begin=s->ptr;
	while(1){
		if(!n)break;
		if(!isspace(*p))break;
		p++;
		n--;
	}
	t.content=p;
	while(1){
		if(!n)break;
		if(isspace(*p))break;
		p++;
		n--;
	}
	t.content_end=p;
	while(1){
		if(!n)break;
		if(!isspace(*p))break;
		p++;
		n--;
	}
	t.end=p;
	f(&t,context);
}


typedef struct obj_file{
	const char*name;
	struct obj_o*o_array;
}obj_file;

inline static void load_obj_file__root(token*t,void*context){
	fsync(1);
	puts("** in root");
	write_token(t,1);
	puts("** out of root");
	fsync(1);
}

static void init_obj_file_from_path(obj_file*this,const char*path){
	printf(" * load obj file: %s\n",path);
	str s;
	str_initfromfile(&s,path);
	str_write(&s,1);
	foreach_token_in_str(&s,load_obj_file__root,0);
}
