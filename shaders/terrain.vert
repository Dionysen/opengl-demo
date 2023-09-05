#version 450 core

out VertexShaderOut {
	vec2 texCoord;
	vec2 texCoord2;
} vs_out; // 这里输出了两个纹理坐标

// 一共输出三个，实例化对象的坐标和两个纹理坐标
const vec3 PATCH_QUAD[] = vec3[] 
		(vec3(-0.5, 0.0, 0.5),
		 vec3(0.5, 0.0, 0.5),
		 vec3(0.5, 0.0, -0.5),
		 vec3(-0.5, 0.0, -0.5)
		); // 每个格子为一个正方形
									
layout(location = 21) uniform ivec2 gridSize; // 网格数量 64*64
layout(location = 25) uniform vec2 terrainSize; // 整体地形大小

void main() {
	const vec2 PATCH_SIZE = terrainSize / vec2(gridSize); // 网格大小，用地形大小除以网格数量

	vec2 offset;  // 偏移量
	
	// 对于每一个实例化对象，设置其偏移量，将所有实例化对象平铺到地形上
	//offset.x = mod(float(gl_InstanceID), gridSize.x) * PATCH_SIZE.x;	
	offset.x = float(gl_InstanceID & (gridSize.x - 1)) * PATCH_SIZE.x; // assuming grid size power of 2
	//offset.y = floor(float(gl_InstanceID) / gridSize.x) * PATCH_SIZE.y;	
	offset.y = float(int(float(gl_InstanceID) / float(gridSize.x))) * PATCH_SIZE.y;
	
	// displacement and normals textures
	// 法线纹理坐标
	vs_out.texCoord = (((PATCH_QUAD[gl_VertexID].xz + vec2(0.5)) * PATCH_SIZE) + offset) / terrainSize;
	vs_out.texCoord.t = 1.0 - vs_out.texCoord.t;
	
	// 反射率纹理坐标
	vs_out.texCoord2 = PATCH_QUAD[gl_VertexID].xz + vec2(0.5);
	vs_out.texCoord2.t = 1.0 - vs_out.texCoord2.t;
	
	// 实例化对象的位置
	vec3 instancePosition = (PATCH_QUAD[gl_VertexID] * vec3(PATCH_SIZE.x, 1, PATCH_SIZE.y)) + 
							vec3(offset.x, 0, offset.y);
	//center terrain on xz 0,0
	instancePosition.xz -= (terrainSize / 2.0) - (PATCH_SIZE / 2.0);
	
	gl_Position = vec4(instancePosition, 1.0);
	// 这里输出顶点数据
}