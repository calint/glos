#version 130
uniform sampler2D utex;
in vec4 vrgba;
in vec3 vnorm;
in vec2 vtex;
out vec4 rgba;
void main(){
	rgba=texture2D(utex,vtex)+vrgba*dot(vec4(1,0,0,1),vec4(vnorm,1));
}
