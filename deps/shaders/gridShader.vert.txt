#version 450
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

layout(row_major) uniform;

uniform GeneralUniformBuffer{
	vec4 generalUniformBuffer[9];
};

out Vertex{
	vec4 color;
}outVertex;

void main(){
	mat4 matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], generalUniformBuffer[2], generalUniformBuffer[3]);
	matrix = matrix * mat4(generalUniformBuffer[4], generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
	
	gl_Position = vec4(matrix * position);
	outVertex.color = color;
}
