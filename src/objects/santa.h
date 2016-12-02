#pragma once
#include "../object.h"
#include "../parts/anim.h"
//--------------------------------------------------------------------- logo_1

#define santa_texture_on_ground_bias 131

inline static void init_santa(object*this) {
	this->position=(position){
			(float)(window_width-texture[this->texture_id].width/2),
			(float)(window_height-texture[this->texture_id].height
						+santa_texture_on_ground_bias
					),
			0,0
		};

	this->scale=(scale){
			(float)texture[this->texture_id].width,
			(float)texture[this->texture_id].height,
			1,0
		};

	set_bounding_radius_from_xy_scale(this);



	animator_part*a=malloc(sizeof(animator_part));
	*a=anim;

	a->current_index_in_textures=a->texture_index_for_first_frame=20;
	a->texture_index_for_last_frame=35;
	a->time_duration_per_frame=.2f;
	a->current_index_in_textures=a->texture_index_for_first_frame;
	a->animation_time_left_for_current_frame=0;

	this->extension=a;
}

//---------------------------------------------------------------------------

inline static void update_santa(object*this,dt dt){

	this->scale.x=this->scale.x+11*dt;
	this->scale.y=this->scale.y+11*dt;

	set_bounding_radius_from_xy_scale(this);


	animator_part*a=(animator_part*)this->extension;
	update_anim(a,dt);
	this->texture_id=a->current_index_in_textures;
}

//---------------------------------------------------------------------------

static object santa={
		.type={{'f',0,0,0,0,0,0,0}},
		.texture_id=20,
		.init=init_santa,
		.update=update_santa,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
