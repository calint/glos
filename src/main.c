#include"sdl.h"
#include"lib.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"object_alloc.h"
#include"metrics.h"
#include"dyni.h"
#include"app/ninja.h"
#include"glo.h"
#include"mat4.h"
//----------------------------------------------------------------------- init
inline static void main_init(){

	const char*vtx="#version 130\n"
			"uniform mat4 umtx_mw;\n"
			"in vec3 apos;\n"
			"void main(){\n"
			"    gl_Position=umtx_mw*vec4(apos,1);\n"
			"}\n";

	const char*frag="#version 130\n"
			"void main(){\n"
			"    gl_FragColor=vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1);\n"
			"}";

	dyni attrs=dyni_def;
	dyni_add(&attrs,shader_apos);
	program_load_from_source(vtx,frag,/*gives*/attrs);



	vtx="#version 130\n"
			"uniform mat4 umtx_mw;\n"
			"in vec3 apos;\n"
			"in vec4 argba;\n"
			"out vec4 color;\n"
			"void main(){\n"
			"   gl_Position=umtx_mw*vec4(apos,1);\n"
			"   color=argba;\n"
			"}\n";

	frag="#version 130\n"
			"in vec4 color;\n"
			"void main(){\n"
			"   gl_FragColor=color;\n"
			"}";

	attrs=dyni_def;
	dyni_add(&attrs,shader_apos);
	dyni_add(&attrs,shader_argba);
	program_load_from_source(vtx,frag,/*gives*/attrs);

	shader.active_program_ix=0;
	glos_load_scene_from_file("obj/bounding_volumes.obj");

//	object*o;
//	o=object_alloc(&ninja_def);
//	o->glo=glo_at(0);
//	o->angle=(angle){0,0,90};
//	const float vr=.5f;
//	o->velocity=(velocity){
//		random_range(-vr,vr),random_range(-vr,vr),0,0};
//
//	const float ar=360;
//	o->angular_velocity=(angular_velocity){
//		0,0,random_range(-ar,ar),0};
//
//	o=object_alloc(&object_def);
//	o->glo=glo_at;
//	o->scale=(scale){2,2,2,1};
//	o->position=(position){0,0,-.5f,1};
//	o->bounding_radius=bounding_radius_for_scale(&o->scale);
//	o->scale=(scale){.5f,.5f,.5f,1};


//	const float sr=.2f;
//	o->scale=(scale){
//		random_range(-sr,sr),
//		random_range(-sr,sr),
//		random_range(-sr,sr),1};
}


