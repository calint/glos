#pragma once
#include"../lib.h"
#include"ci_toc.h"
#include"ci_expression.h"

typedef struct ci_expression_identifier{
	ci_expression super;
	str name;
}ci_expression_identifier;

inline static void _ci_expression_identifier_compile_(
		struct ci_expression*oo,ci_toc*toc){
	ci_expression_identifier*o=(ci_expression_identifier*)oo;
	printf("%s",o->name.data);
}

#define ci_expression_identifier_def (ci_expression_identifier){\
	{_ci_expression_identifier_compile_},str_def}

inline static void ci_expression_identifier_free(ci_expression_identifier*o){
	ci_expression_free(&o->super);
	free(o);
}
