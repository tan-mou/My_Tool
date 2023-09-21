#include "ShaderDispatch.h"
#include "Shader.h"
#include <glad.h>
#include <iostream>
#include <utility>
#include <fstream>

using std::string;
ShaderDispatch::ShaderDispatch()
{
//ShaderDispatch默认的一些绘制单元
    unsigned int circleProgram = generateShader(shader::vCircleShaderStr, shader::fCircleShaderStr);
    unsigned int canvasProgram = generateShader(shader::vCanvasShaderStr, shader::fCanvasShaderStr);
    unsigned int lineProgram = generateShader(shader::vLineShaderStr, shader::fLineShaderStr);
    unsigned int fontProgram = generateShader(shader::vfontShaderStr, shader::ffontShaderStr);
    shaderGather.insert(std::make_pair("circle", circleProgram));
    shaderGather.insert(std::make_pair("canvas", canvasProgram));
    shaderGather.insert(std::make_pair("line", lineProgram));
    shaderGather.insert(std::make_pair("font", fontProgram));
}

ShaderDispatch::~ShaderDispatch()
{
}
void ShaderDispatch::addShader(std::string name, const char *vertexShader, const char *fragmentShader)
{
    unsigned int program = generateShader(vertexShader, fragmentShader);
    shaderGather.insert({name, program});
}

void ShaderDispatch::addShader(std::string name, string vertexShader, string fragmentShader)
{
    unsigned int program = generateShader(vertexShader.c_str(), fragmentShader.c_str());
    shaderGather.insert({name, program});
}
void ShaderDispatch::addShader(std::string name, string shaderFilePath)
{
    std::pair<string,string> shader=loadShaderPath(shaderFilePath);
    unsigned int program=generateShader(shader.first.c_str(),shader.second.c_str());
    shaderGather.insert({name,program});
}

unsigned int ShaderDispatch::getShader(std::string name)
{
    auto result = shaderGather.find(name);
    if (result == shaderGather.end())
    {
        std::cout << "ERROR::SHADERDISPATCH::Cant find shader" << std::endl;
        return 0;
    }
    return result->second;
}
unsigned int ShaderDispatch::generateShader(const char *vertexShaderStr, const char *fragmentShaderStr)
{
    auto getShaderError = [](int id)
    {
        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "SHADER" << id << "::ERROR" << std::endl;
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
    };

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderStr);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderStr);
    if (!vertexShader || !fragmentShader)
    {
        std::cout << "ERROR::SHADERDISPATCH::Cant Load Shader" << std::endl;
        return 0;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    getShaderError(shaderProgram);
    return shaderProgram;
}
unsigned int ShaderDispatch::loadShader(unsigned int type, const char *shaderSrc)
{
    GLuint shader;
    // Create the shader object
    shader =glCreateShader(type);
    if (shader == 0)
    {
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);
    return shader;
}

std::pair<string, string>  ShaderDispatch::loadShaderPath(string shaderPath)
{
    std::ifstream file;
    string line;
    file.open(shaderPath, std::ios::in);
    string fragmentShader = "";
    string vertexShader = "";
    bool isVertex = false;
    bool isFragment = false;
    if (!file.is_open())
    {
        std::cout << "cant load shader file ,please check file path" << std::endl;
        return std::pair<string, string>("", "");
    }
    while (std::getline(file, line))
    {
        // 判断读取vertex shader还是fragment shader
        if (line == "@Vertex")
        {
            isVertex = true;
            isFragment = false;
            continue;
        }
        if (line == "@Fragment")
        {
            isVertex = false;
            isFragment = true;
            continue;
        }
        if (isVertex)
        {
            vertexShader += line+"\n";
        }
        if (isFragment)
        {
            fragmentShader += line+"\n";
        }
    }
    
    return std::pair<string, string>{vertexShader, fragmentShader};
}