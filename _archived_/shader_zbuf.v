#version 130
uniform mat4 umtx_mw;
in vec3 apos;
void main(){
	gl_Position=umtx_mw*vec4(apos,1);
}

