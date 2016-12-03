#pragma once
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include"obj_o.h"
#include<sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>

typedef struct token{
	const char*begin;
	const char*content;
	const char*content_end;
	const char*end;
}token;

inline static size_t token_size_including_whitespace(token*t){
	return (size_t)(t->end-t->begin);
}

inline static void print_token_including_whitespace(token*t){
//	write(1,t->begin,(unsigned)(t->end-t->begin));
	printf("%.*s",(int)token_size_including_whitespace(t),t->begin);
}

inline static void print_token(token*t){
//	write(1,t->content,(unsigned)(t->content_end-t->content));
	printf("%.*s",(int)token_size_including_whitespace(t),t->begin);
}

inline static size_t token_size(token*t){
	return (size_t)(t->content_end-t->content);
}

inline static int token_starts_with(token*t,const char*str){
	return strncmp(str,t->content,strlen(str))==0;
}

inline static int token_equals(token*t,const char*str){
	return strncmp(str,t->content,strlen(str))==0;
}

inline static token next_token_from_string(const char*s){
	const char*p=s;
	token t;
	t.begin=s;
	while(1){
		if(!*p)break;
		if(!isspace(*p))break;
		p++;
	}
	t.content=p;
	while(1){
		if(!*p)break;
		if(isspace(*p))break;
		p++;
	}
	t.content_end=p;
	while(1){
		if(!*p)break;
		if(!isspace(*p))break;
		p++;
	}
	t.end=p;
	return t;
}


typedef struct obj_file{
	const char*name;
	struct obj_o*o_array;
}obj_file;

inline static const char*return_after_end_of_line(const char*p){
	while(1){
		if(!*p)return p;
		if(*p=='\n'){
			p++;
			return p;
		}
		p++;
	}
	return p;
}
static void init_obj_file_from_path(obj_file*this,const char*path){
	printf(" * load obj file: %s\n",path);
	struct stat st;
	if(stat(path,&st)==-1){
		perror("cannot stat");
		exit(-1);
	}
	size_t size=(unsigned)st.st_size;

	int fd=open(path,O_RDONLY);
	if(fd==1){
		perror("Open Failed");
		exit(11);
	}

	char*filedata=malloc(size+1);
	read(fd,filedata,size);
	filedata[size+1]=0;
	close(fd);

	const char*p=filedata;
	while(1){
		if(!*p)break;
		token t=next_token_from_string(p);
		p+=token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=return_after_end_of_line(p);
			continue;
		}
		if(token_equals(&t,"mtllib")){
			p=return_after_end_of_line(p);
			continue;
		}
		if(token_equals(&t,"o")){
			p=return_after_end_of_line(p);
			continue;
		}
		if(token_equals(&t,"usemtl")){
			p=return_after_end_of_line(p);
			continue;
		}
		if(token_equals(&t,"s")){
			p=return_after_end_of_line(p);
			continue;
		}
		print_token_including_whitespace(&t);
	}
	puts("----");
	exit(0);
}

































