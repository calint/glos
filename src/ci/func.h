#pragma once
#include"../lib.h"
#include "expr.h"
#include "func_arg.h"
typedef struct ci_func{
	str type;
	str name;
	dynp/*owns ci_func_arg**/args;
	dynp/*owns ci_expression**/exprs;
}ci_func;

#define ci_func_def (ci_func){str_def,str_def,dynp_def,dynp_def}

inline static void ci_func_free(ci_func*o){
	dynp_foa(&o->exprs,{
		ci_expr*oo=(ci_expr*)o;
		if(oo->free)
			oo->free(oo);
		else
			ci_expr_free(oo);
	});
	dynp_free(&o->exprs);

	dynp_foa(&o->args,{
		ci_func_arg_free((ci_func_arg*)o);
	});
	dynp_free(&o->args);

	str_free(&o->name);
	str_free(&o->type);
	free(o);
}
