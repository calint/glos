#pragma once
#include "types.h"
//--------------------------------------------------------------------- object
typedef struct object {
	position pos;
	position dpos;
	angle agl;
	angle dagl;
	void (*render)();
	struct object*parent;
} object;

#define objects_count 1
static object objects[objects_count];

//--------------------------------------------------------------------- init

inline static void object_render() {
	shader_render();
}

inline static void objects_init() {
	objects[0].pos.x = 100;
	objects[0].pos.y = 100;
	objects[0].render = object_render;
}
//--------------------------------------------------------------------- free
inline static void objects_free() {
}
//--------------------------------------------------------------------- update
inline static void objects_update() {
	object*p = objects;
	int i = objects_count;
	while (i--) {
		p->pos.x += p->dpos.x;
		p->pos.y += p->dpos.y;
		p->pos.z += p->dpos.z;
		p->agl.x += p->dagl.x;
		p->agl.y += p->dagl.y;
		p->agl.x += p->dagl.z;
		p++;
	}
}
//--------------------------------------------------------------------- render
inline static void objects_render() {
	object*p = objects;
	int i = objects_count;
	while (i--) {
		if(p->render)
			p->render();
		p++;
	}
}
//--------------------------------------------------------------------- done
