#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_expr_assign{
	ci_expr super;
	str name;
	ci_expr*expr;
}ci_expr_assign;

inline static void _ci_expr_assign_free_(struct ci_expr*oo){
	ci_expr_assign*o=(ci_expr_assign*)oo;
//	str_free(&o->name);
	ci_expr_free(o->expr);
}

inline static void _ci_expr_assign_compile_(
		struct ci_expr*oo,ci_toc*tc){

	ci_expr_assign*o=(ci_expr_assign*)oo;
	const char idtype=ci_toc_find_ident_type(tc,o->name.data);
	if(idtype=='c'){// class member
		printf("o->%s=",o->name.data);
		o->expr->compile(o->expr,tc);
//		printf(";\n");
		return;
	}
	if(idtype){// local identifier
		printf("%s",o->name.data);
		o->expr->compile((ci_expr*)o,tc);
		return;
	}
	// constant
	char* endptr;
	strtol(o->name.data,&endptr,10);
	if(*endptr!=0){
		// not end of string
		printf("\n\n<file> <line:col> '%s' is not found and could not "
				"be parsed to a number\n\n",o->name.data);
		exit(1);

	}
	str_setz(&o->super.type,"int");
	printf("%s=",o->name.data);
	o->expr->compile((ci_expr*)o,tc);
	printf(";\n");
}

#define ci_expr_assign_def (ci_expr_assign){\
	{str_def,_ci_expr_assign_compile_,_ci_expr_assign_free_},str_def,0}

//inline static void ci_expr_ident_free(ci_expr_ident*o){
//	ci_expression_free(&o->super);
//	free(o);
//}