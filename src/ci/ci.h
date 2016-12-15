#pragma once
#include"../lib.h"

typedef struct ci_toc{

}ci_toc;

#define ci_toc_def {}

inline static void ci_toc_free(ci_toc*o){
	free(o);
}

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

typedef struct ci_member{
	str type;
	str name;
}ci_member;

#define ci_member_def (ci_member){str_def,str_def}

inline static void ci_member_free(ci_member*o){
	str_free(&o->type);
	str_free(&o->name);
	free(o);
}

typedef struct ci_func_arg{
	str type;
	str name;
}ci_func_arg;

#define ci_func_arg_def (ci_func_arg){str_def,str_def}

inline static void ci_func_arg_free(ci_func_arg*o){
	str_free(&o->name);
	str_free(&o->type);
	free(o);
}

typedef struct ci_func{
	str type;
	str name;
	dynp/*owns ci_func_arg**/args;
	dynp/*owns ci_statment**/stmts;
}ci_func;

#define ci_func_def (ci_func){str_def,str_def,dynp_def,dynp_def}

inline static void ci_func_free(ci_func*o){
	dynp_foa(&o->stmts,{
		ci_statement_free((ci_statement*)o);
	});
	dynp_free(&o->stmts);

	dynp_foa(&o->args,{
		ci_func_arg_free((ci_func_arg*)o);
	});
	dynp_free(&o->args);

	str_free(&o->name);
	str_free(&o->type);
	free(o);
}

typedef struct ci_class{
	str name;
	dynp/*owns str*/extends;
	dynp/*owns ci_slot*/members;
	dynp/*owns ci_func*/functions;
}ci_class;

#define ci_class_def (ci_class){str_def,dynp_def,dynp_def,dynp_def}
dynp/*owns*/ci_classes=dynp_def;

inline static void ci_class_free(ci_class*o){

	dynp_foa(&o->functions,{
		ci_func_free((ci_func*)o);
	});
	dynp_free(&o->functions);

	dynp_foa(&o->members,{
		ci_member_free((ci_member*)o);
	});
	dynp_free(&o->members);

	dynp_foa(&o->extends,{
		str_free((str*)o);
	});
	dynp_free(&o->extends);

	str_free(&o->name);

	free(o);
}


inline static void ci_init(){}

static void ci_free(){
	dynp_foa(&ci_classes,{
		ci_class_free((ci_class*)o);
	});
	dynp_free(&ci_classes);
}

static void ci_compile(const char*path){
	str s=str_from_file(path);
	const char*p=s.data;
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
		token_copy_to_str(&nm,&c->name);
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
		// class body starts
		if(*p=='{'){ // compound
			p++;
			while(1){
				token type=token_next(&p);
				if(token_is_empty(&type)){
					if(*p!='}'){
						printf("<file> <line:col> expected end of class\n");
						exit(1);
					}
					p++;
					break;
				}
				ci_member*m=malloc(sizeof(ci_member));
				*m=ci_member_def;
				dynp_add(&c->members,m);
				token_copy_to_str(&type,&m->type);
				token name=token_next(&p);
				if(token_is_empty(&name)){
					token_copy_to_str(&type,&m->name);
				}else{
					token_copy_to_str(&name,&m->name);
				}
				if(*p=='('){
					p++;
					ci_func*f=malloc(sizeof(ci_func));
					*f=ci_func_def;
					dynp_add(&c->functions,f);
					token_copy_to_str(&type,&f->type);
					token_copy_to_str(&name,&f->name);
					while(1){
						token argtype=token_next(&p);
						if(token_is_empty(&argtype)){
							if(*p==')'){
								p++;
								break;
							}
							printf("<file> <line:col> expected function arguments or ')'\n");
							exit(1);
						}
						token argname=token_next(&p);
						ci_func_arg*fa=malloc(sizeof(ci_func_arg));
						*fa=ci_func_arg_def;
						dynp_add(&f->args,fa);
						token_copy_to_str(&argtype,&fa->type);
						token_copy_to_str(&argname,&fa->name);
						if(*p==','){
							p++;
							continue;
						}
					}
					if(*p=='{'){//? require
						p++;
					}
					while(1){
						ci_statement*stmt=ci_statement_next(NULL,&p);
						if(ci_statment_is_empty(stmt)){
							if(*p=='}'){
								p++;
								break;
							}
						}
						dynp_add(&f->stmts,stmt);
					}
				}else if(*p=='='){
					p++;
					printf("<file> <line:col> member initializer not supported\n");
					exit(1);
				}else if(*p==';'){
					p++;
					continue;
				}else{
					printf("<file> <line:col> expected ';'\n");
					exit(1);
				}
			}
		}
	}
	// print
	printf("// compiled from: %s\n",path);
	dynp_foa(&ci_classes,{
			ci_class*c=o;
			printf("\n");
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf("static typedef struct %s{\n",c->name.data);
			dynp_foa(&c->extends,{
				str*s=o;
				printf("    %s %s;\n",s->data,s->data);
			});
			dynp_foa(&c->members,{
				ci_member*s=o;
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
			dynp_foa(&c->members,{
				ci_member*s=o;
				printf("%s_def,",s->type.data);
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
				dynp_foac(&f->args,{
					ci_func_arg*a=o;
					printf(",");
					printf("%s %s",a->type.data,a->name.data);
				});
				printf("){\n");
				printf("}\n");
			});
	});
	printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
	printf("\n");
}

