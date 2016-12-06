#version 130
uniform mat4 umtx_mw; // model-to-world-matrix
attribute vec3 apos;  // vertices { x y z    }
void main(){
	gl_Position=umtx_mw*vec4(apos,1);
}
