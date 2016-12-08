#pragma once
#include<stdio.h>
#include"shader.h"
#include"token.h"
#include"dynp.h"
#include"dynf.h"
#include"dync.h"

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

typedef struct objmtl{
	dync name;
	float Ns;
	vec4 Ka,Kd,Ks,Ke;
	float Ni,d;
	dync map_Kd;

	id texture_id;
}objmtl;
#define objmtl_def (objmtl){dync_def,0,vec4_def,vec4_def,vec4_def,vec4_def,0,0,dync_def,0}

inline static void objmtl_free(objmtl*this){
	dync_free(&this->name);
	dync_free(&this->map_Kd);
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

inline static void objmtls_load_from_file(const char*path){
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
			p=t.end;
			float f=token_get_float(&t);
			o->Ns=f;
			continue;
		}
		if(token_equals(&t,"Ka") ||
				token_equals(&t,"Kd") ||
				token_equals(&t,"Ks") ||
				token_equals(&t,"Ke")){
			vec4 v;

			token x=token_next_from_string(p);
			p=x.end;
			v.x=token_get_float(&x);

			token y=token_next_from_string(p);
			p=y.end;
			v.y=token_get_float(&y);

			token z=token_next_from_string(p);
			p=z.end;
			v.z=token_get_float(&z);

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
			p=t.end;
			float f=token_get_float(&t);
			o->Ni=f;
			continue;
		}

		if(token_equals(&t,"d")){
			token t=token_next_from_string(p);
			p=t.end;
			float f=token_get_float(&t);
			o->d=f;
			continue;
		}

		if(token_equals(&t,"map_Kd")){
			token t=token_next_from_string(p);
			p=t.end;
			size_t n=token_size(&t);
			dync_add_list(&o->map_Kd,t.content,n);
			dync_add(&o->map_Kd,0);
			continue;
		}
	}
	objmtls_add(&materials,o);
}

typedef struct mtlrng{
	indx begin,end;
	objmtl*material;
}mtlrng;
#define mtlrng_def (mtlrng){0,0,NULL}

typedef struct glo{
	dynf vtxbuf;
	dynp ranges;
	id vtxbuf_id;
}glo;
//#define glo_def (glo){dynf_def,dynp_def}
#define glo_def (glo){dynf_def,dynp_def}


static/*gives*/glo*glo_load_next_from_string(const char**ptr_p){
	const char*p=*ptr_p;

	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;


	dynf vertex_buffer=dynf_def;
	dynp material_ranges=dynp_def;




	objmtl*current_objmtl=NULL;
	const char*basedir="obj/";
	indx vtxbufix=0;
	indx prev_vtxbufix=0;
	int first_o=1;
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;//token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
			continue;
		}
		if(token_equals(&t,"mtllib")){
			token t=token_next_from_string(p);
			p=t.end;
			dync s=dync_def;
			dync_add_string(&s,basedir);
			dync_add_list(&s,t.content,token_size(&t));
			dync_add(&s,0);
			objmtls_load_from_file(s.data);
			continue;
		}
		if(token_equals(&t,"o")){
			if(first_o){
				first_o=0;
				p=scan_to_including_newline(p);
				continue;
			}
			p=t.begin;
			break;
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
					mtlrng*mr=malloc(sizeof(mtlrng));
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
				token vert1=token_from_string_additional_delim(p,'/');
				p=vert1.end;
				indx ix1=token_get_uint(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,ix1-1);

				// texture index
				token vert2=token_from_string_additional_delim(p,'/');
				p=vert2.end;
				indx ix2=token_get_uint(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,ix2-1);
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=token_from_string_additional_delim(p,'/');
				p=vert3.end;
				indx ix3=token_get_uint(&vert3);
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
	mtlrng*mr=malloc(sizeof(mtlrng));
//					*mr=material_range_def;
	mr->begin=prev_vtxbufix;
	mr->end=vtxbufix;
	mr->material=current_objmtl;
	dynp_add(&material_ranges,mr);

	printf(" ranges %u   %lu vertices   %lu B\n",
			material_ranges.count,
			vertex_buffer.count,
			dynf_size_in_bytes(&vertex_buffer));

	glo*g=calloc(1,sizeof(glo));
	*g=(glo){vertex_buffer,material_ranges,0};
	*ptr_p=p;
	return g;
}


