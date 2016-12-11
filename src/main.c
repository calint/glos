#include"lib.h"
#include"gfx.h"
#include"obj.h"
#include"metrics.h"

struct{
	int*keybits_ptr;
}game;

#include"app/init.h"
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
	shader_load_program_from_source(vtx,frag,/*gives*/attrs);

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
	shader_load_program_from_source(vtx,frag,/*gives*/attrs);
}

//static float ground_base_y=.25f;
//
//inline static void main_init_scene(){
//	glos_load_scene_from_file("obj/skydome.obj");
//	glos_load_scene_from_file("obj/board.obj");
//	glos_load_scene_from_file("obj/blip.obj");
//
//	for(float y=0;y<5;y++){
//		for(float z=-10;z<=10;z++){
//			for(float x=-10;x<=10;x+=1){
//				object*o=object_alloc(&ninja_def);
//				o->n.glo=glo_at(2);
//				o->p.p=(position){x,y+ground_base_y,z,0};
//				bvol_update_radius_using_scale(&o->b);
//			}
//		}
//	}
//}

inline static void main_init(){
	main_init_programs();
	main_init_scene();
	shader.active_program_ix=0;
}

static int draw_default=0,
		draw_objects=1,
		draw_glos=1;

static struct{
	GLclampf red;
	GLclampf green;
	GLclampf blue;
}c={.1f,.1f, 0};


inline static void main_render(){
	camera_update_matrix_wvp();

	glUniformMatrix4fv(shader_umtx_wvp,1,0,camera.mxwvp);

	glClearColor(c.red,c.green,c.blue,1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	objects_update(metrics.fps.dt);

	if(draw_objects)objects_render();

	if(draw_glos)glos_render();

	if(draw_default)shader_render();

	SDL_GL_SwapWindow(window.ref);

	metrics__at__update_frame_end(stderr);
}

//------------------------------------------------------------------------ main
int main(int argc,char*argv[]){
	while(argc--)printf("%s ",*argv++);
	printf("\n--- - - - ---- - - -- - - - - -- - - -- - - - -- - - - - - -");
	printf("\n   g l o s                                   -- - -- --- - -");
	printf("\n- - - -- - --- - --  -- - - - - - -- - - - --  -- -- - - -- ");
	puts("");
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9s :\n","type",           "bytes");
	printf(":-%15s-:-%-9s-:\n","---------------","---------");
	printf(": %15s : %-9ld :\n","part",sizeof(part));
	printf(": %15s : %-9ld :\n","object",sizeof(object));
	printf(": %15s : %-9ld :\n","glo",sizeof(glo));
	printf(":-%15s-:-%-9s-:\n","---------------","--------");
	puts("");

	unsigned gloid=0,mingloid=0,maxgloid=10;
	sdl_init();
	window_init();
	shader_init();
	objects_init();
	glos_init();
	main_init();

	{
		puts("");
		program*p=dynp_get(&programs,shader.active_program_ix);
		gid progid=p->id;
		glUseProgram(progid);
		dyni*enable=&p->attributes;
		printf(" * using program at index %u\n",shader.active_program_ix);
		for(unsigned i=0;i<enable->count;i++){
			const unsigned ix=(unsigned)dyni_get(enable,i);
//			printf("   * disable vertex attrib array %d\n",ix);
			glEnableVertexAttribArray(ix);
		}
		puts("");
	}
	gid previous_active_program_ix=shader.active_program_ix;
	const float rad_over_degree=2.0f*PI/360.0f;
	float rad_over_mouse_pixels=rad_over_degree*.02f;
	int32_t keybits=0;
	float mouse_sensitivity=1.5f;
	float speed=1;
	int mouse_mode=0;
	printf("   * \n");
	printf("   * press space to grab and ungrab mouse\n");
	printf("   * \n");
	SDL_SetRelativeMouseMode(mouse_mode);

	metrics.fps.calculation_intervall_ms=100;
	metrics_reset_timer();
	metrics_print_headers(stderr);
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
			case SDL_QUIT:running=0;break;
			case SDL_MOUSEMOTION:{
//				printf(" relx,rely=%d,%d    look angle:%f\n",
//						event.motion.xrel,event.motion.yrel,
//						look_angle_z_axis*180/PI);
				if(event.motion.xrel!=0){
					look_angle_y+=(float)event.motion.xrel
							*rad_over_mouse_pixels*mouse_sensitivity;
				}
				if(event.motion.yrel!=0){
					look_angle_x+=(float)event.motion.yrel
							*rad_over_mouse_pixels*mouse_sensitivity;
				}
				break;}
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						mouse_mode=!mouse_mode;
						SDL_SetRelativeMouseMode(mouse_mode);
//						running=0;
						break;
					case SDLK_w:keybits|=1;break;
					case SDLK_a:keybits|=2;break;
					case SDLK_s:keybits|=4;break;
					case SDLK_d:keybits|=8;break;
					case SDLK_q:keybits|=16;break;
					case SDLK_e:keybits|=32;break;
					case SDLK_o:keybits|=64;break;
					case SDLK_1:draw_default=1;	break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_w:keybits&=~1;break;
					case SDLK_a:keybits&=~2;break;
					case SDLK_s:keybits&=~4;break;
					case SDLK_d:keybits&=~8;break;
					case SDLK_q:keybits&=~16;break;
					case SDLK_e:keybits&=~32;break;
					case SDLK_o:keybits&=~64;break;
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
						object_at(0)->n.glo_ptr=glo_at(gloid);
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

		if(game.keybits_ptr)
			*game.keybits_ptr=keybits;

//		printf("   camera: %f  %f  %f   angle: %f\n",
//				camera.eye.x,camera.eye.y,camera.eye.z,look_angle_z_axis*180/PI);

		vec4 lookvector=vec4_def;
		const float a=look_angle_y;
//		printf(" look angle z=%f\n",a);
		const float move_vector_scale=10;
		lookvector.x=move_vector_scale*sinf(a);
		lookvector.y=0;
		lookvector.z=-move_vector_scale*cosf(a);

//		vec4 lookvector;
//		mat4_get_zaxis(camera.mxwvp,&lookvector);
//		vec4_negate(&lookvector);
//		vec4_normalize(&lookvector);//? 3x3 orthonorm?
//		vec4_scale(&lookvector,10);
//
		camera.lookat=lookvector;
		vec4 xaxis;
		vec4 up={0,1,0,0};
		vec3_cross(&xaxis,&lookvector,&up);
		vec3_normalize(&xaxis);
		vec3_scale(&xaxis,move_vector_scale);

		const float dt=metrics.fps.dt;
		if(keybits&1)vec3_inc_with_vec3_over_dt(&camera.eye,&lookvector,dt);
		if(keybits&2)vec3_inc_with_vec3_over_dt(&camera.eye,&xaxis,-dt);
		if(keybits&4)vec3_inc_with_vec3_over_dt(&camera.eye,&lookvector,-dt);
		if(keybits&8)vec3_inc_with_vec3_over_dt(&camera.eye,&xaxis,dt);
		if(keybits&16)camera.eye.y+=speed*(dt);
		if(keybits&32)camera.eye.y-=speed*(dt);
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

			glUseProgram(ap->id);

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
	metrics_print(stderr);
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
