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
#include"camera.h"
//----------------------------------------------------------------------- init
inline static void main_init_programs(){
	glos_load_scene_from_file("obj/color-cube.obj");
	object*o=object_alloc(&ninja_def);
	o->glo=glo_at(0);


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
}

inline static void main_init_scene(){
	glos_load_scene_from_file("obj/color-cube.obj");
	object*o=object_alloc(&ninja_def);
	o->glo=glo_at(0);
}

inline static void main_init(){

	main_init_programs();
	main_init_scene();
	shader.active_program_ix=0;
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

//	camera.eye=(vec4){10,10,10,0};
//	float agl=0;
	for(int running=1;running;){
//		agl+=31.14f/10.f*metrics.previous_frame_dt;
		metrics__at__frame_begin();
//		vec4_increase_with_vec4_over_dt(
//				&camera.eye,
//				&(vec4){cosf(agl)*10.f,10,sinf(agl)*10.f,0},1);
//				vec4_increase_with_vec4_over_dt(
//						&camera.eye,
//						&(vec4){10,10,10,0},metrics.previous_frame_dt);


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

		camera_update_matrix_wvp();

		glUniformMatrix4fv(shader_umtx_wvp,1,0,camera.mxwvp);

		glClearColor(c.red,c.green,c.blue,1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
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
