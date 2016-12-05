#version 130
uniform sampler2D utex;
varying mediump vec4 vrgba;
varying mediump vec3 vnorm;
varying mediump vec2 vtex;
void main(){
	gl_FragColor=texture2D(utex,vtex)+vrgba*dot(vec4(1,0,0,1),vec4(vnorm,1));
}