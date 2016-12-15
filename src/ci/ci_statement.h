#pragma once
#include"../lib.h"

typedef struct ci_statement{
	struct ci_statement*parent;
}ci_statement;

#define ci_statement_def (ci_statement){NULL}

inline static void ci_statement_free(ci_statement*o){
	free(o);
}

inline static /*gives*/ci_statement*ci_statement_next(ci_statement*parent,
		const char**p){
	token t=token_next(p);
	ci_statement*s=malloc(sizeof(ci_statement));
	*s=ci_statement_def;
	s->parent=parent;
	return s;
}

inline static int ci_statment_is_empty(ci_statement*o){
	return 1;
}

