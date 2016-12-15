#pragma once
#include"../lib.h"
#include "class.h"
#include "expr.h"
#include "expr_call.h"
#include "expr_ident.h"

dynp/*owns*/ci_classes=dynp_def;

inline static void ci_init(){}

static void ci_free(){
	dynp_foa(&ci_classes,{
		ci_class_free((ci_class*)o);
	});
	dynp_free(&ci_classes);
}

inline static /*gives*/ci_expr*ci_expr_next(
		ci_toc*toc,const char**pp){
	token t=token_next(pp);
	if(token_is_empty(&t)){
		ci_expr*e=malloc(sizeof(ci_expr));
		*e=ci_expr_def;
		return e;
	}

	if(**pp=='('){// function call
		*pp=t.begin;
		ci_expr_call*e=ci_expr_call_next(toc,pp);
		return (ci_expr*)e;
	}
	// assuming identifier
	ci_expr_ident*e=malloc(sizeof(ci_expr_ident));
	*e=ci_expr_ident_def;
	token_setz(&t,&e->name);
	return (ci_expr*)e;
}

static void ci_parse_function(const char**pp,ci_toc*tc,ci_class*c,
		token*type,token*name){
	ci_func*f=malloc(sizeof(ci_func));
	*f=ci_func_def;
	dynp_add(&c->functions,f);
	token_setz(type,&f->type);
	token_setz(name,&f->name);
	while(1){
		token argtype=token_next(pp);
		if(token_is_empty(&argtype)){
			if(**pp==')'){
				(*pp)++;
				break;
			}
			printf("<file> <line:col> expected function arguments or ')'\n");
			exit(1);
		}
		token argname=token_next(pp);
		ci_func_arg*fa=malloc(sizeof(ci_func_arg));
		*fa=ci_func_arg_def;
		dynp_add(&f->args,fa);
		token_setz(&argtype,&fa->type);
		token_setz(&argname,&fa->name);
		if(**pp==','){
			(*pp)++;
			continue;
		}
	}
	if(**pp=='{'){//? require
		(*pp)++;
	}
	while(1){
		ci_expr*e=ci_expr_next(tc,pp);
		if(ci_expr_is_empty(e)){
			if(**pp=='}'){
				(*pp)++;
				break;
			}
		}
		dynp_add(&f->exprs,e);
		if(**pp==';'){
			(*pp)++;
			continue;
		}
		printf("<file> <line:col> expected ';'\n");
		exit(1);
	}
}

static void ci_parse_field(const char**pp,ci_toc*tc,ci_class*c,
		token*type,token*name){
	ci_field*f=malloc(sizeof(ci_field));
	*f=ci_field_def;
	token_setz(type,&f->type);
	if(token_is_empty(name)){
		token_setz(type,&f->name);
	}else{
		token_setz(name,&f->name);
	}
	dynp_add(&c->fields,f);
	if(**pp=='='){
		(*pp)++;
		ci_expr*e=ci_expr_next(tc,pp);
		f->initval=e;
	}
	if(**pp!=';'){
		printf("<file> <line:col> expected ';' to finish field declaration\n");
		exit(1);
	}
	(*pp)++;
	ci_toc_add_identifier(tc,f->name.data);
}

