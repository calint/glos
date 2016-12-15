#pragma once
#include"../lib.h"

typedef struct ci_slot{
	str type;
	str name;
}ci_slot;

#define ci_slot_def (ci_slot){str_def,str_def}

typedef struct ci_func_arg{
	str type;
	str name;
}ci_func_arg;

#define ci_func_arg_def (ci_func_arg){str_def,str_def}

typedef struct ci_func{
	str type;
	str name;
	dynp/*owns ci_func_arg*/args;
}ci_func;

#define ci_func_def (ci_func){str_def,str_def,dynp_def}

typedef struct ci_class{
	str name;
	dynp/*owns str*/extends;
	dynp/*owns ci_slot*/slots;
	dynp/*owns ci_func*/functions;
}ci_class;

#define ci_class_def (ci_class){str_def,dynp_def,dynp_def}

dynp/*owns*/ci_classes=dynp_def;

inline static void ci_init(){}

inline static void ci_class_free(ci_class*o){
	for(unsigned i=0;i<o->extends.count;i++){
		str_free((str*)&o->extends.data[i]);
	}
	dynp_free(&o->extends);
	str_free(&o->name);
}

static void ci_free(){
	for(unsigned i=0;i<ci_classes.count;i++){
		ci_class_free(ci_classes.data[i]);
	}
	dynp_free(&ci_classes);
}

static void ci_load_def(const char*path){
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
				ci_slot*slt=malloc(sizeof(ci_slot));
				*slt=ci_slot_def;
				dynp_add(&c->slots,slt);
				token name=token_next(&p);
				token_copy_to_str(&type,&slt->type);
				token_copy_to_str(&name,&slt->name);
				if(token_is_empty(&name)){
					token_copy_to_str(&type,&slt->name);
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
					if(*p=='{'){
						p++;
					}
					token body=token_next(&p);
					if(*p=='}'){
						p++;
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
			dynp_foa(&c->slots,{
				ci_slot*s=o;
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
			dynp_foa(&c->slots,{
				ci_slot*s=o;
				printf("%s_def,",s->type.data);
			});
			printf("}\n");
			// alloc
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf(	"inline static %s*%s_alloc(){\n"
					"	%s*p=malloc(sizeof(%s));\n"
					"	*p=%s_def;\n"
					"	return p;\n"
					"}\n",
					c->name.data,c->name.data,
					c->name.data,c->name.data,
					c->name.data
			);

			// init
			printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
			printf(	"inline static void %s_init(%s*o){\n",
					c->name.data,c->name.data);
			dynp_foa(&c->extends,{
				str*s=o;
				printf(	"	%s_init(&o->%s);\n",s->data,s->data);
			});
			printf("}\n");

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
				printf("    %s %s(",f->type.data,f->name.data);
				dynp_foac(&f->args,{
					ci_func_arg*a=o;
					printf("%s %s",a->type.data,a->name.data);
					if(i+1<f->args.count){
						printf(",");
					}
				});
				printf(")\n");
			});
	});
	printf("//--- - - ---------------------  - -- - - - - - - -- - - - -- - - - -- - - -\n");
	printf("\n");
}

