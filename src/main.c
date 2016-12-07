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

	glos_load_obj_file(1,"obj/plane.obj");
	glos_load_obj_file(2,"obj/ico_sphere.obj");
	glos_load_obj_file(3,"obj/cylinder.obj");
	glos_load_obj_file(4,"obj/grid_8x8.obj");
	glos_load_obj_file(5,"obj/sphere.obj");
	glos_load_obj_file(6,"obj/torus.obj");
	glos_load_obj_file(7,"obj/disc.obj");
	glos_load_obj_file(8,"obj/plane_red.obj");
	glos_load_obj_file(9,"obj/cube-blue-red.obj");
	glos_load_obj_file(10,"obj/cubo.obj");

	object*o;
	o=object_alloc(&ninja_def);
	o->glo=glo_at(1);
	const float vr=.5f;
	o->velocity=(velocity){
		random_range(-vr,vr),random_range(-vr,vr),0,0};

	const float ar=360;
	o->angular_velocity=(angular_velocity){
		0,0,random_range(-ar,ar),0};

//	o=object_alloc(&object_def);
//	o->glob_id=1;
//	o->scale=(scale){2,2,2,1};
//	o->position=(position){0,0,-.5f,1};
//	o->bounding_radius=bounding_radius_for_scale(&o->scale);


//	const float sr=.2f;
//	o->scale=(scale){
//		random_range(-sr,sr),
//		random_range(-sr,sr),
//		random_range(-sr,sr),1};
	o->scale=(scale){.5f,.5f,.5f,1};
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

		float mtx_wvp[]={
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1,
		};

		float aspect_ratio=4;
		mat4_load_ortho_projection(mtx_wvp,-4,4,
				-aspect_ratio,aspect_ratio,-1,10);

//		printf("uniform wvp %d\n",
//				glGetUniformLocation(programs[shader.active_program_ix].gid,"utex"));

		glUniformMatrix4fv(shader_umtx_wvp,1,0,mtx_wvp);

		glClearColor(c.red,c.green,c.blue,1.0);

		glClear(GL_COLOR_BUFFER_BIT);

		objects_update(metrics.previous_frame_dt);

		if(draw_objects)objects_render();

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
