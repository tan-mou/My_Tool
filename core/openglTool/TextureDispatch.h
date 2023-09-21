#pragma once
#include <map>
#include <string>
#include <opencv2/core.hpp>
class TextureDispatch
{
public:
    static TextureDispatch &
    getInstance()
    {
        static TextureDispatch TextureDispatch;
        return TextureDispatch;
    } // instance

    TextureDispatch(const TextureDispatch &) = delete;
    TextureDispatch &operator=(const TextureDispatch &) = delete;
    unsigned int getTexture(std::string name);
    /*
        @param name:texture的名字
        @param src:cv::Mat数据
        @brief 添加texture
    */
    void addTexture(std::string name,cv::Mat& src);
    /*
        @param name:texture的名字
        @param filePath:texture的路径
        @brief 添加texture
    */
    void addTexture(std::string name,std::string filePath);


private:
    /*
        @param data:texture的数据
        @param width:texture的宽
        @param height:texture的高
        @param channels:通道数，3或4
        @brief 加载texture，返回texturebuffer的id
    */
    unsigned int loadTexture(unsigned char *data, int width,int height,int channels);
    std::map<std::string, unsigned int> textureGather;
    TextureDispatch(){};
    ~TextureDispatch(){};
};