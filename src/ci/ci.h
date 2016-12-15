#pragma once
#include"../lib.h"
#include"ci_class.h"

dynp/*owns*/ci_classes=dynp_def;

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

