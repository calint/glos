#version 130
uniform mat4 umtx_mw;
uniform mat4 umtx_wvp;
in vec3 apos;
in vec4 argba;
in vec3 anorm;
in vec2 atex;
out vec4 vrgba;
out vec3 vnorm;
out vec2 vtex;
void main(){
	gl_Position=umtx_wvp*umtx_mw*vec4(apos,1);
	vrgba=argba;
	vnorm=anorm;
	vtex=atex;
}