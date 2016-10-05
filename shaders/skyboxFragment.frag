#version 140
 
uniform samplerCube skyboxSampler;
uniform int fogActive;

in vec3 texCoord_v;
out vec4 color_f;

vec4 fogColor = vec4(0.6f, 0.6f, 0.6f, 1.0f);

void main() {
	color_f = texture(skyboxSampler, texCoord_v);
	
	// fog
	if (fogActive != 0) {
		float fogDensity = 1.0f;
		float fogFunc = 0.0; 
		fogFunc = exp(-pow(fogDensity * abs(gl_FragCoord.z / gl_FragCoord.w), 2.0f));      
		fogFunc = 1.0f- clamp(fogFunc, 0.0f, 1.0f);
		color_f = mix(color_f, fogColor, fogFunc);
	}
}