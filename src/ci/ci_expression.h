#pragma once
#include"../lib.h"
#include"ci_toc.h"

typedef struct ci_expression{
	void (*compile)(struct ci_expression*,struct ci_toc*);
}ci_expression;

#define ci_expression_def (ci_expression){NULL}

inline static void ci_expression_free(ci_expression*o){
	free(o);
}


inline static int ci_expression_is_empty(ci_expression*o){
	return o->compile==NULL;
}

