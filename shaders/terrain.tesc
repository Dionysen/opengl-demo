#version 450 core

//how many times this shader will run per patch (n?of out control points)
// 设置每一个patch的控制点数目,也就是说每一个patch都要运行n次此着色器
layout (vertices = 4) out;


// gl_InvocationID是每一个patch中控制点的索引值

in VertexShaderOut {
	vec2 texCoord;
	vec2 texCoord2;
} tcs_in[];

out TCShaderOut {
	vec2 texCoord;
	vec2 texCoord2;
} tcs_out[];

layout(location = 11) uniform mat4 view;
layout(location = 13) uniform mat4 projection;
layout(location = 14) uniform vec4 frustumPlanes[6];

layout (binding = 0) uniform sampler2D displacementTexture;

//desired pixels per tri edge
layout(location = 22) uniform float tessellatedTriWidth;
layout(location = 23) uniform ivec2 viewportSize;
layout(location = 24) uniform float displacementScale;


///////////////////////////////////
//FRUSTUM CULLING
///////////////////////////////////

struct AABB {
	vec3 min, max;
};


// 检查平面和AABB之间的相交性
bool planeAABBIntersection(AABB aabb, vec4 plane) {
	vec3 halfDiagonal =  (aabb.max - aabb.min) * 0.5;
	vec3 center = aabb.min + halfDiagonal;
	
	float e = halfDiagonal.x + abs(plane.x) + 
				halfDiagonal.y + abs(plane.y) +
				halfDiagonal.z + abs(plane.z);
				
	float s = dot(center, plane.xyz) + plane.w;
	
	if(s - e > 0.0) return false; //outside
	else if(s + e < 0.0) return true; //inside
	else return true; //intersecting
}

// 检查视锥体平面和AABB之间的相交性
bool frustumAABBIntersection(AABB aabb) {
	bool intersection = false;
	for(int i = 0; i < 6; ++i) {
		bool result = planeAABBIntersection(aabb, frustumPlanes[i]);
		if(!result) return false;
		else intersection = true;
	}
	return intersection;
}


// displacementScale是高度图的位移缩放因子,创建一个包围整个patch的AABB
void createPatchAABB(vec3 p1, vec3 p2, vec3 p3, vec3 p4, out AABB aabb) {
	//Using knowledge of the input vertices
	aabb.min.x = p4.x;
	aabb.min.y = min(0.0, displacementScale);
	aabb.min.z = p4.z;
	
	aabb.max.x = p2.x;
	aabb.max.y = max(0.0, displacementScale);
	aabb.max.z = p2.z;
}

///////////////////////////////////
//LOD COMPUTATION
///////////////////////////////////

// Project a sphere into clip space and return the number of triangles that are required
// to fit across the screenspace diameter.

// 将一个球体投影到屏幕空间，并返回所需的三角形数量，以适应屏幕上的直径
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
	//return clamp(distance(clip0, clip1) / tessellatedTriWidth, 1.0, 64.0);
}

void main() {							
	if(gl_InvocationID == 0) {  // 只有一个patch的第一个控制点,才执行以下代码
	
		// 创建一个AABB
		AABB aabb; 
		createPatchAABB(gl_in[0].gl_Position.xyz, 
						gl_in[1].gl_Position.xyz,
						gl_in[2].gl_Position.xyz,
						gl_in[3].gl_Position.xyz,
						aabb);
						
		if(frustumAABBIntersection(aabb)) {	// 如果AABB与视锥体相交
			//Copy control points positions to mess with their y without passing to the pipeline
			//复制控制点位置，以在不传递到渲染管线的情况下修改其y坐标
			vec4 p0 = gl_in[0].gl_Position;
			vec4 p1 = gl_in[1].gl_Position;
			vec4 p2 = gl_in[2].gl_Position;
			vec4 p3 = gl_in[3].gl_Position;
			
			//Displace control points on y, to account for height on LOD computations
			// 根据高度图采样值来修改控制点的y坐标，并转化成在范围内的高度
			p0.y += texture(displacementTexture, tcs_in[0].texCoord).r * displacementScale;
			p1.y += texture(displacementTexture, tcs_in[1].texCoord).r * displacementScale;
			p2.y += texture(displacementTexture, tcs_in[2].texCoord).r * displacementScale;
			p3.y += texture(displacementTexture, tcs_in[3].texCoord).r * displacementScale;
				
			/*const float maxX = max(abs(p1.x - p0.x), abs(p1.x - p2.x));
			const float maxY = max(abs(p1.y - p0.y), abs(p1.y - p2.y));
			const float maxZ = max(abs(p1.z - p0.z), abs(p1.z - p2.z));
			const float sideLen = max(max(maxX, maxY), maxZ);*/
			//const float sideLen = max(distance(p1, p0), distance(p1, p2));

			// 计算三角形边的像素数，以用于细分级别计算。
			const float sideLen = p1.x - p0.x; //abusing knowing the tiles are square 假设这些平铺是正方形的

			// 计算并设置外部细分级别
			gl_TessLevelOuter[0] = sphereToScreenSpaceTessellation(p3.xyz, p0.xyz, sideLen);
			gl_TessLevelOuter[1] = sphereToScreenSpaceTessellation(p0.xyz, p1.xyz, sideLen);
			gl_TessLevelOuter[2] = sphereToScreenSpaceTessellation(p1.xyz, p2.xyz, sideLen);
			gl_TessLevelOuter[3] = sphereToScreenSpaceTessellation(p2.xyz, p3.xyz, sideLen);
			// 计算并设置内部细分级别
			gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) * 0.5;
			gl_TessLevelInner[1] = (gl_TessLevelOuter[0], gl_TessLevelOuter[2]) * 0.5;
		}
		else {
		// 如果AABB与视锥体不相交，将细分级别设置为-1，表示不需要细分
			gl_TessLevelOuter[0] = -1.0;
			gl_TessLevelOuter[1] = -1.0;
			gl_TessLevelOuter[2] = -1.0;
			gl_TessLevelOuter[3] = -1.0;
			
			gl_TessLevelInner[0] = -1.0;
			gl_TessLevelInner[1] = -1.0;
		}
	}


	// 直通纹理坐标
	tcs_out[gl_InvocationID].texCoord = tcs_in[gl_InvocationID].texCoord;
	tcs_out[gl_InvocationID].texCoord2 = tcs_in[gl_InvocationID].texCoord2;
	
	// 直通顶点坐标
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}