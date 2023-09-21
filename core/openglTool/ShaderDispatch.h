#pragma once
#include <map>
#include <string>
using std::string;
class ShaderDispatch
{
public:
    static ShaderDispatch &
    getInstance()
    {
        static ShaderDispatch ShaderDispatch;
        return ShaderDispatch;
    } // instance

    ShaderDispatch(const ShaderDispatch &) = delete;
    ShaderDispatch &operator=(const ShaderDispatch &) = delete;
    /*
        @param name:shader的名字
        @brief 获取shader
    */
    unsigned int getShader(std::string name);
    /*
        @param name:shader的名字
        @param vertexShader:顶点着色器
        @param vertexShader:片段着色器
        @brief 添加shader
    */
    void addShader(std::string name, const char *vertexShader, const char *fragmentShader);
    /*
        @param name:shader的名字
        @param vertexShader:顶点着色器
        @param vertexShader:片段着色器
        @brief 添加shader
    */
    void addShader(std::string name, string vertexShader, string fragmentShader);
    /*
        @param name:shader的名字
        @param shaderFilePath:shader
        @brief 添加shader
    */
    void addShader(std::string name, string shaderFilePath);

private:
    /*
        @param shaderPath:加载shader文件路径
        @brief 加载shader文件，返回加载的两个shader内容
    */
    std::pair<string, string> loadShaderPath(string shaderPath);
    /*
        @param vertexShader:顶点着色器的内容
        @param fragmentShader:片段着色器的内容
        @brief 生成shader，首先编译，其次连接
    */
    unsigned int generateShader(const char *vertexShader, const char *fragmentShader);
    /*
        @param type:shader的类型，GL_VERTEX_SHADER和GL_FRAGMENT_SHADER
        @param shaderSrc:shader的内容
        @brief 编译shader
    */
    unsigned int loadShader(unsigned int type, const char *shaderSrc);


    std::map<std::string, unsigned int> shaderGather;
    ShaderDispatch();
    ~ShaderDispatch();
};