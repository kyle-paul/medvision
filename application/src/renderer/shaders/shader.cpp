#include "shader.h"

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    shaderID = CreateShader(vertexPath, fragmentPath);
}

void Shader::Bind() const
{
    glUseProgram(shaderID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

unsigned int Shader::CreateShader(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::vector<unsigned int> modules;
    modules.push_back(CompileShader(vertexPath, GL_VERTEX_SHADER));
    modules.push_back(CompileShader(fragmentPath, GL_FRAGMENT_SHADER));
    shaderID = glCreateProgram();
    for (unsigned int shaderModule : modules)
    {
        glAttachShader(shaderID, shaderModule);
    }
    glLinkProgram(shaderID);
    int success;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char errorLog[1024];
        glGetProgramInfoLog(shaderID, 1024, NULL, errorLog);
        std::cout << "Shader linking error:\n"
                  << errorLog << '\n';
    }
    for (unsigned int shaderModule : modules)
    {
        glDeleteShader(shaderModule);
    }
    return shaderID;
}

unsigned int Shader::CompileShader(const std::string &filepath, unsigned int type)
{
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open(filepath);
    while (getline(file, line))
    {
        bufferedLines << line << '\n';
    }
    std::string shaderSource = bufferedLines.str();
    const char *shaderSrc = shaderSource.c_str();
    bufferedLines.str("");
    file.close();

    unsigned int shaderModule = glCreateShader(type);
    glShaderSource(shaderModule, 1, &shaderSrc, NULL);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char errorLog[1024];
        glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
        std::cout << "Shader Module compilation error:\n"
                  << errorLog << std::endl;
    }

    return shaderModule;
}

int Shader::GetUniformLocation(const std::string &name)
{
    if (UniformLocationCache.find(name) != UniformLocationCache.end())
    {
        return UniformLocationCache[name];
    }
    int location = glGetUniformLocation(shaderID, name.c_str());
    if (location == -1)
    {
        std::cout << "No active uniform variable with name " << name << " found" << std::endl;
    }
    UniformLocationCache[name] = location;
    return location;
}

void Shader::SetUniform1i(const std::string &name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string &name, float value)
{
    glUniform1f(
        GetUniformLocation(name),
        value);
}

void Shader::SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4)
{
    glUniform4f(
        GetUniformLocation(name),
        v1, v2, v3, v4);
}

void Shader::SetUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(
        GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

Shader::~Shader()
{
    glDeleteShader(shaderID);
}