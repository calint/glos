#pragma once

struct cell;
typedef struct grid_ifc{
	struct cell*main_cell;
	bits bits; // 1: overlaps
}grid_ifc;

#define grid_ifc_def {NULL,0}
