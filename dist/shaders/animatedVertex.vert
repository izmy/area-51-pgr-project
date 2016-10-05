#version 140

in vec3 position;              // vertex position in world space
in vec2 texCoord;              // incoming texture coordinates

uniform mat4 PVMmatrix;        // Projection * View * Model --> model to clip coordinates
uniform float time;

smooth out vec2 texCoord_v;    // outgoing vertex texture coordinates

void main() {	
	vec2 offset = vec2(time, 0.0f);
	texCoord_v = texCoord - offset;
	gl_Position = PVMmatrix * vec4(position, 1.0f); 
}
