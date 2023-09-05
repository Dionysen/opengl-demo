#version 450 core

out VertexShaderOut {
	vec2 texCoord;
	vec2 texCoord2;
} vs_out; // ���������������������

// һ�����������ʵ��������������������������
const vec3 PATCH_QUAD[] = vec3[] 
		(vec3(-0.5, 0.0, 0.5),
		 vec3(0.5, 0.0, 0.5),
		 vec3(0.5, 0.0, -0.5),
		 vec3(-0.5, 0.0, -0.5)
		); // ÿ������Ϊһ��������
									
layout(location = 21) uniform ivec2 gridSize; // �������� 64*64
layout(location = 25) uniform vec2 terrainSize; // ������δ�С

void main() {
	const vec2 PATCH_SIZE = terrainSize / vec2(gridSize); // �����С���õ��δ�С������������

	vec2 offset;  // ƫ����
	
	// ����ÿһ��ʵ��������������ƫ������������ʵ��������ƽ�̵�������
	//offset.x = mod(float(gl_InstanceID), gridSize.x) * PATCH_SIZE.x;	
	offset.x = float(gl_InstanceID & (gridSize.x - 1)) * PATCH_SIZE.x; // assuming grid size power of 2
	//offset.y = floor(float(gl_InstanceID) / gridSize.x) * PATCH_SIZE.y;	
	offset.y = float(int(float(gl_InstanceID) / float(gridSize.x))) * PATCH_SIZE.y;
	
	// displacement and normals textures
	// ������������
	vs_out.texCoord = (((PATCH_QUAD[gl_VertexID].xz + vec2(0.5)) * PATCH_SIZE) + offset) / terrainSize;
	vs_out.texCoord.t = 1.0 - vs_out.texCoord.t;
	
	// ��������������
	vs_out.texCoord2 = PATCH_QUAD[gl_VertexID].xz + vec2(0.5);
	vs_out.texCoord2.t = 1.0 - vs_out.texCoord2.t;
	
	// ʵ���������λ��
	vec3 instancePosition = (PATCH_QUAD[gl_VertexID] * vec3(PATCH_SIZE.x, 1, PATCH_SIZE.y)) + 
							vec3(offset.x, 0, offset.y);
	//center terrain on xz 0,0
	instancePosition.xz -= (terrainSize / 2.0) - (PATCH_SIZE / 2.0);
	
	gl_Position = vec4(instancePosition, 1.0);
	// ���������������
}