// returns vertex buffer of array of triangles
//                      [ x y z   r g b a   nx ny nz   u v]
static/*gives*/glo*glo_load_first_from_file(const char*path){
	dync file=dync_from_file(path);
	const char*p=file.data;
	return glo_load_next_from_string(&p);
}

//static/*gives*/dynp glo_load_all_from_file(const char*path){
////	stacktrace_print();
//	dync file=dync_from_file(path);
//	const char*p=file.data;
//	dynp ls=dynp_def;
//	while(*p){
//		glo*g=glo_load_next_from_string(&p);
//		dynp_add(&ls,g);
//	}
//	return ls;
//}

static/*gives*/dynp glo_load_all_from_file(const char*path){
	dync file=dync_from_file(path);
	const char*p=file.data;

	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;


	dynf vertex_buffer=dynf_def;
	dynp mtlrngs=dynp_def;

	dynp ls=dynp_def;

	const char*basedir="obj/";
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;//token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
		}else if(token_equals(&t,"mtllib")){
			token t=token_next_from_string(p);
			p=t.end;
			dync s=dync_def;
			dync_add_string(&s,basedir);
			dync_add_list(&s,t.content,token_size(&t));
			dync_add(&s,0);
			objmtls_load_from_file(s.data);
		}else if(token_equals(&t,"v")){
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
		}else if(token_equals(&t,"vt")){
			token tu=token_next_from_string(p);
			float u=token_get_float(&tu);
			p=tu.end;

			token tv=token_next_from_string(p);
			float v=token_get_float(&tv);
			p=tv.end;

			vec4*ptr=malloc(sizeof(vec4));
			*ptr=(vec4){u,v,0,0};
			dynp_add(&texuv,ptr);
		}else if(token_equals(&t,"vn")){
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
		}else{
			p=scan_to_including_newline(p);
		}
	}
	objmtl*current_objmtl=NULL;
	indx vtxbufix=0;
	indx prev_vtxbufix=0;
	int first_o=1;
	p=file.data;
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;
		if(token_equals(&t,"o")){
			p=scan_to_including_newline(p);
			if(first_o){
				first_o=0;
				continue;
			}
			mtlrng*mr=malloc(sizeof(mtlrng));
		//					*mr=material_range_def;
			mr->begin=prev_vtxbufix;
			mr->end=vtxbufix;
			mr->material=current_objmtl;
			dynp_add(&mtlrngs,mr);

			glo*g=calloc(1,sizeof(glo));
			*g=(glo){/*gives*/vertex_buffer,/*gives*/mtlrngs,0};
			dynp_add(&ls,g);

			prev_vtxbufix=vtxbufix;
			vertex_buffer=dynf_def;
			mtlrngs=dynp_def;

			continue;
		}
		if(token_equals(&t,"usemtl")){
			token t=token_next_from_string(p);
			p=t.end;
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
					mtlrng*mr=malloc(sizeof(mtlrng));
	//					*mr=material_range_def;
					mr->begin=prev_vtxbufix;
					mr->end=vtxbufix;
					mr->material=current_objmtl;
					dynp_add(&mtlrngs,mr);
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
		if(token_equals(&t,"f")){
			for(int i=0;i<3;i++){
				// position
				token vert1=token_from_string_additional_delim(p,'/');
				p=vert1.end;
				indx ix1=token_get_uint(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,ix1-1);

				// texture index
				token vert2=token_from_string_additional_delim(p,'/');
				p=vert2.end;
				indx ix2=token_get_uint(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,ix2-1);
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=token_from_string_additional_delim(p,'/');
				p=vert3.end;
				indx ix3=token_get_uint(&vert3);
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
		p=scan_to_including_newline(p);
	}

	mtlrng*mr=malloc(sizeof(mtlrng));
//					*mr=material_range_def;
	mr->begin=prev_vtxbufix;
	mr->end=vtxbufix;
	mr->material=current_objmtl;
	dynp_add(&mtlrngs,mr);


	glo*g=calloc(1,sizeof(glo));
	*g=(glo){/*gives*/vertex_buffer,/*gives*/mtlrngs,0};
	dynp_add(&ls,g);

	printf(" ranges %u   %lu vertices   %lu B\n",
			mtlrngs.count,
			vertex_buffer.count,
			dynf_size_in_bytes(&vertex_buffer));

	return/*gives*/ls;

}


inline static void glo_upload_to_opengl(glo*this){
	// upload vertex buffer
	glGenBuffers(1,&this->vtxbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER,this->vtxbuf_id);
	glBufferData(GL_ARRAY_BUFFER,
			(signed)dynf_size_in_bytes(&this->vtxbuf),
			this->vtxbuf.data,
			GL_STATIC_DRAW);

	// upload materials
	for(indx i=0;i<this->ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->ranges.data[i];
		objmtl*m=(objmtl*)mr->material;
		if(m->map_Kd.count){// load texture
			glGenTextures(1,&m->texture_id);

			printf(" * loading texture %u from '%s'\n",
					m->texture_id,m->map_Kd.data);

			glBindTexture(GL_TEXTURE_2D,m->texture_id);

			SDL_Surface*surface=IMG_Load(m->map_Kd.data);
			if(!surface)exit(-1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
					surface->w,surface->h,
					0,GL_RGB,GL_UNSIGNED_BYTE,
					surface->pixels);
			SDL_FreeSurface(surface);

			metrics.buffered_texture_data+=
					(unsigned)surface->w*(unsigned)surface->h*sizeof(uint32_t);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		}
	}

	metrics.buffered_vertex_data+=dynf_size_in_bytes(&this->vtxbuf);
}

//----------------------------------------------------------------------- calls

inline static void glo_render(glo*this,const float*mtxmw){

	glUniformMatrix4fv(shader_umtx_mw,1,0,mtxmw);

	glBindBuffer(GL_ARRAY_BUFFER,this->vtxbuf_id);

	glVertexAttribPointer(shader_apos,  3,GL_FLOAT,GL_FALSE,
			sizeof(vertex),0);
	glVertexAttribPointer(shader_argba, 4,GL_FLOAT,GL_FALSE,
			sizeof(vertex),(GLvoid*)(    3*sizeof(float)));
	glVertexAttribPointer(shader_anorm, 3,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)( (3+4)*sizeof(float)));
	glVertexAttribPointer(shader_atex,  2,GL_FLOAT, GL_FALSE,
			sizeof(vertex),(GLvoid*)((3+4+3)*sizeof(float)));

	for(indx i=0;i<this->ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->ranges.data[i];
		objmtl*m=mr->material;

		if(m->texture_id){
			glUniform1i(shader_utex,0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m->texture_id);
			glEnableVertexAttribArray(shader_atex);
		}else{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,0);
			glDisableVertexAttribArray(shader_atex);
		}

		glDrawArrays(GL_TRIANGLES,(signed)mr->begin,(signed)(mr->end-mr->begin));

		if(m->texture_id){
			glBindTexture(GL_TEXTURE_2D,0);
		}
	}
}















//--------------------------------------------------------------------- storage
//#define glos_cap 16
//#define glos_assert_bounds
//static glo __glo[glos_cap];

static dynp glos=dynp_def;

//---------------------------------------------------------------------

inline static glo*glo_at(indx i){
	return dynp_get(&glos,i);
}

inline static const glo*glo_at_const(indx i){
	return dynp_get(&glos,i);
}

inline static void glos_init(){}
inline static void glos_free(){}
inline static void glos_render(){
	for(unsigned i=0;i<glos.count;i++){
		glo*g=dynp_get(&glos,i);
		if(g->ranges.count)
			glo_render(g++,mat4_ident);
	}
}


inline static void glos_load_obj_file(const char*path){
	glo*g=/*takes*/glo_load_first_from_file(path);
	glo_upload_to_opengl(g);
	dynp_add(&glos,g);
}


inline static void glos_load_scene_from_file(const char*path){
	dynp ls=glo_load_all_from_file(path);
	for(indx i=0;i<ls.count;i++){
		glo*g=ls.data[i];
		glo_upload_to_opengl(g);
		dynp_add(&glos,g);
	}
}

inline static size_t glos_count(){
	return glos.count;
}














