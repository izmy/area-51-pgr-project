#version 140

uniform mat4 PVM;
in vec3 position;
out vec3 texCoord_v;

void main() {
	gl_Position = PVM * vec4(position, 1.0);
    texCoord_v = position;
 }