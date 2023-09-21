#pragma once
#include <map>
#include <string>
#include <opencv2/core.hpp>
#include <array>
using std::array;
namespace ArrayTool
{
    /*
        @param array:输入的数组
        @brief 返回数组的长度，注意这里必须引用的数组，而不是数组的指针，才能有效
    */
    template <class T>
    int getlength(T &array)
    {
        return (sizeof(array) / sizeof(array[0]));
    }
};
class VaoDispatch
{
public:
    static VaoDispatch &
    getInstance()
    {
        static VaoDispatch VaoDispatch;
        return VaoDispatch;
    } // instance

    VaoDispatch(const VaoDispatch &) = delete;
    VaoDispatch &operator=(const VaoDispatch &) = delete;
    unsigned int getVao(std::string name);
    /*
        @param name:Vao的名字
        @param data:顶点的数据
        @brief 添加Vao
    */
    void addVao(std::string name,float data[],int length);
    /*
        @param name::Vao的名字
        @param VBO_Data:顶点的数据
        @param EBO_Data:索引的数据
        @param VBO_Length:VBO数据的长度
        @param EBO_Length:EBO数据的长度
        @brief 加载Vao,该vao为使用索引的vao
    */
    void addVao(std::string name,float VBO_Data[],unsigned int  EBO_Data[],int VBO_Length,int EBO_Length);



private:
    /*
        @param data:顶点的数据
        @brief 加载Vao，返回Vaobuffer的id
    */
    unsigned int loadVao(float data[],int length);

        /*
        @param VBO_Data:顶点的数据
        @param EBO_Data:索引的数据
        @param VBO_Length:VBO数据的长度
        @param EBO_Length:EBO数据的长度
        @brief 加载Vao,该vao为使用索引的vao
    */
    unsigned int loadVao(float VBO_Data[],unsigned int EBO_Data[],int VBO_Length,int EBO_Length);


    std::map<std::string, unsigned int> vaoGather;
    VaoDispatch(){};
    ~VaoDispatch(){};
};