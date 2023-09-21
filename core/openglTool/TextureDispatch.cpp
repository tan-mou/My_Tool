#include "TextureDispatch.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <glad.h>
#include <iostream>
#include <utility>

void TextureDispatch::addTexture(std::string name, cv::Mat &src)
{
    unsigned int textureID;
    if (!src.empty())
    {
        textureID = loadTexture(src.data, src.cols, src.rows, src.channels());
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    textureGather.insert({name, textureID});
}

void TextureDispatch::addTexture(std::string name, std::string filePath)
{
    cv::Mat src = cv::imread(filePath, cv::IMREAD_UNCHANGED);
    if(src.channels()==3)
    {
        cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
    }
    else if(src.channels()==4)
    {
        cv::cvtColor(src, src, cv::COLOR_BGRA2RGBA);
    }
    cv::flip(src, src, 0);
    unsigned int textureID;
    if (!src.empty())
    {
        textureID = loadTexture(src.data, src.cols, src.rows, src.channels());
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    textureGather.insert({name, textureID});
}

unsigned int TextureDispatch::loadTexture(unsigned char *data, int width, int height, int channels)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (channels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (channels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,data);
    }
    else
        return 0;
    glGenerateMipmap(GL_TEXTURE_2D);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}
unsigned int TextureDispatch::getTexture(std::string name)
{
    auto result = textureGather.find(name);
    if (result == textureGather.end())
    {
        std::cout << "ERROR::TEXTUREDISPATCH::Cant find texture" << std::endl;
        return 0;
    }
    return result->second;
}
