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
	glos_load_scene_from_file("obj/skydome.obj");
	glos_load_scene_from_file("obj/blip.obj");

//	glos_load_scene_from_file("obj/color-cube.obj");
//	glos_load_scene_from_file("obj/board.obj");
//
//	object*o=object_alloc(&ninja_def);
//	o->glo=glo_at(0);
}

inline static void main_init(){
	main_init_programs();
	main_init_scene();
	shader.active_program_ix=0;
}



static int draw_default=0,
		draw_objects=0,
		draw_glos=1;

static struct{
	GLclampf red;
	GLclampf green;
	GLclampf blue;
}c={.1f,.1f,.4f};

inline static void main_render(){
	camera_update_matrix_wvp();

	glUniformMatrix4fv(shader_umtx_wvp,1,0,camera.mxwvp);

	glClearColor(c.red,c.green,c.blue,1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	objects_update(metrics.previous_frame_dt);

	if(draw_objects)objects_render();

	if(draw_glos)glos_render();

	if(draw_default)shader_render();

	SDL_GL_SwapWindow(window.ref);

	metrics__at__update_frame_end();
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
//
//	indx program_id_min=0;
//	indx program_id_max=2;
//

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
	const float rad_over_degree=2.0f*PI/360.0f;
	float rad_over_mouse_pixels=rad_over_degree*.02f;
	float look_angle_z_axis=0;
	float look_angle_x_axis=0;
	int64_t keymap=0;
	float sensitivity=1.3f;
	float speed=1;
	int mouse_mode=0;
	SDL_SetRelativeMouseMode(mouse_mode);
	for(int running=1;running;){
		metrics__at__frame_begin();
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			switch(event.type) {
			case SDL_WINDOWEVENT:{
				switch(event.window.event){
				case SDL_WINDOWEVENT_SIZE_CHANGED:{
					SDL_Log("Window %d size changed to %dx%d",
							event.window.windowID, event.window.data1,
							event.window.data2);
					int w,h;
					SDL_GetWindowSize(window.ref,&w,&h);// ? glGet
					camera.wi=(float)w;
					camera.hi=(float)h;
					printf(" * resize to  %u x %u\n",w,h);
					glViewport(0,0,w,h);
//					SDL_Rect r={0,0,w,h};
//					SDL_RenderSetViewport(window.renderer,&r);
					break;}
				}
				break;
			}
			case SDL_QUIT:running = 0;break;
			case SDL_MOUSEMOTION:{
//				printf(" relx,rely=%d,%d    look angle:%f\n",
//						event.motion.xrel,event.motion.yrel,
//						look_angle_z_axis*180/PI);
				if(event.motion.xrel!=0){
					look_angle_z_axis+=(float)event.motion.xrel
							*rad_over_mouse_pixels*sensitivity;
				}
				if(event.motion.xrel!=0){
					look_angle_x_axis+=(float)event.motion.xrel
							*rad_over_mouse_pixels*sensitivity;
				}
				break;}
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:running=0;break;
					case SDLK_w:keymap|=1;break;
					case SDLK_a:keymap|=2;break;
					case SDLK_s:keymap|=4;break;
					case SDLK_d:keymap|=8;break;
					case SDLK_q:keymap|=16;break;
					case SDLK_e:keymap|=32;break;
					case SDLK_o:keymap|=64;break;
					case SDLK_1:draw_default=1;	break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_w:keymap&=~1;break;
					case SDLK_a:keymap&=~2;break;
					case SDLK_s:keymap&=~4;break;
					case SDLK_d:keymap&=~8;break;
					case SDLK_q:keymap&=~16;break;
					case SDLK_e:keymap&=~32;break;
					case SDLK_o:keymap&=~64;break;
					case SDLK_SPACE:
						mouse_mode=!mouse_mode;
						SDL_SetRelativeMouseMode(mouse_mode);
						break;
					case SDLK_1:draw_default=0;break;
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
						if(shader.active_program_ix>=programs.count){
							shader.active_program_ix=0;
						}
						break;
					}
					case SDLK_4:camera.ortho=!camera.ortho;break;
					case SDLK_TAB:
						printf("   camera: %f  %f  %f",
								camera.eye.x,camera.eye.y,camera.eye.z);
//						camera.ortho=!camera.ortho;
						break;
				}
				break;
			}
		}

//		printf("   camera: %f  %f  %f   angle: %f\n",
//				camera.eye.x,camera.eye.y,camera.eye.z,look_angle_z_axis*180/PI);

		vec4 lookvector=vec4_def;
		const float a=look_angle_z_axis;
//		printf(" look angle z=%f\n",a);
		const float s=1.0f;
		lookvector.x=s*sinf(a);
		lookvector.y=0;
		lookvector.z=-s*cosf(a);

//		vec4 lookvector;
//		mat4_get_zaxis(camera.mxwvp,&lookvector);
//		vec4_negate(&lookvector);
//		vec4_normalize(&lookvector);//? 3x3 orthonorm?
//		vec4_scale(&lookvector,10);
//
		camera.lookat=lookvector;
		const float dt=metrics.previous_frame_dt;
		if(keymap&1)vec4_increase_with_vec4_over_dt(&camera.eye,&lookvector,dt*.5f);
		if(keymap&2)camera.eye.x-=speed*(dt);
		if(keymap&4)vec4_increase_with_vec4_over_dt(&camera.eye,&lookvector,-dt*.5f);
		if(keymap&8)camera.eye.x+=speed*(dt);
		if(keymap&16)camera.eye.y+=speed*(dt);
		if(keymap&32)camera.eye.y-=speed*(dt);
//		printf("  %f  %f  %f  \n",
//				camera.lookat.x,camera.lookat.y,camera.lookat.z);
//		printf("  %f  %f  %f  \n",
//					camera.eye.x,camera.eye.y,camera.eye.z);

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

		main_render();
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
