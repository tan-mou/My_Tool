#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <string>
#include <vector>
using std::shared_ptr;
using std::string;
using std::vector;
#define MAP_FRONT_AREA 0
#define MAP_RIGHT_AREA 1
#define MAP_REAR_AREA 2
#define MAP_LEFT_AREA 3
//控制是否输出世界坐标和对应的uv坐标的宏
#define IS_OUTPUT_UV_WORLD 0

namespace BirdView
{
    struct CarPositionPARA
    {
        // 左上角的世界坐标位置
        cv::Point2f initalWorldPoint;
        // 车模左上角世界坐标位置
        cv::Point2f carModelInitPoint;
        // 车宽
        float carWidth;
        // 车高
        float carHeight;
        // 鸟瞰图中世界坐标的宽
        float worldWidth;
        // 鸟瞰图中世界坐标的高
        float worldHeight;
        // 拼接区的大小
        float spliceSize;
    };
    /**
     @param para 车的位置的参数
     @param intrinsic 相机内参
     @param distortCoefff 相机的畸变系数
     @param rvec 旋转向量
     @param tvec 平移向量
     @param imageWidth 图像的宽
     @param imageHeight 图像的高
     @param type  map所在位置的区域
     @brief 生成2d鸟瞰图的map
    */
    shared_ptr<cv::Mat> generateMap2D(shared_ptr<CarPositionPARA> para,
    cv::Mat intrinsic,cv::Mat distortCoeff,cv::Mat rvec,cv::Mat tvec,
    unsigned int imageWidth, unsigned int imageHeight, unsigned int type);
    /**
     @param para 车的位置的参数
     @param intrinsic 相机内参
     @param distortCoefff 相机的畸变系数
     @param rvec 旋转向量
     @param tvec 平移向量
     @param imageWidth 图像的宽
     @param imageHeight 图像的高
     @param type  map所在位置的区域
     @brief 生成3d环视图的map
    */
    vector<shared_ptr<cv::Mat>> generateMap3D(shared_ptr<CarPositionPARA> para,
    cv::Mat intrinsic,cv::Mat distortCoeff,cv::Mat rvec,cv::Mat tvec,
    unsigned int imageWidth, unsigned int imageHeight, unsigned int type);

    
    
}