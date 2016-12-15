#pragma once
#include"../lib.h"
#include"ci_toc.h"
#include"ci_expression.h"

typedef struct ci_statement{
	void (*compile)(struct ci_statement*,ci_toc*);
}ci_statement;

#define ci_statement_def (ci_statement){NULL}

inline static void ci_statement_free(ci_statement*o){
	free(o);
}

inline static int ci_statment_is_empty(ci_statement*o){
	return 1;
}

