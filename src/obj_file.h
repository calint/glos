#pragma once
#include"token.h"
#include"dynp.h"
#include"dynf.h"
#include"dync.h"

inline static dync dync_from_file(const char*path){
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

	return (dync){
		.data=filedata,
		.count=((unsigned)length+1)/sizeof(char),
		.cap=(unsigned)length+1
	};
}


//newmtl texture
//Ns 96.078431
//Ka 1.000000 1.000000 1.000000
//Kd 0.640000 0.640000 0.640000
//Ks 0.500000 0.500000 0.500000
//Ke 0.000000 0.000000 0.000000
//Ni 1.000000
//d 1.000000
//illum 2
//map_Kd /home/c/w/glos/logo.jpeg

typedef struct _objmtl{
	dync name;
	float Ns;
	vec4 Ka,Kd,Ks,Ke;
	float Ni,d;
	dync map_Kd;
}objmtl;

#define DYNC_DEF (dync){0,0,0}
#define objmtl_def (objmtl){DYNC_DEF,0,vec4_def,vec4_def,vec4_def,vec4_def,0,0,DYNC_DEF}

inline static void objmtl_free(objmtl*this){
	dync_free(&this->name);
}

#include"objmtls.h"

static objmtls materials;

inline static objmtl*objmtl_alloc(){
	objmtl*o=malloc(sizeof(objmtl));
	if(!o){
		perror("\nout of memory while allocating a objmtl\n");
		fprintf(stderr,"\n     %s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
	*o=objmtl_def;
	return o;
}

inline static void obj_load_materials_from_file(const char*path){
	dync file=dync_from_file(path);
	objmtl*o=NULL;
	const char*p=file.data;
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;

		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_starts_with(&t,"newmtl")){
			if(o){
				objmtls_add(&materials,o);
			}
			o=objmtl_alloc();
			token t=token_next_from_string(p);
			p=t.end;
			size_t n=token_size(&t);
			dync_add_list(&o->name,t.content,n);
			dync_add(&o->name,0);
			continue;
		}
		if(token_equals(&t,"Ns")){
			token t=token_next_from_string(p);
			float f=token_get_float(&t);
			p=t.end;
			o->Ns=f;
			continue;
		}
		if(token_equals(&t,"Ka") ||
				token_equals(&t,"Kd") ||
				token_equals(&t,"Ks") ||
				token_equals(&t,"Ke")){
			vec4 v;

			token x=token_next_from_string(p);
			v.x=token_get_float(&x);
			p=x.end;

			token y=token_next_from_string(p);
			v.y=token_get_float(&y);
			p=y.end;

			token z=token_next_from_string(p);
			v.z=token_get_float(&z);
			p=z.end;

			v.q=0;

			if(token_equals(&t,"Ka")){
				o->Ka=v;
			}else if(token_equals(&t,"Kd")){
				o->Kd=v;
			}else if(token_equals(&t,"Ks")){
				o->Ks=v;
			}else if(token_equals(&t,"Ke")){
				o->Ke=v;
			}
			continue;
		}

		if(token_equals(&t,"Ni")){
			token t=token_next_from_string(p);
			float f=token_get_float(&t);
			p=t.end;
			o->Ni=f;
			continue;
		}

		if(token_equals(&t,"d")){
			token t=token_next_from_string(p);
			float f=token_get_float(&t);
			p=t.end;
			o->d=f;
			continue;
		}

		if(token_equals(&t,"map_Kd")){
			token t=token_next_from_string(p);
			p=t.end;
			size_t n=token_size(&t);
			dync_add_list(&o->map_Kd,t.content,n);
			dync_add(&o->name,0);
			continue;
		}
	}
	objmtls_add(&materials,o);
}

typedef struct material_range{
	arrayix begin,end;
	objmtl*material;
}material_range;
#define material_range_def (material_range){0,0,0}

typedef struct glo{
	dynf vertex_buffer;
	dynp render_ranges;
}glo;
#define glo_def (glo){dynf_def,dynp_def}

// returns vertex buffer of array of triangles
//                      [ x y z   r g b a   nx ny nz   u v]
static/*gives*/glo*read_obj_file_from_path(const char*path){
	dync file=dync_from_file(path);
	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;
	dynf vertex_buffer=dynf_def;
	dynp material_ranges=dynp_def;

	const char*p=file.data;
	objmtl*current_objmtl=NULL;
	const char*basedir="obj/";
	arrayix vtxbufix=0;
	arrayix prev_vtxbufix=0;
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;//token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"mtllib")){
			token t=token_next_from_string(p);
			dync s=dync_def;
			dync_add_string(&s,basedir);
			dync_add_list(&s,t.content,token_size(&t));
			dync_add(&s,0);
			obj_load_materials_from_file(s.data);
			continue;
		}
		if(token_equals(&t,"o")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"usemtl")){
			token t=token_next_from_string(p);
			dync name=dync_def;
			dync_add_list(&name,t.content,token_size(&t));
			dync_add(&name,0);

			objmtl*m=NULL;
			for(unsigned i=0;i<name.count;i++){
				m=objmtls_get(&materials,i);
				if(!strcmp(m->name.data,name.data)){
					break;
				}
			}
			if(m){
				if(prev_vtxbufix!=vtxbufix){
					material_range*mr=malloc(sizeof(material_range));
//					*mr=material_range_def;
					mr->begin=prev_vtxbufix;
					mr->end=vtxbufix;
					mr->material=current_objmtl;
					dynp_add(&material_ranges,mr);
					prev_vtxbufix=vtxbufix;
				}
				current_objmtl=m;
				continue;
			}
			fprintf(stderr,"\ncould not find material\n");
			fprintf(stderr,"   %s\n",name.data);
			fprintf(stderr,"\n     %s %d\n",__FILE__,__LINE__);
			exit(-1);
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
				arrayix ix1=token_get_uint(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,ix1-1);

				// texture index
				token vert2=next_token_from_string_additional_delim(p,'/');
				p=vert2.end;
				arrayix ix2=token_get_uint(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,ix2-1);
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=next_token_from_string_additional_delim(p,'/');
				p=vert3.end;
				arrayix ix3=token_get_uint(&vert3);
				vec4*norm=(vec4*)dynp_get(&normals,ix3-1);

				// buffer
				dynf_add(&vertex_buffer,vtx->x);
				dynf_add(&vertex_buffer,vtx->y);
				dynf_add(&vertex_buffer,vtx->z);

				dynf_add(&vertex_buffer,current_objmtl->Kd.x);
				dynf_add(&vertex_buffer,current_objmtl->Kd.y);
				dynf_add(&vertex_buffer,current_objmtl->Kd.z);
				dynf_add(&vertex_buffer,1);

				dynf_add(&vertex_buffer,norm->x);
				dynf_add(&vertex_buffer,norm->y);
				dynf_add(&vertex_buffer,norm->z);

				dynf_add(&vertex_buffer,tex->x);
				dynf_add(&vertex_buffer,tex->y);

				vtxbufix++;
			}
			continue;
		}
	}
	material_range*mr=malloc(sizeof(material_range));
//					*mr=material_range_def;
	mr->begin=prev_vtxbufix;
	mr->end=vtxbufix;
	mr->material=current_objmtl;
	dynp_add(&material_ranges,mr);

	printf(" ranges %u   %lu vertices   %lu B\n",
			material_ranges.count,
			vertex_buffer.count,
			dynf_size_in_bytes(&vertex_buffer));

	glo*g=malloc(sizeof(glo));
	g->render_ranges=material_ranges;
	g->vertex_buffer=vertex_buffer;
	return g;
}

