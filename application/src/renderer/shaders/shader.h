#pragma once
#include "../../exlibs.h"
#include "../../standard.h"

class Shader
{
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Uniform control
    void SetUniform1i(const std::string &name, int value);
    void SetUniform1f(const std::string &name, float value);
    void SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4);
    void SetUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix);

private:
    unsigned int shaderID;
    std::unordered_map<std::string, int> UniformLocationCache;
    unsigned int CreateShader(const std::string &vertexPath, const std::string &fragmentPath);
    unsigned int CompileShader(const std::string &filepath, unsigned int type);
    int GetUniformLocation(const std::string &uniformName);
};