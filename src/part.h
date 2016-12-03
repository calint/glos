#pragma once
typedef struct part{
	void(*init)(object*,struct part*);
	void(*update)(object*,struct part*,dt);
	void(*render)(object*,struct part*);
	void(*free)(object*,struct part*);
}part;

