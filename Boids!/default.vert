#version 330 core

//Positions/Coords
layout(location = 0) in vec3 aPos;
//Colours
layout(location = 1) in vec3 aColour;
//Normal
layout(location = 2) in vec3 aNorm;

//Ouputs colour for the Fragment shader
out vec3 color;
//Normal to the plane (used for lighting)
out vec3 normal;
//Posistion of the fragment in world space
out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	color = aColour;
	normal = aNorm;

	gl_Position  = projection * view * vec4(FragPos, 1.0);
}