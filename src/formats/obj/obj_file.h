#pragma once
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<fcntl.h>

#include "../../dynv.h"
#include"../../lib.h"

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
	const char*p=t->content;//? stdlib
	while(1){
		if(p==t->content_end)
			return 1;
		if(*p!=*str)
			return 0;
		p++;
		str++;
	}
}

inline static float token_get_float(token*t){
	float f=(float)atof(t->content);// assuming file ends with whitespace
	return f;
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

inline static token next_token_from_string_additional_delim(
		const char*s,
		char delim)
{
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
		if(*p==delim){
			p++;
			t.end=t.content_end=p;
			return t;
		}
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

inline static const char*scan_to_including_newline(const char*p){
	while(1){
		if(!*p)
			return p;
		if(*p=='\n'){
			p++;
			return p;
		}
		p++;
	}
}

// returns triangles:  3 vertices times [ x y z  r g b  nx ny nz ]
static/*gives*/float*read_obj_file_from_path(const char*path,size_t*bufsize){
	printf(" * load: %s\n",path);
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
	size_t ncharsread=fread(filedata,1,(size_t)length+1,f);
	if(ncharsread!=(size_t)length){
		fprintf(stderr,"\nnot-a-full-read\n");
		fprintf(stderr,"\t\n%s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	fclose(f);
	filedata[length]=0;

	dynv vertices=_dynv_init_;
	dynv normals=_dynv_init_;

	float*glbuf=malloc(100000);//? dyna_float
	float*glbuf_seek=glbuf;

	const char*p=filedata;
	while(*p){
		token t=next_token_from_string(p);
		p=t.end;//token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"mtllib")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"o")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"usemtl")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"s")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"v")){
			token tx=next_token_from_string(p);
			float x=token_get_float(&tx);
			p=tx.end;

			token ty=next_token_from_string(p);
			float y=token_get_float(&ty);
			p=ty.end;

			token tz=next_token_from_string(p);
			float z=token_get_float(&tz);
			p=tz.end;

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynv_add(&vertices,ptr);
			continue;
		}
		if(token_equals(&t,"vn")){
			token tx=next_token_from_string(p);
			p=tx.end;
			float x=token_get_float(&tx);

			token ty=next_token_from_string(p);
			p=ty.end;
			float y=token_get_float(&ty);

			token tz=next_token_from_string(p);
			p=tz.end;
			float z=token_get_float(&tz);

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynv_add(&normals,ptr);
			continue;
		}

		if(token_equals(&t,"f")){
			for(int i=0;i<3;i++){
				token v1=next_token_from_string_additional_delim(p,'/');
				p=v1.end;
				int v1ix=atoi(v1.content);
				// position
				vec4*vtx=(vec4*)dynv_get(&vertices,(size_t)(v1ix-1));
				*glbuf_seek++=vtx->x;
				*glbuf_seek++=vtx->y;
				*glbuf_seek++=vtx->z;
				// color
				*glbuf_seek++=0;// r
				*glbuf_seek++=1;// g
				*glbuf_seek++=0;// b
				// texture index
				token v2=next_token_from_string_additional_delim(p,'/');
				p=v2.end;
				// normal
				token v3=next_token_from_string_additional_delim(p,'/');
				p=v3.end;
				int v3ix=atoi(v3.content);
				vec4 norm=*(vec4*)dynv_get(&normals,(size_t)(v3ix-1));
				*glbuf_seek++=norm.x;
				*glbuf_seek++=norm.y;
				*glbuf_seek++=norm.z;
			}
			continue;
		}
	}
	*bufsize=sizeof(float)*(size_t)(glbuf_seek-glbuf);
	return glbuf;
}

































//			FILE*f=fopen("/home/c/w/glos/dump.txt","w");
//			// position(x,y,z) normal(x,y,z)
//			for(size_t i=0;i<vertices.size;i++){
//				vec4*v=(vec4*)dynpvec_get(&vertices,i);
//				fprintf(f,"v %f %f %f\n",v->x,v->y,v->z);
//			}
//			// position(x,y,z) normal(x,y,z)
//			for(size_t i=0;i<normals.size;i++){
//				vec4*v=(vec4*)dynpvec_get(&normals,i);
//				fprintf(f,"vn %.4f %.4f %.4f\n",v->x,v->y,v->z);
//			}
//			fclose(f);
//			exit(0);
//inline static void dbg(const char*p){
//	while(1){
//		if(!*p)return;
//		if(*p=='\n')break;
//		putchar(*p++);
//	}
//	putchar('\n');
//}
//
