#pragma once
#include"lib/consts.h"
#include"lib/types.h"
#include"lib/str.h"
#include"lib/dynf.h"
#include"lib/dyni.h"
#include"lib/dynp.h"
#include"lib/vec3.h"
#include"lib/mat4.h"
#include"lib/token.h"
#include"lib/etc.h"

#define foo(return_type,body_and_args)({return_type __fn__ body_and_args __fn__;})

//		while(n){
//			printf(n&1?"1":"0");
//			n>>=1;
//		}
//		printf(" %f %f %f\n",po->p.p.x,po->p.p.y,po->p.p.z);
