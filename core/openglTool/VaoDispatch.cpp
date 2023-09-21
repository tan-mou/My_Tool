#include "VaoDispatch.h"
#include "glad.h"
#include <opencv2/highgui.hpp>
#include <iostream>
unsigned int VaoDispatch::getVao(std::string name)
{
    auto result = vaoGather.find(name);
    if (result == vaoGather.end())
    {
        std::cout << "ERROR::VAODISPATCH::Cant find vao" << std::endl;
        return 0;
    }
    return result->second;
}
void VaoDispatch::addVao(std::string name, float data[], int length)
{
    unsigned int vaoID = loadVao(data, length);
    vaoGather.insert({name, vaoID});
}
void VaoDispatch::addVao(std::string name,float VBO_Data[],unsigned int  EBO_Data[],int VBO_Length,int EBO_Length)
{
    unsigned int vaoID = loadVao(VBO_Data, EBO_Data,VBO_Length,EBO_Length);
    vaoGather.insert({name, vaoID});
}
unsigned int VaoDispatch::loadVao(float data[], int length)
{
    unsigned int vaoID;
    unsigned int vboID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // texture buffer
    return vaoID;
}
unsigned int VaoDispatch::loadVao(float VBO_Data[], unsigned int  EBO_Data[], int VBO_Length, int EBO_Length)
{
    unsigned int vaoID;
    unsigned int vboID;
    unsigned int eboID;
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1,&eboID);
    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, VBO_Length * sizeof(float), VBO_Data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    //绑定ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBO_Length*sizeof(unsigned int ), EBO_Data, GL_STATIC_DRAW);




    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // texture buffer
    return vaoID;

}
