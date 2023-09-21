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
    namespace MaskTool
    {
        // 根据找到的棋盘格角点去放遮罩
        shared_ptr<cv::Mat> putCornerMask(shared_ptr<cv::Mat> src, vector<cv::Point2f>);
        shared_ptr<cv::Mat> putCornerMaskShow(shared_ptr<cv::Mat> src, vector<cv::Point2f>, int index = 0);
        //指定标定布的大概位置
        shared_ptr<cv::Mat> generateCabMask(shared_ptr<cv::Mat>src,int minY,int maxY,int width=1280,int height=720);
        shared_ptr<cv::Mat> generateCabMaskShow(shared_ptr<cv::Mat>src, int minY,int maxY,int index=0,int width=1280,int height=720);
    }
}