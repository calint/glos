#pragma once
#include<stdio.h>
#include"objmtl.h"

typedef struct mtlrng{
	unsigned begin,count;
	objmtl*material_ptr;
	str name;
}mtlrng;

#define mtlrng_def (mtlrng){0,0,NULL,str_def}

typedef struct glo{
	dynf vtxbuf;
	dynp ranges;
	id vtxbuf_id;
	str name;
}glo;

#define glo_def (glo){dynf_def,dynp_def,str_def}


inline static/*gives*/glo*glo_alloc(){
	metrics.glos_allocated++;
	return malloc(sizeof(glo));
}


inline static/*gives*/glo*glo_alloc_zeroed(){
	metrics.glos_allocated++;
	return calloc(1,sizeof(glo));
}

inline static/*gives*/glo*glo_alloc_from(const glo* g){
	metrics.glos_allocated++;
	glo*o=malloc(sizeof(glo));
	*o=*g;
	return o;
}


inline static void glo_free(glo*o){
	glDeleteBuffers(1,&o->vtxbuf_id);
	metrics.buffered_vertex_data-=dynf_size_in_bytes(&o->vtxbuf);
	dynf_free(&o->vtxbuf);

	for(unsigned i=0;i<o->ranges.count;i++){
		mtlrng*mr=(mtlrng*)o->ranges.data[i];
		objmtl*m=(objmtl*)mr->material_ptr;
		if(m->texture_id){
			glDeleteTextures(1,&m->texture_id);
			metrics.buffered_texture_data-=m->texture_size_bytes;
		}
	}
	dynp_free(&o->ranges);
	metrics.glos_allocated--;
//	memset(o,0,sizeof(glo));
}

static/*gives*/glo*glo_make_next_from_string(const char**ptr_p){
	const char*p=*ptr_p;

	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;

	dynf vertex_buffer=dynf_def;
	dynp material_ranges=dynp_def;

	objmtl*current_objmtl=NULL;
	const char*basedir="obj/";
	unsigned vtxbufix=0;
	unsigned prev_vtxbufix=0;
	int first_o=1;
	str object_name=str_def;
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
			str s=str_def;
			str_add_string(&s,basedir);
			str_add_list(&s,t.content,token_size(&t));
			str_add(&s,0);
			objmtls_load_from_file(s.data);
			continue;
		}
		if(token_equals(&t,"o")){
			if(first_o){
				first_o=0;
				token t=token_next_from_string(p);
				p=t.end;
				str_add_list(&object_name,t.content,token_size(&t));
				str_add(&object_name,0);
				puts(object_name.data);
//				p=scan_to_including_newline(p);
				continue;
			}
			p=t.begin;
			break;
		}
		if(token_equals(&t,"usemtl")){
			token t=token_next_from_string(p);
			str name=str_def;
			str_add_list(&name,t.content,token_size(&t));
			str_add(&name,0);
			objmtl*m=NULL;
			for(unsigned i=0;i<materials.count;i++){
				objmtl*mm=objmtls_get(&materials,i);
//				printf("%s\n",mm->name.data);
				if(!strcmp(mm->name.data,name.data)){
					m=mm;
					break;
				}
			}
			if(m){
				if(prev_vtxbufix!=vtxbufix){
					mtlrng*mr=malloc(sizeof(mtlrng));
//					*mr=material_range_def;
					mr->begin=prev_vtxbufix;
					mr->count=vtxbufix;
					mr->material_ptr=current_objmtl;
					dynp_add(&material_ranges,mr);
					prev_vtxbufix=vtxbufix;
				}
				current_objmtl=m;
				continue;
			}
			fprintf(stderr,"\n%s:%u: could not find material\n",__FILE__,__LINE__);
			fprintf(stderr,"        name: '%s'\n\n",name.data);
			stacktrace_print(stderr);
			fprintf(stderr,"\n\n");
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
				unsigned ix1=token_get_uint(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,ix1-1);

				// texture index
				token vert2=token_from_string_additional_delim(p,'/');
				p=vert2.end;
				unsigned ix2=token_get_uint(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,ix2-1);
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=token_from_string_additional_delim(p,'/');
				p=vert3.end;
				unsigned ix3=token_get_uint(&vert3);
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
	mr->count=vtxbufix;
	mr->material_ptr=current_objmtl;
	dynp_add(&material_ranges,mr);

	printf(" ranges %u   %u vertices   %lu B\n",
			material_ranges.count,
			vertex_buffer.count,
			dynf_size_in_bytes(&vertex_buffer));

	glo*g=glo_alloc_zeroed();
	*g=(glo){vertex_buffer,material_ranges,0,/*gives*/object_name};
	*ptr_p=p;
	return g;
}


