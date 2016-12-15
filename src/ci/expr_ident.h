#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_expression_identifier{
	ci_expression super;
	str name;
}ci_expr_ident;

inline static void _ci_expression_identifier_free_(struct ci_expression*oo){
	ci_expr_ident*o=(ci_expr_ident*)oo;
	str_free(&o->name);
	ci_expression_free((ci_expression*)o);
}

inline static void _ci_expression_identifier_compile_(
		struct ci_expression*oo,ci_toc*toc){
	ci_expr_ident*o=(ci_expr_ident*)oo;
	printf("%s",o->name.data);
}

#define ci_expr_ident_def (ci_expr_ident){\
	{_ci_expression_identifier_compile_,_ci_expression_identifier_free_},str_def}

inline static void ci_expression_identifier_free(ci_expr_ident*o){
	ci_expression_free(&o->super);
	free(o);
}
