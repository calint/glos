#pragma once

#define PI 3.1415926535897932384626433832795f

#define foo(return_type,body_and_args)({return_type __fn__ body_and_args __fn__;})
#define fow(return_type,body_and_args)({return_type __fn__ body_and_args __fn__;})
