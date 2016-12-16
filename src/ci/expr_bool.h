#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"
#include"block.h"
typedef struct ci_expr_bool{
	ci_expr super;

	struct ci_expr*lh;

	char op;

	struct ci_expr*rh;

	str bool_op_list;

	dynp/*owns &expr_bool*/bool_list;

	bool is_encapsulated;

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
	dynp_def,\
	0}

inline static void ci_expr_bool_parse(ci_expr_bool*o,
		const char**pp,ci_toc*tc){

	o->super.type=str_from_string("bool");
	if(**pp!='('){//   keybits==1 && ok || (a&b!=0)
		o->is_encapsulated=false;
		token l=token_next(pp);
		char op=0;
		if(**pp=='='){
			(*pp)++;
			if(**pp!='='){
				printf("<file> <line:col> expected '=='\n");
				exit(1);
			}
			op='=';
		}
	}
	o->is_encapsulated=true;
	(*pp)++;
	ci_expr_bool*e=malloc(sizeof(ci_expr_bool));
	*e=ci_expr_bool_def;
	ci_expr_bool_parse(o,pp,tc);
}
