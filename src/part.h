#pragma once
typedef struct part{
	short type_id;
	void(*init)(object*,struct part*);
	void(*update)(object*,struct part*,dt);
	void(*render)(object*,struct part*);
	void(*free)(object*,struct part*);
}part;

