#pragma once
#include"../lib.h"
#include "toc.h"

typedef struct ci_expression{
	void (*compile)(struct ci_expression*,struct ci_toc*);
	void (*free)(struct ci_expression*);
}ci_expression;

inline static void ci_expression_free(ci_expression*o){
//	free(o);
}

#define ci_expression_def (ci_expression){NULL,ci_expression_free}

inline static int ci_expression_is_empty(ci_expression*o){
	return o->compile==NULL;
}

