#version 140

struct Material {
	vec3  ambient;             // ambient component
	vec3  diffuse;             // diffuse component
	vec3  specular;            // specular component
	float shininess;           // sharpness of specular reflection
	bool  useTexture;          // defines whether the texture is used or not
};

struct Light {                 // structure describing light parameters
	vec4  position;            // light position
	vec3  ambient;             // intensity & color of the ambient component
	vec3  diffuse;             // intensity & color of the diffuse component
	vec3  specular;            // intensity & color of the specular component
	vec3  spotDirection;       // spotlight direction
	float spotCosCutoff;       // cosine of the spotlight's half angle
	float spotExponent;        // distribution of the light energy within the reflector's cone (center->cone's edge)
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float intensity;
};

struct Fog {
	float fogDensity;
	vec4 fogColor;
};

smooth in vec2 texCoord_v;      // fragment texture coordinates
smooth in vec3 normal_v;		//camera space normal
smooth in vec3 position_v;      // camera space position

uniform sampler2D texSampler;   // sampler for the texture access
uniform Material material;      // current material
uniform float time;             // time used for simulation of moving lights (such as sun)
uniform int lampActive;
uniform int fogActive;
uniform float sunTime;

uniform mat4 PVMmatrix;        // Projection * View * Model  --> model to clip coordinates
uniform mat4 Vmatrix;          // View                       --> world to eye coordinates
uniform mat4 Mmatrix;          // Model                      --> model to world coordinates
uniform mat4 normalMatrix;     // inverse transposed VMmatrix


// flashlight
uniform float flashlightIntensity;
uniform vec3 flashlightPosition;
uniform vec3 flashlightDirection;

// lamp
uniform vec3 lampPosition;
uniform float lampIntensity;

Light sunLight;
Light flashLight;
Light lampLight;
Fog fog;

out vec4       color_f;        // outgoing fragment color

vec4 setLight(Light light, Material material, vec3 vertexNormal, vec3 vertexPosition) {
	vec3 result = vec3(0.0f);
	
	vec3 L = vec3(0.0f);
	if (light.position.w == 0) {
		L = normalize(light.position.xyz);
	} else {
		L = normalize(light.position.xyz - vertexPosition);
	} 
	vec3 N = normalize(vertexNormal);
	vec3 V = normalize(-vertexPosition);

	result += light.ambient * material.ambient;
	result += max(dot(L,N), 0.0f) * light.diffuse * material.diffuse;
	result += pow(max(dot(reflect(-L, N), V), 0.0f), material.shininess) * light.specular * material.specular;

	float spotFactor = max(dot(-L, normalize(light.spotDirection)), 0.0f);
	if(light.spotCosCutoff > spotFactor) {
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} 

	if (light.position.w != 0) {
		float dst = distance(light.position.xyz, vertexPosition);
		float attenuationFactor = 1.0f / (	light.constantAttenuation + light.linearAttenuation * dst + light.quadraticAttenuation * (dst * dst) );
										
		result *= attenuationFactor;
		result *= pow(spotFactor, light.spotExponent);
		result *= light.intensity;
	}

	return vec4(result, 1.0f);
}

void main() {
	vec3 globalAmbientLight = vec3(0.20f);
  	vec4 outputColor = vec4(globalAmbientLight * material.ambient, 0.0f); //ambient light from the environment
	
	// sun
	sunLight.ambient  = vec3(0.0f);
	sunLight.diffuse  = vec3(1.0f, 1.0f, 0.7f);
	sunLight.specular = vec3(1.0f);
	sunLight.position = Vmatrix * vec4(sin(sunTime), cos(sunTime), 1.0f, 0.0f); // moving against plane
	outputColor += setLight(sunLight, material, normal_v, position_v);
	
	color_f = outputColor;

	// lamp
	lampLight.ambient = vec3(0.2f);
	lampLight.diffuse = vec3(1.0f, 1.0f, 0.35f);
	lampLight.specular = vec3(1.0f);
	lampLight.intensity = lampIntensity;
	lampLight.position = Vmatrix * vec4(lampPosition, 1.0f);
	lampLight.constantAttenuation = 0.5f;
	lampLight.spotExponent = 0.0f;
	lampLight.linearAttenuation = 1.0f;
	lampLight.quadraticAttenuation = 0.0f;
	
	if (lampActive == 1) color_f += setLight(lampLight, material, normal_v, position_v);

	// flashLight
	flashLight.ambient = vec3(0.25f);
	flashLight.diffuse = vec3(1.0f, 1.0f, 0.8f);
	flashLight.specular = vec3(1.0f);
	flashLight.intensity = flashlightIntensity;
	flashLight.position = Vmatrix * vec4(flashlightPosition, 1.0f);
	flashLight.spotDirection = normalize((Vmatrix * vec4(flashlightDirection, 0.0f)).xyz);
	flashLight.constantAttenuation = 0.0f;
	flashLight.linearAttenuation = 1.5f;
	flashLight.quadraticAttenuation = 0.0f;
	flashLight.spotExponent = 10.0f;
	flashLight.spotCosCutoff = 0.97f;

	color_f += setLight(flashLight, material, normal_v, position_v);
		
	// texture 
	if (material.useTexture)  color_f = color_f * texture(texSampler, texCoord_v);

	// fog
	if (fogActive != 0) {
		fog.fogDensity = 1.0f;
		fog.fogColor = vec4(0.6f, 0.6f, 0.6f, 1.0f);

		float fogFunc = 0.0f;
		fogFunc = exp(-pow(fog.fogDensity * abs(gl_FragCoord.z / gl_FragCoord.w), 2.0f));      
		fogFunc = 1.0f - clamp(fogFunc, 0.0f, 1.0f);
		color_f = mix(color_f, fog.fogColor, fogFunc);
	}
}
