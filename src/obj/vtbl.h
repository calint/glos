#pragma once
struct object;
typedef struct vtbl{
	void(*init)(struct object*);
	void(*update)(struct object*,dt);
	void(*collision)(struct object*,struct object*,dt);
	void(*render)(struct object*);
	void(*free)(struct object*);
}vtbl;
#define vtbl_def {0,0,0,0,0}

