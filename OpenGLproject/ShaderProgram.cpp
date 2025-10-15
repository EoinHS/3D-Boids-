#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertPath, const char* fragPath)
{
	// 1. Retrieve the vertex/fragment source code from filePaths
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertPath);
		fShaderFile.open(fragPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Milord! Milord!, the file hath not been read!" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. Compile Shader
	GLuint vertex, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile error if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "MiLord! MiLord! the vertex shader hasn't compiled!" << infoLog << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if anyu
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "MiLord! MiLord! the fragment shader hasn't compiled!" << infoLog << std::endl;
	}

	// Combine vertex and fragment shaders into a program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		std::cout << "VERTEX SHADER:" << std::endl << vertex << std::endl << std::endl;

		std::cout << "FRAGMENT SHADER:" << std::endl << fragment << std::endl << std::endl;

		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "MiLord! MiLord! there was an error linking the program!" << infoLog << std::endl;
	}

	// delete the shaders as the're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ShaderProgram::use() 
{
	glUseProgram(ID);
}

void ShaderProgram::setMatrix(std::string name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setBool(std::string &name, GLboolean value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::setFloat(std::string& name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::setInt(std::string& name, GLuint value)
{
	glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::setVec3(std::string name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}