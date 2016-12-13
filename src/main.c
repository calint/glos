#include"lib.h"
#include"gfx.h"
#include"metrics.h"
#include"net.h"
#include"grid.h"
#include"obj/objects.h"

static struct _game{
	int*keybits_ptr;
	const object*follow_ptr;
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
		draw_glos=0,do_main_render=1;

static struct color{
	GLclampf red;
	GLclampf green;
	GLclampf blue;
}bg={.1f,.1f, 0};


inline static void main_render(framectx*fc){
	camera_update_matrix_wvp();

	glUniformMatrix4fv(shader_umtx_wvp,1,0,camera.mxwvp);

	glClearColor(bg.red,bg.green,bg.blue,1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(draw_objects)grid_render(fc);
	if(draw_glos)glos_render();
	if(draw_default)shader_render();

	SDL_GL_SwapWindow(window.ref);
}

//------------------------------------------------------------------------ main
int main(int argc,char*argv[]){
	if(argc>1 && *argv[1]=='s'){
		netsrv_init();
		netsrv_loop();
		netsrv_free();
	}

	int use_net=0;
	if(argc>1 && *argv[1]=='c'){
		srand(0);
		use_net=1;
		if(argc>2){
			net_host=argv[2];
		}
	}

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
	if(use_net)net_init();
	metrics_init();
	sdl_init();
	window_init();
	shader_init();
//	objects_init();
	glos_init();
	grid_init();
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
	float mouse_sensitivity=1.5f;
	int mouse_mode=0;
//	printf("   * \n");
//	printf("   * press space to grab and ungrab mouse\n");
//	printf("   * \n");
	SDL_SetRelativeMouseMode(mouse_mode);

	metrics.fps.calculation_intervall_ms=100;
	metrics_reset_timer();
	metrics_print_headers(stderr);
	unsigned frameno=1;
	for(int running=1;running;){
		metrics__at__frame_begin();
		if(use_net){
			net_state_to_send.lookangle_y=camera_lookangle_y;
			net_state_to_send.lookangle_x=camera_lookangle_x;
			net__at__frame_begin();
		}

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
					camera_lookangle_y+=(float)event.motion.xrel
							*rad_over_mouse_pixels*mouse_sensitivity;
				}
				if(event.motion.yrel!=0){
					camera_lookangle_x+=(float)event.motion.yrel
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
					case SDLK_w:net_state_to_send.keybits|=1;break;
					case SDLK_a:net_state_to_send.keybits|=2;break;
					case SDLK_s:net_state_to_send.keybits|=4;break;
					case SDLK_d:net_state_to_send.keybits|=8;break;
					case SDLK_q:net_state_to_send.keybits|=16;break;
					case SDLK_e:net_state_to_send.keybits|=32;break;
					case SDLK_o:net_state_to_send.keybits|=64;break;
					case SDLK_1:draw_default=1;	break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym){
					case SDLK_w:net_state_to_send.keybits&=~1;break;
					case SDLK_a:net_state_to_send.keybits&=~2;break;
					case SDLK_s:net_state_to_send.keybits&=~4;break;
					case SDLK_d:net_state_to_send.keybits&=~8;break;
					case SDLK_q:net_state_to_send.keybits&=~16;break;
					case SDLK_e:net_state_to_send.keybits&=~32;break;
					case SDLK_o:net_state_to_send.keybits&=~64;break;
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
					case SDLK_4:camera.type=!camera.type;break;
					case SDLK_TAB:
						printf("   camera: %f  %f  %f",
								camera.eye.x,camera.eye.y,camera.eye.z);
//						camera.ortho=!camera.ortho;
						break;
				}
				break;
			}
		}

		if(!use_net){
			net_state_current[net_active_player_index].keybits=
					net_state_to_send.keybits;
		}
		if(game.keybits_ptr)
			*game.keybits_ptr=net_state_current[net_active_player_index].keybits;

		if(game.follow_ptr){
			camera.lookat=game.follow_ptr->p.p;
		}


//		printf("   camera: %f  %f  %f   angle: %f\n",
//				camera.eye.x,camera.eye.y,camera.eye.z,look_angle_z_axis*180/PI);

//		vec4 lookvector=vec4_def;
//		const float a=camera_lookangle_y;
////		printf(" look angle z=%f\n",a);
//		const float move_vector_scale=10;
//		lookvector.x=move_vector_scale*sinf(a);
//		lookvector.y=0;
//		lookvector.z=-move_vector_scale*cosf(a);

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

		grid_clear();

//		objects_foreach_allocated_all(foo(void,(object*o){
//			grid_add(o);
//		}));

		objects_foreach_allocated_all(foa_object({
			grid_add(o);
		}));

		framectx fc={
				.dt=use_net?net_dt:metrics.fps.dt,
				.tick=frameno,
		};

		frameno++;

		grid_update(&fc);

		if(do_main_render)main_render(&fc);

		if(use_net)net__at__frame_end();

		metrics__at__frame_end(stderr);
	}
	//---------------------------------------------------------------------free
	//? early-hangup
	grid_free();
	glos_free();
	objects_free();
	shader_free();
	window_free();
	sdl_free();
	if(use_net)net_free();
	metrics_print(stderr);
	metrics_free();
	puts(" * clean exit");
	return 0;
}
//----------------------------------------------------------------------------
