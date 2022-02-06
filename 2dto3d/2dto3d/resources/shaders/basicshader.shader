#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;

uniform mat4 MVP;

void main()
{
	fragmentColor = vertexColor;
	gl_Position = MVP * position;
};

#shader fragment
#version 330 core


in vec3 fragmentColor;
out vec3 color;
void main()
{
	color = fragmentColor;
	
};