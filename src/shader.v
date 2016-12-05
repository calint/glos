#version 130
uniform mat4 umtx_mw; // model-to-world-matrix
attribute vec3 apos;  // vertices { x y z    }
attribute vec4 argba; // colors   { r g b a  }
attribute vec3 anorm; // normals  { nx ny nz }
attribute vec2 atex;  // texture  { u v      }
varying vec4 vrgba;
varying vec3 vnorm;
varying vec2 vtex;
void main(){
	gl_Position=umtx_mw*vec4(apos,1);
	vrgba=argba;
	vnorm=anorm;
	vtex=atex;
}
