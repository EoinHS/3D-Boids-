#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram
{
private:
	// Reference for the shader program
	int ID;
public:
	// Constructor, takes two file names, assumes they are located in the resource files
	ShaderProgram(const char* vertPath, const char* fragPath);
	// Activates the shader
	void use();
	// Utility functions to set values of uniforms
	void setMatrix(std::string name, glm::mat4 value);
	void setBool(std::string& name, GLboolean value);
	void setInt(std::string& name, GLuint value);
	void setFloat(std::string& name, GLfloat value);
	void setVec3(std::string name, glm::vec3 value);
};

#endif