static void ci_compile_to_c(ci_toc*tc){
//	for(unsigned i=0;i<ci_classes.count;i++){
//		ci_class*c=dynp_get(&ci_classes,i);
//		printf("\n");
//		printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
//		printf("static typedef struct %s{\n",c->name.data);
//		for(unsigned j=0;j<c->extends.count;j++){
//			str*e=dynp_get(&c->extends,j);
//			printf("    %s %s;\n",e->data,e->data);
//		}
//		for(unsigned j=0;j<c->fields.count;j++){
//			ci_field*f=dynp_get(&c->fields,j);
//			printf("    %s %s;\n",f->type.data,f->name.data);
//		}
//		printf("}%s;\n",c->name.data);
//
//		printf("\n");
//		// #define object_def {...}
//		printf("#define %s_def {",c->name.data);
//		for(unsigned j=0;j<c->extends.count;j++){
//			str*e=dynp_get(&c->extends,j);
//			printf("%s_def,",e->data);
//		}
//		for(unsigned j=0;j<c->fields.count;j++){
//			ci_field*f=dynp_get(&c->fields,j);
//			printf("    %s %s;\n",f->type.data,f->name.data);
//		}
//		printf("}\n");
//	}
//	return;

	dynp_foa(&ci_classes,{
			ci_class*c=o;
			printf("\n");
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf("static typedef struct %s{\n",c->name.data);
			dynp_foa(&c->extends,{
				str*s=o;
				printf("    %s %s;\n",s->data,s->data);
			});
			dynp_foa(&c->fields,{
				ci_field*s=o;
				printf("    %s %s;\n",s->type.data,s->name.data);
			});
			printf("}%s;\n",c->name.data);

			printf("\n");

			// #define object_def {...}
			printf("#define %s_def {",c->name.data);
			dynp_foa(&c->extends,{
				str*s=o;
				printf("%s_def,",s->data);
			});
			dynp_foa(&c->fields,{
				ci_field*s=o;
				if(s->initval){
					s->initval->compile(s->initval,tc);
				}else{
					printf("%s_def",s->type.data);
				}
				printf(",");
			});
			printf("}\n");

			// init
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf(	"inline static void %s_init(%s*o){\n",
					c->name.data,c->name.data);
			dynp_foa(&c->extends,{
				str*s=o;
				printf(	"	%s_init(&o->%s);\n",s->data,s->data);
			});
			printf("}\n");

			// alloc
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf(	"inline static %s*%s_alloc(){\n"
					"	%s*p=malloc(sizeof(%s));\n"
					"	*p=%s_def;\n"
					"	%s_init(p);\n"
					"	return p;\n"
					"}\n",
					c->name.data,c->name.data,
					c->name.data,c->name.data,
					c->name.data,
					c->name.data
			);
			// free
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf(	"inline static void %s_free(%s*o){\n"
					,c->name.data,c->name.data);

			dynp_foar(&c->extends,{
				str*s=o;
				printf(	"	%s_free(&o->%s);\n",s->data,s->data);
			});
			printf("	free(o);\n");
			printf("}\n");

			// casts
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			dynp_foa(&c->extends,{
				str*s=o;
				printf(	"inline static %s*%s_as_%s(%s*o){"
						"return(%s*)&o->%s;"
						"}\n",s->data,c->name.data,s->data,c->name.data,
						s->data,s->data
				);
			});

			// functions
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			dynp_foa(&c->functions,{
				ci_func*f=o;
				printf("inline static %s %s_%s(",f->type.data,c->name.data,f->name.data);
				printf("%s*o",c->name.data);
				dynp_foa(&f->args,{
					ci_func_arg*a=o;
					printf(",");
					printf("%s %s",a->type.data,a->name.data);
				});
				printf("){\n");
				dynp_foa(&f->exprs,{
					ci_expr*e=o;
					e->compile(e,tc);
					printf(";\n");
				});
				printf("}\n");
			});
	});
	printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
	printf("\n");
}
static void ci_compile(const char*path){
	str s=str_from_file(path);
	const char*p=s.data;
	ci_toc toc=ci_toc_def;
	while(1){
		token t=token_next(&p);
		if(token_is_empty(&t))
			break;
		if(!token_equals(&t,"class")){
			printf("expected class declaration\n");
			exit(1);
		}
		token nm=token_next(&p);
		ci_class*c=malloc(sizeof(ci_class));
		*c=ci_class_def;
		dynp_add(&ci_classes,c);
		token_setz(&nm,&c->name);
		ci_toc_push_scope(&toc, 'c',c->name.data);
		if(*p==':'){
			p++;
			while(1){
				token extends_name=token_next(&p);
				dynp_add(&c->extends,/*takes*/token_to_str(&extends_name));
				if(*p=='{'){
					break;
				}else if(*p==','){
					p++;
					continue;
				}
				printf("<file> <line:col> expected '{' or ',' followed by "
						"class name");
				exit(1);

			}
		}
		if(*p!='{'){
			printf("<file> <line:col> expected '{' to open class body");
			exit(1);
		}
		p++;
		while(1){
			token type=token_next(&p);
			if(token_is_empty(&type)){
				if(*p!='}'){
					printf("<file> <line:col> expected '}' to close class body\n");
					exit(1);
				}
				p++;
				break;
			}
			token name=token_next(&p);
			if(*p=='('){
				p++;
				ci_parse_function(&p,&toc,c,&type,&name);
			}else if(*p=='=' || *p==';'){
				ci_parse_field(&p,&toc,c,&type,&name);
			}else{
				printf("<file> <line:col> expected ';'\n");
				exit(1);
			}
		}
		ci_toc_pop_scope(&toc);//, 'c',c->name.data);
	}
	ci_compile_to_c(&toc);
}

