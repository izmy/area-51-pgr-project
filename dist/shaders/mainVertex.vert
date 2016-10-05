#version 140

in vec3 position;           
in vec3 normal;            
in vec2 texCoord;           

smooth out vec2 texCoord_v;  
smooth out vec3 normal_v;      //normal in eye coord
smooth out vec3 position_v;    //vertex in eye coord

uniform mat4 normalMatrix;     // inverse transposed VMmatrix
uniform mat4 PVMmatrix;   
uniform mat4 Vmatrix;          // View --> world to eye coordinates
uniform mat4 Mmatrix;          // Model --> model to world coordinates


void main() {
	normal_v = normalize(normalMatrix * vec4(normal, 0.0f)).xyz;   // normal in eye coordinates by NormalMatrix
	position_v = (Vmatrix * Mmatrix * vec4(position, 1.0f)).xyz ;
	texCoord_v = texCoord;
	gl_Position = PVMmatrix * vec4(position, 1.0f);   
}
