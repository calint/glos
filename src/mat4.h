#pragma once
#include"lib.h"
#include<math.h>

#define mat4_identity (float[]){1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1}


inline static void mat4_assign(float*this,float*src){
	memcpy(this,src,16*sizeof(float));
}

inline static void mat4_set_identity(float*this){
	mat4_assign(this,(float[]){1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1});
}


inline static void mat4_set_ortho_projection(float*c,
		const float left,const float right,
		const float bottom,const float top,
		const float nearz,const float farz
){
	c[ 0]=2.f/(right-left);
	c[ 1]=0;
	c[ 2]=0;
	c[ 3]=0;

	c[ 4]=0;
	c[ 5]=2.f/(top-bottom);
	c[ 6]=0;
	c[ 7]=0;

	c[ 8]=0;
	c[ 9]=0;
	c[10]=-2.f/(farz-nearz);
	c[11]=0;

	c[12]=-(right+left)/(right-left);
	c[13]=-(top+bottom)/(top-bottom);
	c[14]=-(farz+nearz)/(farz-nearz);
	c[14]=-(farz+nearz)/(farz-nearz);
	c[15]=1;
}


inline static void mat4_set_look_at(float*this,
		const position*eye,
		const position*lookat,
		const vec4*up){

	vec4 zaxis;
	vec4_minus(&zaxis,lookat,eye);
	vec4_normalize(&zaxis);// zaxis

	vec4 xaxis;
	vec4_cross(&xaxis,up,&zaxis); // xaxis
	vec4_normalize(&xaxis);

	vec4 yaxis;
	vec4_cross(&yaxis,&zaxis,&xaxis);

	// X
	this[0]=xaxis.x;
	this[1]=xaxis.y;
	this[2]=xaxis.z;
	this[3]=0;

	// Y
	this[4]=yaxis.x;
	this[5]=yaxis.y;
	this[6]=yaxis.z;
	this[7]=0;

//	this[4]=0;
//	this[5]=1;
//	this[6]=0;
//	this[7]=0;

	// Z
	this[8]=-zaxis.x;
	this[9]=-zaxis.y;
	this[10]=-zaxis.z;
	this[11]=0;

//	this[8]=0;
//	this[9]=0;
//	this[10]=1;
//	this[11]=0;

	// T
//	this[12]=0;
//	this[13]=0;
//	this[14]=0;
//	this[15]=1;
//
	this[12]=eye->x;
	this[13]=eye->y;
	this[14]=eye->z;
	this[15]=1;

//	mat4_set_identity(this);
}

inline static void mat4_set_perpective_projection(float*c,
		const float nearz,const float farz,
		const float fov_rad,const float aspect,
		bool left_handed
){

	const float frustrum_depth=farz-nearz;
	const float one_over_frustrum_depth=1/frustrum_depth;
	const float a=1/tanf(0.5f*fov_rad);

	// X
	c[0]=(left_handed?1:-1)*a/aspect;
	c[1]=0;
	c[2]=0;
	c[3]=0;

	// Y
	c[4]=0;
	c[5]=a;
	c[6]=0;
	c[7]=0;

	// Z
	c[8]=0;
	c[9]=0;
	c[10]=farz*one_over_frustrum_depth;
	c[11]=-farz*nearz*one_over_frustrum_depth;

	// T
	c[12]=0;
	c[13]=0;
	c[14]=1;
	c[15]=0;



//	const float aspect_ratio=1;
//	const float d=1.0f/tanf(fov_rad);
//
//	c[ 0]=d*aspect_ratio;
//	c[ 1]=0;
//	c[ 2]=0;
//	c[ 3]=0;
//
////		c[0]=1;c[1]=0;c[2]=0;c[3]=0;
//
//	c[ 4]=0;
//	c[ 5]=d;
//	c[ 6]=0;
//	c[ 7]=0;
//
////		c[4]=0;c[5]=1;c[6]=0;c[7]=0;
//
//
//	const float rangez=farz-nearz;
//	c[ 8]=0;
//	c[ 9]=0;
//	c[10]=-(nearz+farz)/rangez;
//	c[11]=-2.0f*farz*nearz/rangez;
//
////		c[ 8]=0;c[ 9]=0;c[10]=1;c[11]=0;
//
//	c[12]=0;
//	c[13]=0;
//	c[14]=1;
//	c[15]=0;
//
////	c[12]=0;c[13]=0;c[14]=0;c[15]=1;
}

















