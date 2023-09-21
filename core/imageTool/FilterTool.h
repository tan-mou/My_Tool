#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
using std::map;
using std::shared_ptr;
using std::string;
using std::vector;
namespace ImageTool
{
    /*
        @param
        @param
        @param
        @brief
    */
    namespace FilterTool
    {
        /**
            @param src:输入的图像
            @param kern:卷积核
            @brief 能够根据卷积核，返回卷积后的图像
        */
        shared_ptr<cv::Mat> filter2D(shared_ptr<cv::Mat> src, cv::Mat kern);
        shared_ptr<cv::Mat> filter2DShow(shared_ptr<cv::Mat> src, cv::Mat kern, int index = 0);
        /**
            @param src:输入的图像
            @param blockSize:领域大小
            @param C:常数大小
            @brief 自适应二值化
        */
        shared_ptr<cv::Mat> adaptiveBinaryThreshold(shared_ptr<cv::Mat> src, int blockSize, int C,bool isInv=false);
        shared_ptr<cv::Mat> adaptiveBinaryThresholdShow(shared_ptr<cv::Mat> src, int blockSize, int C, bool isInv=false,int index = 0);
        /**
            @param src:输入图像
            @param pattern:输入的棋盘格大小
            @brief opencv查找棋盘格方法
        */
        vector<cv::Point2f> findChessBoard(shared_ptr<cv::Mat> src, cv::Size pattern);
        vector<cv::Point2f> findChessBoardShow(shared_ptr<cv::Mat> src, cv::Size pattern, int index = 0);
    }


}