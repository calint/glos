#pragma once
#include"../lib.h"
#include "expr.h"
#include "toc.h"
#include"block.h"
typedef struct ci_expr_bool{
	ci_expr super;

	// element
	bool lh_negate;

	struct ci_expr*lh;

	char op;

	bool rh_negate;

	struct ci_expr*rh;


	// list
	str bool_op_list;

	dynp/*owns &expr_bool*/bool_list;

	bool is_encapsulated;

}ci_expr_bool;

inline static void _ci_expr_bool_compile_(const ci_expr*oo,ci_toc*tc){
	ci_expr_bool*o=(ci_expr_bool*)oo;

	if(o->lh_negate)
		printf("!");

	o->lh->compile(o->lh,tc);

	if(!o->rh)
		return;

	if(o->op=='='){
		printf("==");
	}else if(o->op=='!'){
		printf("!=");
	}else{
		printf("<file> <line:col> unknown op '%d' ')'\n",o->op);
		exit(1);
	}

	if(o->rh_negate)
		printf("!");

	o->rh->compile(o->rh,tc);
}

inline static void _ci_expr_bool_free_(ci_expr*oo){
	ci_expr_free(oo);
}

#define ci_expr_bool_def (ci_expr_bool){\
	{str_def,_ci_expr_bool_compile_,_ci_expr_bool_free_},\
	false,NULL,0,NULL,false,\
	str_def,\
	dynp_def,\
	false}

inline static void ci_expr_bool_parse(ci_expr_bool*o,
		const char**pp,ci_toc*tc){

	o->super.type=str_from_string("bool");
	if(**pp!='('){//   keybits==1 && ok || (a&b!=0)
		o->is_encapsulated=false;

		if(**pp=='!'){// if(!ok){}
			(*pp)++;
			o->lh_negate=true;
		}

		o->lh=ci_expr_new_from_pp(pp,tc);

		if(**pp=='='){
			(*pp)++;
			if(**pp!='='){
				printf("<file> <line:col> expected '=='\n");
				exit(1);
			}
			o->op='=';
			(*pp)++;
		}else if(**pp=='!'){
			(*pp)++;
			if(**pp!='='){
				printf("<file> <line:col> expected '!='\n");
				exit(1);
			}
			o->op='!';
			(*pp)++;
		}else{// if(active)
//			o->op='.';
			return;
		}

		if(**pp=='!'){// if(!ok){}
			(*pp)++;
			o->rh_negate=true;
		}

		o->rh=ci_expr_new_from_pp(pp,tc);
		//? keybits==1 && ok
		if(**pp=='&'){
			(*pp)++;
			if(**pp=='&'){
				(*pp)++;
				printf("<file> <line:col> and list not supported\n");
				exit(1);
			}
		}else if(**pp=='|'){
			(*pp)++;
			if(**pp=='|'){
				(*pp)++;
				printf("<file> <line:col> and list not supported\n");
				exit(1);
			}
		}else{
			return;
//			printf("<file> <line:col> unknown boolean operator. expected && or ||\n");
//			exit(1);
		}
	}
	o->is_encapsulated=true;
	(*pp)++;
	ci_expr_bool*e=malloc(sizeof(ci_expr_bool));
	*e=ci_expr_bool_def;
	ci_expr_bool_parse(o,pp,tc);
}
