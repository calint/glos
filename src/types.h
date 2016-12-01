#pragma once
//------------------------------------------------------------------ vec4
typedef struct vec3 {
	float x, y, z, q;
} vec4;

//------------------------------------------------------------------ typedefs
typedef vec4 position;
typedef vec4 angle;
typedef vec4 scale;
typedef float bounding_radius;
typedef vec4 velocity;
typedef vec4 angular_velocity;
typedef Uint32 color;
typedef int id;
typedef Uint8 bits;
typedef int8_t byte;
typedef int16_t word;
typedef int32_t dword;
typedef int64_t quad;

//---------------------------------------------------------------------- lib
inline static int bits_is_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	return (*b & bb) != 0;
}

inline static void bits_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b |= bb;
}

inline static void bits_unset(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b &= (bits) ~bb;
}
//---------------------------------------------------------------------
