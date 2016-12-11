#pragma once
typedef struct phy{
	position p;
	velocity v;
	angle a;
	angular_velocity av;
}phy;
#define phy_def {vec4_def,vec4_def,vec4_def,vec4_def}

