#version 450 core

layout (binding = 1) uniform sampler2D normalsTexture;
layout (binding = 2) uniform sampler2DArray albedoTextures;

layout(location = 24) uniform float displacementScale;

layout(location = 30) uniform vec3 sunDirection;
layout(location = 31) uniform vec3 sunColor;
uniform bool enableWireframe;

in GShaderOut {
	vec3 positionWorld;
	vec3 positionEye;
	vec2 texCoord;
	vec2 texCoord2;
	noperspective vec3 distToEdges;
} fs_in;

out vec4 color;

vec4 mixAlbedoTextures(float v, float t1, float t2, float t3, float transition) {
	if(v < t1 - transition) {
		return texture(albedoTextures, vec3(fs_in.texCoord2, 0.0));
	}
	else if(v < t1 + transition) {
		float factor = (v - (t1 - transition)) / (2.0 * transition);
		return mix(texture(albedoTextures, vec3(fs_in.texCoord2, 0.0)), 
					texture(albedoTextures, vec3(fs_in.texCoord2, 1.0)),
					factor);
	}
	else if(v < t2 - transition) {
		return texture(albedoTextures, vec3(fs_in.texCoord2, 1.0));
	}
	else if(v < t2 + transition) {
		float factor = (v - (t2 - transition)) / (2.0 * transition);
		return mix(texture(albedoTextures, vec3(fs_in.texCoord2, 1.0)), 
					texture(albedoTextures, vec3(fs_in.texCoord2, 2.0)),
					factor);
	}
	else if(v < t3 - transition) {
		return texture(albedoTextures, vec3(fs_in.texCoord2, 2.0));
	}
	else if(v < t3 + transition) {
		float factor = (v - (t3 - transition)) / (2.0 * transition);
		return mix(texture(albedoTextures, vec3(fs_in.texCoord2, 2.0)), 
					texture(albedoTextures, vec3(fs_in.texCoord2, 3.0)),
					factor);
	}
	else {
		return texture(albedoTextures, vec3(fs_in.texCoord2, 3.0));
	}
}

void main() {
	//LIGHTING
	vec3 normal = normalize((texture(normalsTexture, fs_in.texCoord).rbg * 2.0) - 1.0);
	
	vec3 ambient = sunColor * 0.35;
	vec3 diffuse = sunColor * max(0.0, dot(normal, -sunDirection));
	vec3 light = ambient + diffuse;


	//MULTITEXTURING HEIGHT BASED
	float height = fs_in.positionWorld.y;

	const float THRES1 = displacementScale * 0.15;
	const float THRES2 = displacementScale * 0.45;
	const float THRES3 = displacementScale * 0.9;
	const float TRANSITION = displacementScale * 0.1;
	
	vec4 tex = mixAlbedoTextures(fs_in.positionWorld.y, THRES1, THRES2, THRES3, TRANSITION);
	
	float slope = 1.0 - normal.y;
	tex = mix(tex, 
				texture(albedoTextures, vec3(fs_in.texCoord2, 3.0)),
				slope * 0.15);
				
	color = vec4(light, 1.0) * tex;

	if(enableWireframe) {
		const vec4 WIREFRAME_COLOR = vec4(0.0);
		const float WIREFRAME_THICKNESS = 1.0;
		
		float wireframeStrength = 0.0;

		float minDistToEdge = min(fs_in.distToEdges.x, min(fs_in.distToEdges.y, fs_in.distToEdges.z));
		if(minDistToEdge <= WIREFRAME_THICKNESS) {
			wireframeStrength = smoothstep(0.0, 1.0, 1.0 - (minDistToEdge / WIREFRAME_THICKNESS));
		}

		color = WIREFRAME_COLOR * wireframeStrength + (1.0 - wireframeStrength) * color;
	}
}