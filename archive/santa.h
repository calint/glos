#pragma once
#include"../object.h"
#include"../parts/animator_part.h"
//--------------------------------------------------------------------- logo_1

#define santa_texture_on_ground_bias 211

inline static void init_santa(object*this) {
	this->position=(position){
		(float)(window_width-texture[this->texture_id].width/2),
		(float)(window_height-texture[this->texture_id].height
				+santa_texture_on_ground_bias),
		0,0
	};

	this->scale=(scale){
		(float)texture[this->texture_id].width,
		(float)texture[this->texture_id].height,
		1,0
	};

	object_update_bounding_radius_using_scale(this);

	animator_part*a=/**takes*/alloc_animator_part(NULL);
	a->current_texture_index=a->texture_index_for_first_frame=20;
	a->texture_index_for_last_frame=35;
	a->time_duration_per_frame=.2f;
	a->current_texture_index=a->texture_index_for_first_frame;
	a->animation_time_left_for_current_frame=a->time_duration_per_frame;
	this->parts=a;
}

//---------------------------------------------------------------------------

inline static void update_santa(object*this,dt dt){
//	accumulate_dt(&this->scale,&this->scale,dt);
	this->scale.x=this->scale.x+11*dt;
	this->scale.y=this->scale.y+11*dt;

	object_update_bounding_radius_using_scale(this);

	animator_part*a=animator_part_(this->parts);
	update_animator_part(a,dt);
	this->texture_id=a->current_texture_index;
}

//---------------------------------------------------------------------------

static object default_santa={
	.type={{'f',0,0,0,0,0,0,0}},
	.texture_id=20,
	.init=init_santa,
	.update=update_santa,
	.render=_draw_texture_and_bounding_sphere_,
};

//---------------------------------------------------------------------------
