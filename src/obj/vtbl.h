#pragma once
typedef struct object object;
typedef struct vtbl{
	void(*init)(object*);
	void(*update)(object*,dt);
	void(*collision)(object*,object*,dt);
	void(*render)(object*);
	void(*free)(object*);
}vtbl;
#define vtbl_def {0,0,0,0,0}