inline static float vec4_dot(const vec4*lh,const vec4*rh){
	return lh->x*rh->x+lh->y*rh->y+lh->z*rh->z;

}


inline static void mat4_set_translation(float*c,const position*p){
	// [ 0 4  8  x ]
	// [ 1 5  9  y ]
	// [ 2 6 10  z ]
	// [ 3 7 11 15 ]
	c[ 1]=c[ 2]=c[ 3]=c[ 4]=
	c[ 6]=c[ 7]=c[ 8]=c[ 9]=
	c[11]=0;
	c[ 0]=c[ 5]=c[10]=c[15]=1;
	c[12]=p->x;
	c[13]=p->y;
	c[14]=p->z;
}

inline static void mat4_append_rotation_about_z_axis(float*c,float degrees){
	// [ 0 4  8 12 ]   [ cos -sin 0  0 ]
	// [ 1 5  9 13 ] x [ sin cos  0  0 ]
	// [ 2 6 10 14 ]   [ 0   0    1  0 ]
	// [ 3 7 11 15 ]   [ 0   0    0  1 ]
	const float rad=degrees*(float)(PI/180.0f);
	const float cosrad = cosf(rad);
	const float sinrad = sinf(rad);

	const float mtx00=c[0];
	const float mtx01=c[1];
	const float mtx02=c[2];
	const float mtx03=c[3];

	c[ 0]=c[ 4]*sinrad+mtx00*cosrad;
	c[ 4]=c[ 4]*cosrad-mtx00*sinrad;

	c[ 1]=c[ 5]*sinrad+mtx01*cosrad;
	c[ 5]=c[ 5]*cosrad-mtx01*sinrad;

	c[ 2]=c[ 6]*sinrad+mtx02*cosrad;
	c[ 6]=c[ 6]*cosrad-mtx02*sinrad;

	c[ 3]=c[ 7]*sinrad+mtx03*cosrad;
	c[ 7]=c[ 7]*cosrad-mtx03*sinrad;
}

inline static void mat4_set_scale(float*c,scale*s){
	// [ 0 4  8 12 ]   [ x 0 0 0 ]
	// [ 1 5  9 13 ] x [ 0 y 0 0 ]
	// [ 2 6 10 14 ]   [ 0 0 z 0 ]
	// [ 3 7 11 15 ]   [ 0 0 0 1 ]

	memset(c,0,16);

	c[ 0]=s->x;
	c[ 4]=s->y;
	c[ 8]=s->z;

	c[ 1]=s->x;
	c[ 5]=s->y;
	c[ 9]=s->z;

	c[ 2]=s->x;
	c[ 6]=s->y;
	c[10]=s->z;

	c[ 3]=s->x;
	c[ 7]=s->y;
	c[11]=s->z;
}


inline static void mat4_scale(float*c,scale*s){
	// [ 0 4  8 12 ]   [ x 0 0 0 ]
	// [ 1 5  9 13 ] x [ 0 y 0 0 ]
	// [ 2 6 10 14 ]   [ 0 0 z 0 ]
	// [ 3 7 11 15 ]   [ 0 0 0 1 ]
	c[ 0]*=s->x;
	c[ 4]*=s->y;
	c[ 8]*=s->z;

	c[ 1]*=s->x;
	c[ 5]*=s->y;
	c[ 9]*=s->z;

	c[ 2]*=s->x;
	c[ 6]*=s->y;
	c[10]*=s->z;

	c[ 3]*=s->x;
	c[ 7]*=s->y;
	c[11]*=s->z;
}

