#pragma once
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<fcntl.h>

#include"dynf.h"
#include"dync.h"
#include "dynp.h"
#include"lib.h"

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

inline static void token_print(token*t){
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
	float f=(float)atof(t->content);//? assuming file ends with whitespace
	return f;
}

inline static int token_get_int(token*t){
	int i=atoi(t->content);//?  assuming file ends with whitespace
	return i;
}

inline static token token_next_from_string(const char*s){
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





//# Blender MTL File: 'None'
//# Material Count: 2
//
//newmtl blue
//Ns 96.078431
//Ka 1.000000 1.000000 1.000000
//Kd 0.005881 0.000000 0.640000
//Ks 0.500000 0.500000 0.500000
//Ke 0.000000 0.000000 0.000000
//Ni 1.000000
//d 1.000000
//illum 2
//map_Kd /home/c/w/glos/logo.jpg
//
//newmtl red
//Ns 96.078431
//Ka 1.000000 1.000000 1.000000
//Kd 0.640000 0.000000 0.012335
//Ks 0.500000 0.500000 0.500000
//Ke 0.000000 0.000000 0.000000
//Ni 1.000000
//d 1.000000
//illum 2
//map_Kd /home/c/w/glos/logo.jpg

typedef dync str;

typedef struct obj_mtl{

	str name;

	str texture_path;

	GLuint texture_gid;

}obj_mtl;












// returns array buffer of triangles [ x y z   r g b a   nx ny nz   u v]
static/*gives*/dynf read_obj_file_from_path(const char*path){
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

	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;
	dynf vertex_buffer=dynf_def;

	const char*p=filedata;
	while(*p){
		token t=token_next_from_string(p);
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
			token tx=token_next_from_string(p);
			float x=token_get_float(&tx);
			p=tx.end;

			token ty=token_next_from_string(p);
			float y=token_get_float(&ty);
			p=ty.end;

			token tz=token_next_from_string(p);
			float z=token_get_float(&tz);
			p=tz.end;

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynp_add(&vertices,ptr);
			continue;
		}
		if(token_equals(&t,"vt")){
			token tu=token_next_from_string(p);
			float u=token_get_float(&tu);
			p=tu.end;

			token tv=token_next_from_string(p);
			float v=token_get_float(&tv);
			p=tv.end;

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){u,v,0,0};
			dynp_add(&texuv,ptr);
			continue;
		}
		if(token_equals(&t,"vn")){
			token tx=token_next_from_string(p);
			p=tx.end;
			float x=token_get_float(&tx);

			token ty=token_next_from_string(p);
			p=ty.end;
			float y=token_get_float(&ty);

			token tz=token_next_from_string(p);
			p=tz.end;
			float z=token_get_float(&tz);


			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynp_add(&normals,ptr);
			continue;
		}

		if(token_equals(&t,"f")){
			for(int i=0;i<3;i++){
				// position
				token vert1=next_token_from_string_additional_delim(p,'/');
				p=vert1.end;
				int ix1=token_get_int(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,(size_t)(ix1-1));

				// texture index
				token vert2=next_token_from_string_additional_delim(p,'/');
				p=vert2.end;
				int ix2=token_get_int(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,(size_t)(ix2-1));
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=next_token_from_string_additional_delim(p,'/');
				p=vert3.end;
				int ix3=token_get_int(&vert3);
				vec4*norm=(vec4*)dynp_get(&normals,(size_t)(ix3-1));

				// buffer
				dynf_add(&vertex_buffer,vtx->x);
				dynf_add(&vertex_buffer,vtx->y);
				dynf_add(&vertex_buffer,vtx->z);

				dynf_add(&vertex_buffer,0);
				dynf_add(&vertex_buffer,0);
				dynf_add(&vertex_buffer,0);
				dynf_add(&vertex_buffer,1);

				dynf_add(&vertex_buffer,norm->x);
				dynf_add(&vertex_buffer,norm->y);
				dynf_add(&vertex_buffer,norm->z);

				dynf_add(&vertex_buffer,tex->x);
				dynf_add(&vertex_buffer,tex->y);
			}
			continue;
		}
	}
	return vertex_buffer;
}
