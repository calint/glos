#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_expr_call{
	ci_expression super;
	str func_name;
	dynp/*owns ci_expression*/args;
}ci_expr_call;

inline static void _ci_expr_call_compile_(
		struct ci_expression*o,ci_toc*toc){

	ci_expr_call*e=(ci_expr_call*)o;
	printf("%s(",e->func_name.data);
	dynp_foac(&e->args,{
		ci_expression*a=(ci_expression*)o;
		a->compile(a,toc);
		if(i!=e->args.count-1){
			printf(",");
		}
	});
	printf(")");
}

#define ci_expr_call_def (ci_expr_call){\
	{_ci_expr_call_compile_,ci_expr_call_free},\
	str_def,dynp_def}

inline static void ci_expr_call_free(ci_expression*o){
	ci_expr_call*oo=(ci_expr_call*)o;
	ci_expression_free(&oo->super);
	dynp_foa(&oo->args,{
			ci_expression*e=(ci_expression*)o;
			if(e->free)
				e->free(e);
			else
				ci_expression_free(e);
	});
	free(o);
}

inline static /*gives*/ci_expression*ci_expr_next(
		ci_toc*toc,const char**pp);

inline static /*gives*/ ci_expr_call*ci_expr_call_next(
		ci_toc*toc,const char**pp){
	token t=token_next(pp);
	ci_expr_call*o=malloc(sizeof(ci_expr_call));
	*o=ci_expr_call_def;
	token_copy_to_str(&t,&o->func_name);
	if(**pp=='('){// arguments
		(*pp)++;
		while(1){
			if(**pp==')'){
				(*pp)++;
				break;
			}
			ci_expression*a=ci_expr_next(toc,pp);
			if(ci_expression_is_empty(a)){
				printf("<file> <line> <col> expected ')' or more arguments");
				exit(1);
			}
			dynp_add(&o->args,a);
			if(**pp==','){
				(*pp)++;
				continue;
			}
			if(**pp==')'){
				(*pp)++;
				break;
			}
			printf("<file> <line> <col> expected ',' followed by more arguments");
			exit(1);
		}
	}
	return o;
}