inline static/*gives*/float*mat4_multiply(float*ret,const float*lhs,const float*rhs){
	// [ 0 4  8 12 ]   [ 0 4  8 12 ]
	// [ 1 5  9 13 ] x [ 1 5  9 13 ]
	// [ 2 6 10 14 ]   [ 2 6 10 14 ]
	// [ 3 7 11 15 ]   [ 3 7 11 15 ]

//	float*ret=malloc(sizeof(float)*16);
	ret[ 0] = lhs[ 0]*rhs[ 0] + lhs[ 4]*rhs[ 1] + lhs[ 8]*rhs[ 2] + lhs[12]*rhs[ 3];
	ret[ 1] = lhs[ 1]*rhs[ 0] + lhs[ 5]*rhs[ 1] + lhs[ 9]*rhs[ 2] + lhs[13]*rhs[ 3];
	ret[ 2] = lhs[ 2]*rhs[ 0] + lhs[ 6]*rhs[ 1] + lhs[10]*rhs[ 2] + lhs[14]*rhs[ 3];
	ret[ 3] = lhs[ 3]*rhs[ 0] + lhs[ 7]*rhs[ 1] + lhs[11]*rhs[ 2] + lhs[15]*rhs[ 3];

	ret[ 4] = lhs[ 0]*rhs[ 4] + lhs[ 4]*rhs[ 5] + lhs[ 8]*rhs[ 6] + lhs[12]*rhs[ 7];
	ret[ 5] = lhs[ 1]*rhs[ 4] + lhs[ 5]*rhs[ 5] + lhs[ 9]*rhs[ 6] + lhs[13]*rhs[ 7];
	ret[ 6] = lhs[ 2]*rhs[ 4] + lhs[ 6]*rhs[ 5] + lhs[10]*rhs[ 6] + lhs[14]*rhs[ 7];
	ret[ 7] = lhs[ 3]*rhs[ 4] + lhs[ 7]*rhs[ 5] + lhs[11]*rhs[ 6] + lhs[15]*rhs[ 7];

	ret[ 8] = lhs[ 0]*rhs[ 8] + lhs[ 4]*rhs[ 9] + lhs[ 8]*rhs[10] + lhs[12]*rhs[11];
	ret[ 9] = lhs[ 1]*rhs[ 8] + lhs[ 5]*rhs[ 9] + lhs[ 9]*rhs[10] + lhs[13]*rhs[11];
	ret[10] = lhs[ 2]*rhs[ 8] + lhs[ 6]*rhs[ 9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
	ret[11] = lhs[ 3]*rhs[ 8] + lhs[ 7]*rhs[ 9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];

	ret[12] = lhs[ 0]*rhs[12] + lhs[ 4]*rhs[13] + lhs[ 8]*rhs[14] + lhs[12]*rhs[15];
	ret[13] = lhs[ 1]*rhs[12] + lhs[ 5]*rhs[13] + lhs[ 9]*rhs[14] + lhs[13]*rhs[15];
	ret[14] = lhs[ 2]*rhs[12] + lhs[ 6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
	ret[15] = lhs[ 3]*rhs[12] + lhs[ 7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];
	return ret;
}
/*
		m4&set_translation(const p3&p){
			// [ 0 4  8  x ]
			// [ 1 5  9  y ]
			// [ 2 6 10  z ]
			// [ 3 7 11 15 ]
			c[12]=p.x;
			c[13]=p.y;
			c[14]=p.z;
			return*this;
		}
		m4&load_identity(){
			// [ 0 4  8 12 ]
			// [ 1 5  9 13 ]
			// [ 2 6 10 14 ]
			// [ 3 7 11 15 ]
			c[ 1]=c[ 2]=c[ 3]=c[ 4]=
			c[ 6]=c[ 7]=c[ 8]=c[ 9]=
			c[11]=0;
			c[ 0]=c[ 5]=c[10]=c[15]=1;
			return*this;
		}
		m4&append_scaling(const p3&scale){
			// [ 0 4  8 12 ]   [ x 0 0 0 ]
			// [ 1 5  9 13 ] x [ 0 y 0 0 ]
			// [ 2 6 10 14 ]   [ 0 0 z 0 ]
			// [ 3 7 11 15 ]   [ 0 0 0 1 ]
			c[ 0]*=scale.x;
			c[ 4]*=scale.y;
			c[ 8]*=scale.z;

			c[ 1]*=scale.x;
			c[ 5]*=scale.y;
			c[ 9]*=scale.z;

			c[ 2]*=scale.x;
			c[ 6]*=scale.y;
			c[10]*=scale.z;

			c[ 3]*=scale.x;
			c[ 7]*=scale.y;
			c[11]*=scale.z;
			return*this;
		}
		m4&load_ortho_projection(const floato left,const floato right,const floato bottom,const floato top,const floato nearz,const floato farz){
			c[ 0]=2.f/(right-left);
			c[ 1]=0;
			c[ 2]=0;
			c[ 3]=0;

			c[ 4]=0;
			c[ 5]=2.f/(top-bottom);
			c[ 6]=0;
			c[ 7]=0;

			c[ 8]=0;
			c[ 9]=0;
			c[10]=-2.f/(farz-nearz);
			c[11]=0;

			c[12]=-(right+left)/(right-left);
			c[13]=-(top+bottom)/(top-bottom);
			c[14]=-(farz+nearz)/(farz-nearz);
			c[15]=1;
			return*this;
		}
	//	inline p3 x_axis()const{return p3{c[0],c[4],c[8]};}

		//	inline p3 y_axis()const{return p3{c[1],c[5],c[9]};}
		inline p3 y_axis()const{return p3{c[4],c[5],c[6]};}

	//	inline p3 z_axis()const{return p3{c[2],c[6],c[10]};}
	//	inline p3 w_axis()const{return p3{c[3],c[7],c[11]};}
	};
	m4 operator*(const m4&lh,const m4&rh){
		// [ 0 4  8 12 ]   [ 0 4  8 12 ]
		// [ 1 5  9 13 ] x [ 1 5  9 13 ]
		// [ 2 6 10 14 ]   [ 2 6 10 14 ]
		// [ 3 7 11 15 ]   [ 3 7 11 15 ]
		m4 m;
		floato*ret=m.c;
		const floato*lhs=lh.c;
		const floato*rhs=rh.c;
		ret[ 0] = lhs[ 0]*rhs[ 0] + lhs[ 4]*rhs[ 1] + lhs[ 8]*rhs[ 2] + lhs[12]*rhs[ 3];
		ret[ 1] = lhs[ 1]*rhs[ 0] + lhs[ 5]*rhs[ 1] + lhs[ 9]*rhs[ 2] + lhs[13]*rhs[ 3];
		ret[ 2] = lhs[ 2]*rhs[ 0] + lhs[ 6]*rhs[ 1] + lhs[10]*rhs[ 2] + lhs[14]*rhs[ 3];
		ret[ 3] = lhs[ 3]*rhs[ 0] + lhs[ 7]*rhs[ 1] + lhs[11]*rhs[ 2] + lhs[15]*rhs[ 3];

		ret[ 4] = lhs[ 0]*rhs[ 4] + lhs[ 4]*rhs[ 5] + lhs[ 8]*rhs[ 6] + lhs[12]*rhs[ 7];
		ret[ 5] = lhs[ 1]*rhs[ 4] + lhs[ 5]*rhs[ 5] + lhs[ 9]*rhs[ 6] + lhs[13]*rhs[ 7];
		ret[ 6] = lhs[ 2]*rhs[ 4] + lhs[ 6]*rhs[ 5] + lhs[10]*rhs[ 6] + lhs[14]*rhs[ 7];
		ret[ 7] = lhs[ 3]*rhs[ 4] + lhs[ 7]*rhs[ 5] + lhs[11]*rhs[ 6] + lhs[15]*rhs[ 7];

		ret[ 8] = lhs[ 0]*rhs[ 8] + lhs[ 4]*rhs[ 9] + lhs[ 8]*rhs[10] + lhs[12]*rhs[11];
		ret[ 9] = lhs[ 1]*rhs[ 8] + lhs[ 5]*rhs[ 9] + lhs[ 9]*rhs[10] + lhs[13]*rhs[11];
		ret[10] = lhs[ 2]*rhs[ 8] + lhs[ 6]*rhs[ 9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
		ret[11] = lhs[ 3]*rhs[ 8] + lhs[ 7]*rhs[ 9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];

		ret[12] = lhs[ 0]*rhs[12] + lhs[ 4]*rhs[13] + lhs[ 8]*rhs[14] + lhs[12]*rhs[15];
		ret[13] = lhs[ 1]*rhs[12] + lhs[ 5]*rhs[13] + lhs[ 9]*rhs[14] + lhs[13]*rhs[15];
		ret[14] = lhs[ 2]*rhs[12] + lhs[ 6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
		ret[15] = lhs[ 3]*rhs[12] + lhs[ 7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];
		return m;//? std::move
	}
	// - - - -------- - - - - - - -    - - --- - - - - - --- -- - - - - -
	// forked from apple examples
}
*/
