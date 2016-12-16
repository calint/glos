#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"
#include"block.h"
#include "expr_bool.h"
typedef struct ci_expr_if{
	ci_expr super;
	ci_expr_bool cond;
}ci_expr_if;

inline static void _ci_expr_if_compile_(const ci_expr*oo,ci_toc*tc){
	printf("if");
}

inline static void _ci_expr_if_free_(ci_expr*oo){
	ci_expr_free(oo);
}

#define ci_expr_if_def (ci_expr_if){\
	{str_def,_ci_expr_if_compile_,_ci_expr_if_free_}}

inline static ci_expr*ci_expr_if_next(const char**pp,ci_toc*tc){
	if(**pp!=';'){
		printf("\n\n<file> <line:col> expected ';' after keyword 'break'");
		exit(1);
	}
	(*pp)++;
	ci_expr_if*e=malloc(sizeof(ci_expr_if));
	*e=ci_expr_if_def;
	return(ci_expr*)e;
}
