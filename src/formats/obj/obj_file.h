#pragma once
#include<stdlib.h>
#include<string.h>
#include"../../formats/obj/obj_o.h"

typedef struct obj_file{
	const char*name;
	struct obj_o*o_array;
}obj_file;

static void load_obj_file(obj_file*this,const char*path){
	printf(" * load obj file: %s\n",path);
	FILE*ptr_file =fopen(path,"r");
	if (!ptr_file){
		perror("could not open file");
		perror(path);
		exit(32);
	}

	int v_count=0;
	int vn_count=0;
	int vf_count=0;

	char buf[1024];
	while(1){
		int res=fscanf(ptr_file,"%s",buf);
		if(res==EOF){
			printf("end-of-file");
			break;
		}
		if(!strncmp(buf,"#",sizeof(buf))){// comment
			continue;
		}
		printf("%s\n",buf);
//		exit(0);
//
//		const char*delim=" ";
//		char*token=strtok(line_buf,delim);
//		while(token){
//			if(!strncmp(token,"o")){
//				printf(" * found object\n");
//
//			}
//			printf( " %s\n",token);
//			token=strtok(NULL,delim);
//		}
//		printf("%s",line_buf);
	}

	fclose(ptr_file);
}
