#pragma once
typedef struct part{
	void(*init)(object*,part*);
	void(*update)(object*,part*,dt);
	void(*render)(object*,part*);
	void(*free)(object*,part*);
}part;

