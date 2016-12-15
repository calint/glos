#pragma once
#include"../lib.h"
#include "expr.h"
#include "func_arg.h"
#include "ci_code.h"
typedef struct ci_func{
	str type;
	str name;
	dynp/*owns ci_func_arg**/args;
	ci_code code;
}ci_func;

#define ci_func_def (ci_func){str_def,str_def,dynp_def,ci_code_def}

inline static void ci_func_free(ci_func*o){
	dynp_foa(&o->args,{
		ci_func_arg_free((ci_func_arg*)o);
	});
	dynp_free(&o->args);

	str_free(&o->name);
	str_free(&o->type);
	free(o);
}