// returns vertex buffer of array of triangles
//                      [ x y z   r g b a   nx ny nz   u v]
static/*gives*/glo*glo_make_first_from_file(const char*path){
	str file=str_from_file(path);
	const char*p=file.data;
	return glo_make_next_from_string(&p);
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
	printf(" * loading objects from '%s'\n",path);
	str file=str_from_file(path);
	const char*p=file.data;

	dynp vertices=dynp_def;
	dynp normals=dynp_def;
	dynp texuv=dynp_def;


	dynf vertex_buffer=dynf_def;
	dynp mtlrngs=dynp_def;
	dynp reuslt=dynp_def;
	const char*basedir="obj/";
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;//token_size_including_whitespace(&t);
		if(token_starts_with(&t,"#")){
			p=scan_to_including_newline(p);
		}else if(token_equals(&t,"mtllib")){
			token t=token_next_from_string(p);
			p=t.end;
			str s=str_def;
			str_add_string(&s,basedir);
			str_add_list(&s,t.content,token_size(&t));
			str_add(&s,0);
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
	unsigned vtxbufix=0;
	unsigned prev_vtxbufix=0;
	int first_o=1;
	p=file.data;
	unsigned vtxbufix_base=0;
	while(*p){
		token t=token_next_from_string(p);
		p=t.end;
		if(token_equals(&t,"o")){
			token t=token_next_from_string(p);
			p=t.end;
			str s=str_def;
			str_add_list(&s,t.content,(unsigned)(t.content_end-t.content));
			str_add(&s,0);
			printf("     object '%s'\n",s.data);
			p=scan_to_including_newline(p);
			if(first_o){
				first_o=0;
				continue;
			}
			mtlrng*mr=malloc(sizeof(mtlrng));
		//					*mr=material_range_def;
			mr->begin=prev_vtxbufix-vtxbufix_base;
			mr->count=vtxbufix-prev_vtxbufix;
			mr->material_ptr=current_objmtl;
			dynp_add(&mtlrngs,mr);

			glo*g=glo_alloc_zeroed();
			*g=(glo){/*gives*/vertex_buffer,/*gives*/mtlrngs,0};
			dynp_add(&reuslt,g);

			printf("       %u range%cs   %lu vertices   %zu B\n",
					mtlrngs.count,mtlrngs.count==1?' ':'s',
					vertex_buffer.count/sizeof(vertex),
					dynf_size_in_bytes(&vertex_buffer));


			vtxbufix_base=prev_vtxbufix=vtxbufix;
			vertex_buffer=dynf_def;
			mtlrngs=dynp_def;

			continue;
		}
		if(token_equals(&t,"usemtl")){
			token t=token_next_from_string(p);
			p=t.end;
			str name=str_def;
			str_add_list(&name,t.content,token_size(&t));
			str_add(&name,0);

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
					mr->begin=prev_vtxbufix-vtxbufix_base;
					mr->count=vtxbufix-prev_vtxbufix;
					mr->material_ptr=current_objmtl;
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
				unsigned ix1=token_get_uint(&vert1);
				vec4*vtx=(vec4*)dynp_get(&vertices,ix1-1);

				// texture index
				token vert2=token_from_string_additional_delim(p,'/');
				p=vert2.end;
				unsigned ix2=token_get_uint(&vert2);
				vec4 tx,*tex;tex=&tx;
				if(ix2){
					tex=(vec4*)dynp_get(&texuv,ix2-1);
				}else{
					*tex=(vec4){0,0,0,0};
				}
				// normal
				token vert3=token_from_string_additional_delim(p,'/');
				p=vert3.end;
				unsigned ix3=token_get_uint(&vert3);
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
	mr->begin=prev_vtxbufix-vtxbufix_base;
	mr->count=vtxbufix-prev_vtxbufix;
	mr->material_ptr=current_objmtl;
	dynp_add(&mtlrngs,mr);



	glo*g=glo_alloc_zeroed();
	*g=(glo){/*gives*/vertex_buffer,/*gives*/mtlrngs,0};
	dynp_add(&reuslt,g);

	printf("       %u range%c   %lu vertices   %lu B\n",
			mtlrngs.count,mtlrngs.count==1?' ':'s',
			vertex_buffer.count/sizeof(vertex),
			dynf_size_in_bytes(&vertex_buffer));

	return/*gives*/reuslt;

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
	for(unsigned i=0;i<this->ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->ranges.data[i];
		objmtl*m=(objmtl*)mr->material_ptr;
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

			m->texture_size_bytes=
					(unsigned)(surface->w*surface->h*(signed)sizeof(uint32_t));

			metrics.buffered_texture_data+=m->texture_size_bytes;

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		}
	}

	metrics.buffered_vertex_data+=dynf_size_in_bytes(&this->vtxbuf);
}

//----------------------------------------------------------------------- calls

inline static void glo_render(glo*this,const mat4 mtxmw){

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

	for(unsigned i=0;i<this->ranges.count;i++){
		mtlrng*mr=(mtlrng*)this->ranges.data[i];
		objmtl*m=mr->material_ptr;

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

		glDrawArrays(GL_TRIANGLES,(signed)mr->begin,(signed)mr->count);
		metrics.triangles_rendered_prv_frame+=mr->count/3;

		if(m->texture_id){
			glBindTexture(GL_TEXTURE_2D,0);
//			glDisableVertexAttribArray(shader_atex);
		}
	}
}

//--------------------------------------------------------------------- storage

static dynp glos=dynp_def;

inline static glo*glo_at(unsigned i){return dynp_get(&glos,i);}
inline static const glo*glo_at_const(unsigned i){return dynp_get(&glos,i);}
inline static unsigned glos_count(){return glos.count;}

inline static void glos_init(){}

inline static void glos_free(){
	for(unsigned i=0;i<glos.count;i++){
		glo_free(glos.data[i]);
	}
}

inline static void glos_render(){
	for(unsigned i=0;i<glos.count;i++){
		glo*g=dynp_get(&glos,i);
		if(g->ranges.count)
			glo_render(g++,mat4_identity);
	}
}


inline static void glos_load_first_in_file(const char*path){
	glo*g=/*takes*/glo_make_first_from_file(path);
	glo_upload_to_opengl(g);
	dynp_add(&glos,/*sinks*/g);
//	return g;
}


inline static void glos_load_scene_from_file(const char*path){
	dynp ls=glo_load_all_from_file(path);
	for(unsigned i=0;i<ls.count;i++){
		glo*g=ls.data[i];
		glo_upload_to_opengl(g);
		dynp_add(&glos,g);
	}
}
//
//inline static glo*glos_last_in_array(){
//	return glo_at(glos.data[glos.count-1]);
//}

inline static glo*glos_find_by_name(const char*name){
	for(unsigned i=0;i<glos.count;i++){
		glo*g=glos.data[i];
		if(!strcmp(name,g->name.data)){
			return g;
		}
	}
	fprintf(stderr,"\n%s:%u: could not find glo '%s' \n",__FILE__,__LINE__,name);
	stacktrace_print(stderr);
	fprintf(stderr,"\n\n");
	exit(-1);
	return NULL;
}
