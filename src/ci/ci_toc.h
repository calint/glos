#pragma once
#include"../lib.h"

typedef struct ci_toc{

}ci_toc;

#define ci_toc_def {}

inline static void ci_toc_free(ci_toc*o){
	free(o);
}
