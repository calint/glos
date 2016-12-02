#pragma once
#include "../object.h"
#include "../parts/anim.h"

//----------------------------------------------------------------------- init

inline static void init_ninja(object*this) {
	this->position=(position){
		100,
		(float)(window_height-texture[10].height),
		0,0
	};

	this->velocity=(position){5,10,0,0};

	this->scale=(scale){
		(float)texture[10].width,
		(float)texture[10].height,
		1,0
	};

	update_bounding_radius_using_scale(this);


	animator_part*a=/**takes*/alloc_animator_part(NULL);
	a->time_duration_per_frame=0.5f;
	a->current_texture_index=
	a->texture_index_for_first_frame=10;
	a->texture_index_for_last_frame=19;
	a->time_duration_per_frame=.2f;
	this->extension=a;
}

//----------------------------------------------------------------------- free

inline static void free_ninja(object*this){free(this->extension);}

//----------------------------------------------------------------------update

inline static void update_ninja(object*this,dt_in_seconds dt){
//	printf(" ninja y: %f\n",this->position.y);
	if(this->position.y>620){
		this->velocity.y=-this->velocity.y;
	}else if(this->position.y<600){
		this->velocity.y=-this->velocity.y;
	}

	animator_part*a=animator_part_(this->extension);
	update_anim(a,dt);
	this->texture_id=a->current_texture_index;
}

//---------------------------------------------------------------------------

static object ninja={
	.type={{'e',0,0,0,0,0,0,0}},
	.init=init_ninja,
	.free=free_ninja,
	.texture_id=10,
	.update=update_ninja,
	.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
