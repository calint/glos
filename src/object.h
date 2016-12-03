#pragma once
#include<SDL2/SDL2_gfxPrimitives.h>
#include"lib.h"
#include"window.h"
#include"textures.h"
#include"drawables.h"

//--------------------------------------------------------------------- object

typedef struct object{

	position position;

	velocity velocity;

	angle angle;

	angular_velocity angular_velocity;

	bounding_radius bounding_radius;

	scale scale;

	type type;

	id id;

	id texture_id;

	color color;

	bits*bits;

	id drawable_id;

	float model_to_world_matrix[4*4];

	int model_to_world_matrix_is_updated;

	void(*init)(struct object*);

	void(*update)(struct object*,dt);

	void(*collision)(struct object*,struct object*,dt);

	void(*render)(struct object*);

	void(*free)(struct object*);

	void*extension;

}object;

//-------------------------------------------------------------------- default
//	if(not model_to_world_matrix_needs_update)
//		return;

static object default_object={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.scale={0,0,0,0},
	.texture_id=0,
	.bounding_radius=0,
	.bits=0,
	.type={{0,0,0,0,0,0,0,0}},
	.id=0,
	.color=0,
	.drawable_id=0,
	.init=NULL,
	.update=NULL,
	.collision=NULL,
	.render=NULL,
	.free=NULL,
};

inline static void object_update_physics(object*this,dt dt){
	add_vec4_over_dt(&this->position,&this->velocity,dt);
	add_vec4_over_dt(&this->angle,&this->angular_velocity,dt);
	if(this->velocity.x || this->velocity.y || this->velocity.z ||
		this->angular_velocity.x || this->angular_velocity.y ||
		this->angular_velocity.z
	){
		this->model_to_world_matrix_is_updated=0;
	}
}

inline static void object_update_model_to_world_matrix(object*this){
	if(this->model_to_world_matrix_is_updated)
		return;

	mat4_load_translate(this->model_to_world_matrix,&this->position);
	mat4_append_rotation_about_z_axis(this->model_to_world_matrix,this->angle.z);

	this->model_to_world_matrix_is_updated=1;

//	model_to_world_matrix_.append_scaling(phy.s);
//	model_to_world_matrix_needs_update=false;
//		p(" [%u]  update_model_to_world_matrix %p\n",gl::time_stamp,this);
}

//----------------------------------------------------------- ------ functions

inline static void update_bounding_radius_using_scale(object*o) {
	o->bounding_radius=(bounding_radius)
		sqrtf(o->scale.x*o->scale.x+o->scale.y*o->scale.y);
}

//----------------------------------------------------------------------------

inline static void draw_texture(object*o) {
	SDL_Rect d={
		(int)(o->position.x-o->scale.x),
		(int)(o->position.y-o->scale.y),
		(int)o->scale.x*2,
		(int)o->scale.y*2
	};
	SDL_RenderCopy(window.renderer,texture[o->texture_id].ptr,NULL,&d);
}

//----------------------------------------------------------------------------

inline static void draw_bounding_sphere(object*o) {
	circleColor(
		window.renderer,
		(short)o->position.x,
		(short)o->position.y,
		(short)o->bounding_radius,
		o->color
	);
}

//----------------------------------------------------------------------------

inline static void draw_texture_and_bounding_sphere(object*o) {
	draw_texture(o);
	draw_bounding_sphere(o);
}
//----------------------------------------------------------------------------

inline static void render_drawable(object*o) {
	if(o->drawable_id){
		object_update_model_to_world_matrix(o);
		glUniformMatrix4fv((signed)shader.model_to_world_matrix_slot,1,0,
				o->model_to_world_matrix);
		draw_drawable(o->drawable_id);
	}
}
//----------------------------------------------------------------------------
