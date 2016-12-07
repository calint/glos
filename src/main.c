#include"sdl.h"
#include"lib.h"
#include"window.h"
#include"shader.h"
#include"objects.h"
#include"obj_file.h"
#include"object_alloc.h"
#include"glob.h"
#include"metrics.h"
#include"dyni.h"
#include"app/ninja.h"
//----------------------------------------------------------------------- init
inline static void main_init(){

	const char*vsrc="#version 130\n\
uniform mat4 umtx_mw;\n\
in vec3 apos;\n\
void main(){\n\
	gl_Position=umtx_mw*vec4(apos,1);\n\
}\n";

	const char*fsrc="#version 130\n\
void main(){\n\
	gl_FragColor=vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1);\n\
}";

	dyni attrs=dyni_def;
	dyni_add(&attrs,shader_apos);
	programs_load(1,vsrc,fsrc,/*gives*/attrs);
//	shader.active_program_ix=1;

	globs_load_obj_file(1,"obj/plane.obj");
	globs_load_obj_file(2,"obj/ico_sphere.obj");
	globs_load_obj_file(3,"obj/cylinder.obj");
	globs_load_obj_file(4,"obj/grid_8x8.obj");
	globs_load_obj_file(5,"obj/sphere.obj");
	globs_load_obj_file(6,"obj/torus.obj");
	globs_load_obj_file(7,"obj/disc.obj");

	object*o;
	o=object_alloc(&ninja_def);
	o->glob_id=1;
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
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9s :\n","type",           "bytes");
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9ld :\n","part",sizeof(part));
	printf(": %15s : %-9ld :\n","object",sizeof(object));
	printf(": %15s : %-9ld :\n","objects",sizeof(objects));
	printf(": %15s : %-9ld :\n","glob",sizeof(glob));
	printf(": %15s : %-9ld :\n","globs",sizeof(globs));
	printf(":-%15s-:-%-9s-:\n","---------------","--------");

	sdl_init();
	window_init();
	shader_init();
	objects_init();
	globs_init();
	main_init();

	arrayix program_id_min=0;
	arrayix program_id_max=1;

	struct{
		GLclampf red;
		GLclampf green;
		GLclampf blue;
	}c={.11f,.37f,1};

	int draw_default=0,draw_objects=1;
	puts("");
	metrics_print_headers();
	{
		program*p=&programs[shader.active_program_ix];
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
						object*o=object_at(0);
						o->glob_id++;
						if(o->glob_id>8){
							o->glob_id=1;
						}
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

			dyni disable=programs[previous_active_program_ix]
								  .attributes;

			for(unsigned i=0;i<disable.count;i++){
				const unsigned ix=(unsigned)dyni_get(&disable,i);
				printf("   * disable vertex attrib array %d\n",ix);
				glDisableVertexAttribArray(ix);
			}

			const unsigned progid=programs[shader.active_program_ix].gid;

			glUseProgram(progid);

			program*p=&programs[shader.active_program_ix];
			dyni*enable=&p->attributes;

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

		float aspect_ratio=1;
		mat4_load_ortho_projection(mtx_wvp, -1,1,
				-aspect_ratio,aspect_ratio,  0,2);

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
	globs_free();
	objects_free();
	shader_free();
	window_free();
	sdl_free();
	metrics_print();
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
