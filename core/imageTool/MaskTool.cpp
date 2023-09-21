#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <ctime>
#include <chrono>
#include "MaskTool.h"
#include "FilterTool.h"
#include "ImageTool.h"
shared_ptr<cv::Mat> ImageTool::MaskTool::putCornerMask(shared_ptr<cv::Mat> src, vector<cv::Point2f> corners)
{
    shared_ptr<cv::Mat> result=std::make_shared<cv::Mat>();
    src->copyTo(*result);
    auto x_minmax = std::minmax_element(corners.begin(), corners.end(), [](const cv::Point2f &p1, const cv::Point2f &p2)
                                        { return p1.x < p2.x; });
    auto y_minmax = std::minmax_element(corners.begin(), corners.end(), [](const cv::Point2f &p1, const cv::Point2f &p2)
                                        { return p1.y < p2.y; });
    int minX = x_minmax.first->x;
    int maxX = x_minmax.second->x;
    int minY = y_minmax.first->y;
    int maxY = y_minmax.second->y;
    cv::Rect roi(minX, minY, maxX - minX, maxY - minY);
    (*result)(roi) = cv::Scalar(255);
    return result;
}
shared_ptr<cv::Mat>  ImageTool::MaskTool::putCornerMaskShow(shared_ptr<cv::Mat> src, vector<cv::Point2f>corners, int index)
{
    
    shared_ptr<cv::Mat> result=std::make_shared<cv::Mat>();
    src->copyTo(*result);
    auto x_minmax = std::minmax_element(corners.begin(), corners.end(), [](const cv::Point2f &p1, const cv::Point2f &p2)
                                        { return p1.x < p2.x; });
    auto y_minmax = std::minmax_element(corners.begin(), corners.end(), [](const cv::Point2f &p1, const cv::Point2f &p2)
                                        { return p1.y < p2.y; });
    float minX = x_minmax.first->x-25;
    float maxX = x_minmax.second->x+25;
    float minY = y_minmax.first->y-25;
    float maxY = y_minmax.second->y+25;
    cv::Rect roi(minX, minY, maxX - minX, maxY - minY);
    (*result)(roi) = cv::Scalar(255);

    // 固有代码
    string windowName="putCornerMaskShow "+std::to_string(index);
    cv::namedWindow(windowName,cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName,cvWindow::base*cvWindow::xRatio,cvWindow::base*cvWindow::yRatio);
    cv::imshow(windowName,*result);
    return result;
}

shared_ptr<cv::Mat> ImageTool::MaskTool::generateCabMask(shared_ptr<cv::Mat> src, int minY, int maxY,int width,int height)
{
    shared_ptr<cv::Mat> result=(std::make_shared<cv::Mat>(height,width,CV_8UC1));
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            if(i>minY&&i<maxY)
            {
                result->at<unsigned char>(i,j)=1;
            }
            else
            {
                result->at<unsigned char>(i,j)=0;
            }
        }
    }
    return result;
}
shared_ptr<cv::Mat> ImageTool::MaskTool::generateCabMaskShow(shared_ptr<cv::Mat> src, int minY, int maxY, int index,int width,int height)
{

    shared_ptr<cv::Mat> result = (std::make_shared<cv::Mat>(height, width, CV_8UC1));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i > minY && i < maxY)
            {
                result->at<unsigned char>(i, j) = 255;
            }
            else
            {
                result->at<unsigned char>(i, j) = 0;
            }
        }
    }
        // 固有代码
    string windowName="generateCabMaskShow "+std::to_string(index);
    cv::namedWindow(windowName,cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName,width,height);
    cv::imshow(windowName,*result);
    return result;
}