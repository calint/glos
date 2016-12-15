#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"

typedef struct ci_code{
	ci_expr super;
	int is_encaps;
	dynp/*own expr*/exprs;
}ci_code;

inline static void _ci_code_free_(ci_expr*oo){
	ci_code*o=(ci_code*)oo;
	dynp_foa(&o->exprs,{
		ci_expr*oo=(ci_expr*)o;
		if(oo->free)
			oo->free(oo);
		else
			ci_expr_free(oo);
	});
	dynp_free(&o->exprs);
}

inline static void _ci_code_compile_(ci_expr*oo,ci_toc*tc){
	ci_code*o=(ci_code*)oo;
	if(o->is_encaps){
		printf("{\n");
	}
	for(unsigned i=0;i<o->exprs.count;i++){
		ci_expr*e=dynp_get(&o->exprs,i);
		e->compile(e,tc);
		printf(";\n");
	}
	if(o->is_encaps){
		printf("}\n");
	}

}

#define ci_code_def (ci_code){{_ci_code_compile_,_ci_code_free_},0,dynp_def}

//inline static void ci_expr_ident_free(ci_expr_ident*o){
//	ci_expression_free(&o->super);
//	free(o);
//}

inline static /*gives*/ci_expr*ci_expr_next(const char**,ci_toc*);

static /*gives*/ci_code*ci_code_parse(ci_code*o,const char**pp,ci_toc*tc){
//	ci_code*c=malloc(sizeof(ci_code));
	*o=ci_code_def;
	if(**pp=='{'){
		(*pp)++;
		o->is_encaps=1;
	}
	while(1){
		ci_expr*e=ci_expr_next(pp,tc);
		if(ci_expr_is_empty(e)){
			break;
		}
		dynp_add(&o->exprs,e);
		if(**pp==';'){
			(*pp)++;
			continue;
		}
		printf("<file> <line:col> expected ';'\n");
		exit(1);
	}
	if(o->is_encaps){
		if(**pp!='}'){
			printf("<file> <line:col> expected '}' to end block\n");
			exit(1);
		}
		(*pp)++;
	}
	return o;
}
