#pragma once
#include"../lib.h"

typedef struct ci_field{
	str type;
	str name;
}ci_field;

#define ci_field_def (ci_field){str_def,str_def}

inline static void ci_field_free(ci_field*o){
	str_free(&o->type);
	str_free(&o->name);
	free(o);
}
