#version 140

uniform sampler2D texSampler;  // sampler for the texture access
uniform float time;            // time used for simulation of moving lights (such as sun)
uniform mat4 PVMmatrix;        // Projection * View * Model  --> model to clip coordinates

out vec4  color_f;             // outgoing fragment color
smooth in vec2 texCoord_v;     // fragment texture coordinates

void main() {	
	color_f = texture(texSampler, texCoord_v);	
}