#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
in vec3 color;
in vec3 normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	// ambient
	float ambientStrength = 0.75;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(lightDir, norm), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * color;
	FragColor = vec4(result, 1.0f);
}