#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

// --------- helper
std::string Shader::readFile(const char* path)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream ss;
    try
    {
        file.open(path);
        ss << file.rdbuf();
        file.close();
    }
    catch(std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << path << '\n';
    }
    return ss.str();
}

void Shader::checkCompile(unsigned int id, const std::string& type)
{
    int success;
    char infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(id, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << type << "\n" << infoLog << "\n";
        }
    }
    else
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(id, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
        }
    }
}

// --------- ctor
Shader::Shader(const char* vertPath, const char* fragPath)
{
    std::string vCode = readFile(vertPath);
    std::string fCode = readFile(fragPath);
    const char* vSrc = vCode.c_str();
    const char* fSrc = fCode.c_str();

    unsigned int vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vSrc, nullptr);
    glCompileShader(vID);
    checkCompile(vID, "VERTEX");

    unsigned int fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fSrc, nullptr);
    glCompileShader(fID);
    checkCompile(fID, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vID);
    glAttachShader(ID, fID);
    glLinkProgram(ID);
    checkCompile(ID, "PROGRAM");

    glDeleteShader(vID);
    glDeleteShader(fID);
}

// --------- uniform helpers
void Shader::use() const { glUseProgram(ID); }

void Shader::setBool(const std::string& name, bool v) const
{ glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)v); }
void Shader::setInt(const std::string& name, int v) const
{ glUniform1i(glGetUniformLocation(ID, name.c_str()), v); }
void Shader::setFloat(const std::string& name, float v) const
{ glUniform1f(glGetUniformLocation(ID, name.c_str()), v); }
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{ glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{ glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
                     1, GL_FALSE, &mat[0][0]); }