struct{
	position eye;
	float mxwvp[16];
	position lookat;
	vec4 up;
	float znear,zfar;
	float wi,hi;
}camera={
		.eye={0,0,0,0},
		.mxwvp={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
		.lookat={0,0,0,0},
		.up={0,1,0,0},
		.znear=0.01f,
		.zfar=100,
		.wi=1024,
		.hi=1024,
};
inline static void camera_update(){
	vec4_increase_with_vec4_over_dt(&camera.eye,&(vec4){-1,0,0,0},
			metrics.previous_frame_dt);


	float mtx_lookat[16];
	mat4_set_look_at(mtx_lookat,&camera.eye,&camera.lookat,&camera.up);
	//----
	vec4 t=camera.eye;
	vec4_negate(&t);
	float mtx_trans[16];
	mat4_set_translate(mtx_trans,&t);
	//----
	float mtx_proj[16];
	float aspect_ratio=10;
	mat4_set_ortho_projection(mtx_proj,-10,10,
			-aspect_ratio,aspect_ratio, camera.znear,camera.zfar);
	//----
	float m1[16];
	mat4_multiply(m1,mtx_trans,mtx_lookat);
	mat4_multiply(camera.mxwvp,mtx_proj,m1);
}

//------------------------------------------------------------------------ main
int main(int argc,char*argv[]){
	indx gloid=0,mingloid=0,maxgloid=10;

	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9s :\n","type",           "bytes");
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9ld :\n","part",sizeof(part));
	printf(": %15s : %-9ld :\n","object",sizeof(object));
	printf(": %15s : %-9ld :\n","glo",sizeof(glo));
	printf(":-%15s-:-%-9s-:\n","---------------","--------");

	sdl_init();
	window_init();
	shader_init();
	objects_init();
	glos_init();
	main_init();

	indx program_id_min=0;
	indx program_id_max=2;

	struct{
		GLclampf red;
		GLclampf green;
		GLclampf blue;
	}c={.11f,.37f,1};

	int draw_default=0,draw_objects=1;
	puts("");
	metrics_print_headers();
	{
		program*p=dynp_get(&programs,shader.active_program_ix);
		gid progid=p->gid;
		glUseProgram(progid);
		dyni*enable=&p->attributes;
		printf(" * using program at index %u\n",shader.active_program_ix);
		for(unsigned i=0;i<enable->count;i++){
			const unsigned ix=(unsigned)dyni_get(enable,i);
			printf("   * disable vertex attrib array %d\n",ix);
			glEnableVertexAttribArray(ix);
		}
	}
	gid previous_active_program_ix=shader.active_program_ix;

	camera.eye=(vec4){2,2,10,0};

	for(int running=1;running;){
		metrics__at__frame_begin();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						running = 0;
						break;
					case SDLK_w:
						objects[0].velocity.y=.5f;
						c.red = 1;
						c.green = 0;
						c.blue = 0;
						break;
					case SDLK_a:
						objects[0].angular_velocity.z=90;
						c.red = 0;
						c.green = 1;
						c.blue = 0;
						break;
					case SDLK_s:
						objects[0].velocity.y=-.5f;
						c.red = 0;
						c.green = 0;
						c.blue = 1;
						break;
					case SDLK_d:
						objects[0].angular_velocity.z=-90;
						c.red = 1;
						c.green = 1;
						break;
					case SDLK_1:
						draw_default=1;
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_w:
						objects[0].velocity.y=0;
						break;
					case SDLK_a:
						objects[0].angular_velocity.z=0;
						break;
					case SDLK_s:
						objects[0].velocity.y=-0;
						break;
					case SDLK_d:
						objects[0].angular_velocity.z=0;
						break;
					case SDLK_1:
						draw_default=0;
						break;
					case SDLK_2:{
						gloid++;
						if(gloid>maxgloid){
							gloid=mingloid;
						}
						object_at(0)->glo=glo_at(gloid);
						break;
					}
					case SDLK_3:{
						shader.active_program_ix++;
						if(shader.active_program_ix>program_id_max){
							shader.active_program_ix=program_id_min;
						}
						break;
					}
				}
				break;


			}
		}

		if(previous_active_program_ix!=shader.active_program_ix){
			printf(" * switching to program at index %u\n",
					shader.active_program_ix);

			program*pp=dynp_get(&programs,previous_active_program_ix);
			dyni disable=pp->attributes;

			for(unsigned i=0;i<disable.count;i++){
				const unsigned ix=(unsigned)dyni_get(&disable,i);
				printf("   * disable vertex attrib array %d\n",ix);
				glDisableVertexAttribArray(ix);
			}

			program*ap=dynp_get(&programs,shader.active_program_ix);
//			const unsigned progid=ap->gid;

			glUseProgram(ap->gid);

			dyni*enable=&ap->attributes;
			for(unsigned i=0;i<enable->count;i++){
				const unsigned ix=(unsigned)dyni_get(enable,i);
				printf("   * enable vertex attrib array %d\n",ix);
				glEnableVertexAttribArray(ix);
			}
			previous_active_program_ix=shader.active_program_ix;
		}

		camera_update();

		glUniformMatrix4fv(shader_umtx_wvp,1,0,camera.mxwvp);

		glClearColor(c.red,c.green,c.blue,1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//		objects_update(metrics.previous_frame_dt);
//		if(draw_objects)objects_render();

		glos_render();

		if(draw_default)shader_render();

		SDL_GL_SwapWindow(window.ref);

		metrics__at__update_frame_end();
	}
	//---------------------------------------------------------------------free
	//? early-hangup
//	fps_free();
	glos_free();
	objects_free();
	shader_free();
	window_free();
	sdl_free();
	metrics_print();
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
