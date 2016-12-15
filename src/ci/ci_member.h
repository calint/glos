#pragma once
#include"../lib.h"

typedef struct ci_member{
	str type;
	str name;
}ci_member;

#define ci_member_def (ci_member){str_def,str_def}

inline static void ci_member_free(ci_member*o){
	str_free(&o->type);
	str_free(&o->name);
	free(o);
}
