#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_expr_loop{
	ci_expr super;
	ci_block code;
}ci_expr_loop;

inline static void _ci_expr_loop_free_(struct ci_expr*oo){
	ci_expr_loop*o=(ci_expr_loop*)oo;
	ci_block_free(&o->code);
	ci_expr_free(&o->super);
}

inline static void _ci_ci_expr_loop_compile_(struct ci_expr*oo,ci_toc*tc){
	ci_expr_loop*o=(ci_expr_loop*)oo;
	printf("while(1)");
	o->code.super.compile(o,tc);
}

#define ci_expr_loop_def (ci_expr_loop){\
	{str_def,_ci_ci_expr_loop_compile_,_ci_expr_loop_free_},ci_block_def}
