#pragma once
#include"../lib.h"
#include"ci_toc.h"
#include"ci_expression.h"

typedef struct ci_expression_func_call{
	ci_expression super;
	str func_name;
	dynp/*owns ci_expression*/args;
}ci_expression_func_call;

inline static void _ci_expression_func_call_compile_(
		struct ci_expression*o,ci_toc*toc){
	printf(" compile statement \n");
}

#define ci_expression_func_call_def (ci_expression_func_call){\
	{_ci_expression_func_call_compile_},str_def,dynp_def}

inline static void ci_expression_func_call_free(ci_expression_func_call*o){
	ci_expression_free(&o->super);
//	dynp_foa(&o->args,{
//			ci_experssion_func_call_arg_free((ci_experssion_func_call_arg_free*)o);
//	});
	free(o);
}

inline static /*gives*/ci_expression*ci_expression_next(
		ci_toc*toc,const char**pp);

inline static /*gives*/ ci_expression_func_call*ci_expression_func_call_next(
		ci_toc*toc,const char**pp){
	token t=token_next(pp);
	ci_expression_func_call*o=malloc(sizeof(ci_expression_func_call));
	*o=ci_expression_func_call_def;
	token_copy_to_str(&t,&o->func_name);
	if(**pp=='('){// arguments
		(*pp)++;
		while(1){
			if(**pp==')'){
				(*pp)++;
				break;
			}
			ci_expression*a=ci_expression_next(toc,pp);
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
