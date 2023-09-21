#pragma once
#include<opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include<memory>
#include <string>
#include<vector>
using std::vector;
using std::string;
using std::shared_ptr;
namespace ImageTool
{
    namespace cvWindow
    {
        extern int xRatio;
        extern int yRatio;
        extern int base;
    }
    /**
        @brief 根据mat源数据获取指针
    */
    shared_ptr<cv::Mat> getCvShared(cv::Mat &&src);
    shared_ptr<cv::Mat> getCvShared(cv::Mat& src);
    shared_ptr<cv::Mat> getCvShared(string path);
    shared_ptr<cv::Mat> getCvGray(shared_ptr<cv::Mat> src);
    void noCvInfoLog();
    void showImage(shared_ptr<cv::Mat>,string name);
    void showImage(cv::Mat,const char* name);
    void showImage(cv::Mat,string name);
    cv::Point getIntersection(cv::Point orign1,cv::Point direction1,cv::Point orign2,cv::Point direction2);


    /**
        @param 输入图像
        @param 轮廓
        @param 图像名字
        @brief 画轮廓到输入的图像中
    */
    void drawContours(shared_ptr<cv::Mat> src,vector<vector<cv::Point>>contours,string name);

    /**
        @param 输入图像
        @param 点
        @param 图像名字
        @brief 画点到图像中
    */
    void drawPoints(shared_ptr<cv::Mat> src,vector<cv::Point2f> points,string name);
}