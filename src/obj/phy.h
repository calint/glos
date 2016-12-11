#pragma once

#pragma once

typedef struct _phy{
	position position;
	velocity velocity;
	angle angle;
	angular_velocity angular_velocity;
}phy;
#define phy_def {vec4_def,vec4_def,vec4_def,vec4_def}

