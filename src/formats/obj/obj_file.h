#pragma once
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<fcntl.h>
#include"../../lib.h"
#include"../../dynpvec.h"

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
	const char*p=t->content;
	while(1){
		if(p==t->content_end)
			return 1;
		if(*p!=*str)
			return 0;
		p++;
		str++;
	}
}

inline static float token_asfloat(token*t){
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
		if(*p==delim)break;
		p++;
	}
	if(*p==delim)
		p++;
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
		if(!*p)return p;
		if(*p=='\n'){
			p++;
			return p;
		}
		p++;
	}
	return p;
}

// returns triangles:  3 vertices times [ x y z  r g b  nx ny nz ]
static/*gives*/float*read_obj_file_from_path(const char*path,size_t*bufsize){
	printf(" * load: %s\n",path);
	struct stat st;
	if(stat(path,&st)==-1){
		perror("\ncannot stat");
		fprintf(stderr,"\t%s\n\n%s %d\n",path,__FILE__,__LINE__);
		exit(-1);
	}
	size_t size=(unsigned)st.st_size;

	int fd=open(path,O_RDONLY);
	if(fd==1){
		perror("\ncannot open");
		fprintf(stderr,"\t%s\n\n%s %d\n",path,__FILE__,__LINE__);
		exit(-1);
	}

	char*filedata=malloc(size+1);
	read(fd,filedata,size);
	filedata[size]=0;
	close(fd);

	dynpvec vertices=_dynpvec_init_;
	dynpvec normals=_dynpvec_init_;
	dynpvec faces=_dynpvec_init_;

//	vec4*vertex_array=malloc(sizeof(vec4)*vertex_count);
//	vec4*vertex_array_seek=vertex_array;

//	vec4*normal_array=malloc(sizeof(vec4)*normal_count);
//	vec4*normal_array_seek=normal_array;
	//                   vertex  3 vertices
//	*bufsize=face_count*((3+3+3)*3)*sizeof(float);//? assumes triangles


	float*glbuf=malloc(10000);
	float*glbuf_seek=glbuf;

	size_t nnormals=0;
	size_t nfaces=0;

	const char*p=filedata;

//	puts(p);
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
			float x=token_asfloat(&tx);
			p=tx.end;

			token ty=next_token_from_string(p);
			float y=token_asfloat(&ty);
			p=ty.end;

			token tz=next_token_from_string(p);
			float z=token_asfloat(&tz);
			p=tz.end;

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynpvec_add(&vertices,ptr);
//			if(dynpvec_get_last(&vertices)!=ptr){
//				printf(" returned value not same");
//				exit(-2);
//			}
//			*vertex_array_seek++=(vec4){x,y,z,0};
			continue;
		}
		if(token_equals(&t,"vn")){
			token tx=next_token_from_string(p);
			p=tx.end;
			float x=token_asfloat(&tx);

			token ty=next_token_from_string(p);
			p=ty.end;
			float y=token_asfloat(&ty);

			token tz=next_token_from_string(p);
			p=tz.end;
			float z=token_asfloat(&tz);

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){x,y,z,0};
			dynpvec_add(&normals,ptr);
			if(dynpvec_get_last(&normals)!=ptr){
				printf(" returned value not same");
				exit(-2);
			}
			nnormals++;
//			*normal_array_seek++=(vec4){x,y,z,0};
			continue;
		}
		if(token_equals(&t,"f")){
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
			char*start_of_f=p;
			nfaces++;
			printf("face : %zu\n",nfaces);
			for(int i=0;i<3;i++){
				token v1=next_token_from_string_additional_delim(p,'/');
				p=v1.end;
//				int v1ix=token_as_int(&v1);
				int v1ix=atoi(v1.content);

				// position
				vec4*vtx=(vec4*)dynpvec_get(&vertices,(size_t)(v1ix-1));
//				vec4 vtx=vertex_array[v1ix-1];
				*glbuf_seek++=vtx->x;
				*glbuf_seek++=vtx->y;
				*glbuf_seek++=vtx->z;

				// color
				*glbuf_seek++=0;// r
				*glbuf_seek++=1;// g
				*glbuf_seek++=0;// b
//				*glbuf_seek++=0;// a


				// texture index
				token v2=next_token_from_string_additional_delim(p,'/');
				p=v2.end;

				// normal
				token v3=next_token_from_string_additional_delim(p,'/');
				p=v3.end;
				int v3ix=atoi(v3.content);

//				vec4 norm=normal_array[v3ix-1];
				vec4 norm=*(vec4*)dynpvec_get(&normals,(size_t)(v3ix-1));
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

































