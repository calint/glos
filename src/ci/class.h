#pragma once
#include"../lib.h"
#include "func.h"
#include "member.h"
typedef struct ci_class{
	str name;
	dynp/*owns str*/extends;
	dynp/*owns ci_slot*/members;
	dynp/*owns ci_func*/functions;
}ci_class;

#define ci_class_def (ci_class){str_def,dynp_def,dynp_def,dynp_def}

inline static void ci_class_free(ci_class*o){

	dynp_foa(&o->functions,{
		ci_func_free((ci_func*)o);
	});
	dynp_free(&o->functions);

	dynp_foa(&o->members,{
		ci_member_free((ci_member*)o);
	});
	dynp_free(&o->members);

	dynp_foa(&o->extends,{
		str_free((str*)o);
	});
	dynp_free(&o->extends);

	str_free(&o->name);

	free(o);
}

