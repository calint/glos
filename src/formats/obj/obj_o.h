#pragma once
#include<stdio.h>
#include "../../formats/obj/obj_v.h"

typedef struct obj_o{
	char*name;
	struct obj_v*vertex_array;
	struct obj_vn*normal_array;
	struct obj_vf*face_array;
}obj_o;
