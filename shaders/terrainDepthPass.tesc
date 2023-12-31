#version 450 core

//how many times this shader will run per patch (n?of out control points)
layout (vertices = 4) out;

in VertexShaderOut {
	vec2 texCoord;
} tcs_in[];

out TCShaderOut {
	vec2 texCoord;
} tcs_out[];

layout(location = 11) uniform mat4 view;
layout(location = 13) uniform mat4 projection;
layout(location = 22) uniform float tessellatedTriWidth;
layout(location = 23) uniform ivec2 viewportSize;
layout(location = 24) uniform float displacementScale;

layout (binding = 0) uniform sampler2D displacementTexture;

///////////////////////////////////
//LOD COMPUTATION
///////////////////////////////////

// Project a sphere into clip space and return the number of triangles that are required
// to fit across the screenspace diameter.
float sphereToScreenSpaceTessellation(vec3 cp0, vec3 cp1, float diameter) {
	vec3 mid = (cp0 + cp1) * 0.5;
	
	vec4 p0 = view * vec4(mid, 1.0);
	vec4 p1 = p0;
	p1.x += diameter;

	vec4 clip0 = projection * p0;
	vec4 clip1 = projection * p1;
	
	clip0 /= clip0.w;
	clip1 /= clip1.w;
	
	clip0.xy = ((clip0.xy * 0.5) + 0.5) * viewportSize;
	clip1.xy = ((clip1.xy * 0.5) + 0.5) * viewportSize;
	
	return distance(clip0, clip1) / tessellatedTriWidth; //spec says it will be clamped by GL
	//return clamp(distance(clip0, clip1) / tessellatedTriWidth, 10.0, 64.0);
}

void main() {							
	if(gl_InvocationID == 0) {
		
		vec4 p0 = gl_in[0].gl_Position;
		vec4 p1 = gl_in[1].gl_Position;
		vec4 p2 = gl_in[2].gl_Position;
		vec4 p3 = gl_in[3].gl_Position;
			
		//Displace control points on y, to account for height on LOD computations
		p0.y += texture(displacementTexture, tcs_in[0].texCoord).r * displacementScale;
		p1.y += texture(displacementTexture, tcs_in[1].texCoord).r * displacementScale;
		p2.y += texture(displacementTexture, tcs_in[2].texCoord).r * displacementScale;
		p3.y += texture(displacementTexture, tcs_in[3].texCoord).r * displacementScale;
				
		const float sideLen = p1.x - p0.x; //abusing knowing the tiles are square
			
		gl_TessLevelOuter[0] = sphereToScreenSpaceTessellation(p3.xyz, p0.xyz, sideLen);
		gl_TessLevelOuter[1] = sphereToScreenSpaceTessellation(p0.xyz, p1.xyz, sideLen);
		gl_TessLevelOuter[2] = sphereToScreenSpaceTessellation(p1.xyz, p2.xyz, sideLen);
		gl_TessLevelOuter[3] = sphereToScreenSpaceTessellation(p2.xyz, p3.xyz, sideLen);
		
		gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) * 0.5;
		gl_TessLevelInner[1] = (gl_TessLevelOuter[0], gl_TessLevelOuter[2]) * 0.5;
	}
	tcs_out[gl_InvocationID].texCoord = tcs_in[gl_InvocationID].texCoord;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}