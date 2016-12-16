#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"
#include"block.h"
typedef struct ci_expr_bool{
	ci_expr super;

	struct ci_expr_bool*lh;

	char op;

	struct ci_expr_bool*rh;

	str bool_op_list;

	dynp/*owns &expr_bool*/bool_list;

}ci_expr_bool;

inline static void _ci_expr_bool_compile_(const ci_expr*oo,ci_toc*tc){
	printf("bool");
}

inline static void _ci_expr_bool_free_(ci_expr*oo){
	ci_expr_free(oo);
}

#define ci_expr_bool_def (ci_expr_bool){\
	{str_def,_ci_expr_bool_compile_,_ci_expr_bool_free_},\
	NULL,0,NULL,\
	str_def,\
	dynp_def}

inline static ci_expr*ci_expr_bool_parse(ci_expr_bool*o,
		const char**pp,ci_toc*tc){

	o->super.type=str_from_string("bool");
	if(**pp!='('){


	}
	(*pp)++;
	ci_expr_bool*e=malloc(sizeof(ci_expr_bool));
	*e=ci_expr_bool_def;
	return(ci_expr*)e;
}
