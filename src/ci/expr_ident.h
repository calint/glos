#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_expr_ident{
	ci_expr super;
	str name;
}ci_expr_ident;

inline static void _ci_expr_ident_free_(struct ci_expr*oo){
	ci_expr_ident*o=(ci_expr_ident*)oo;
	str_free(&o->name);
	ci_expr_free((ci_expr*)o);
}

inline static void _ci_expr_ident_compile_(
		struct ci_expr*oo,ci_toc*tc){
	ci_expr_ident*o=(ci_expr_ident*)oo;
	if(ci_toc_has_identifier(tc,o->name.data)){
		printf("%s",o->name.data);
		return;
	}
	// constant
	printf("%s",o->name.data);
}

#define ci_expr_ident_def (ci_expr_ident){\
	{_ci_expr_ident_compile_,_ci_expr_ident_free_},str_def}

//inline static void ci_expr_ident_free(ci_expr_ident*o){
//	ci_expression_free(&o->super);
//	free(o);
//}
