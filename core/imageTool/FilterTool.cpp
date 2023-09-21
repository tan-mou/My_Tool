#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <glm/glm.hpp>
#include <ctime>
#include <chrono>
#include <queue>
#include "FilterTool.h"
#include "ImageTool.h"
shared_ptr<cv::Mat> ImageTool::FilterTool::filter2D(shared_ptr<cv::Mat> src, cv::Mat kern)
{
    // 固有代码
    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>());
    // 实现代码
    cv::filter2D(*src, *result, src->depth(), kern);
    // 固有代码
    return result;
}
shared_ptr<cv::Mat> ImageTool::FilterTool::filter2DShow(shared_ptr<cv::Mat> src, cv::Mat kern, int index)
{
    // 固有代码
    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>());
    // 实现代码
    cv::filter2D(*src, *result, src->depth(), kern);
    // 固有代码
    string windowName = "filter2D " + std::to_string(index);
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(windowName, *result);
    return result;
}
shared_ptr<cv::Mat> ImageTool::FilterTool::adaptiveBinaryThreshold(shared_ptr<cv::Mat> src, int blockSize, int C,bool isInv)
{

    unsigned int flag;
    // 实现代码
    if (isInv)
    {
        flag = cv::THRESH_BINARY_INV;
    }
    else
    {
        flag = cv::THRESH_BINARY;
    }
    // 固有代码
    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>());
    // 实现代码
    if (src->channels() == 1)
    {
        cv::adaptiveThreshold(*src, *result, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,isInv, blockSize, C);
    }
    else if (src->channels() == 3)
    {
        cv::Mat gray;
        cv::cvtColor(*src, gray, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(gray, *result, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,isInv, blockSize, C);
    }

    // 固有代码
    return result;
}
shared_ptr<cv::Mat> ImageTool::FilterTool::adaptiveBinaryThresholdShow(shared_ptr<cv::Mat> src, int blockSize, int C, bool isInv,int index)
{
    // 固有代码
    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>());
    unsigned int flag;
    // 实现代码
    if(isInv)
    {
        flag= cv::THRESH_BINARY_INV;
    }
    else
    {
        flag= cv::THRESH_BINARY;
    }
    if (src->channels() == 1)
    {
        cv::adaptiveThreshold(*src, *result, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, flag, blockSize, C);
    }
    else if (src->channels() == 3)
    {
        cv::Mat gray;
        cv::cvtColor(*src, gray, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(gray, *result, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, flag, blockSize, C);
    }

    // 固有代码
    string windowName = "adaptiveBinaryThresholdShow " + std::to_string(index);
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(windowName, *result);
    return result;
}

vector<cv::Point2f> ImageTool::FilterTool::findChessBoard(shared_ptr<cv::Mat> src, cv::Size pattern)
{
    // 固有代码
    vector<cv::Point2f> result;
    int flag = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;
    bool isFind;
    if (src->channels() == 3)
    {
        cv::Mat gray;
        cv::cvtColor(*src, gray, cv::COLOR_BGR2GRAY);
        // 实现代码
        isFind = cv::findChessboardCorners(gray, pattern, result, flag);
    }
    else if (src->channels() == 1)
    {
        // 实现代码
        isFind = cv::findChessboardCorners(*src, pattern, result, flag);
    }

    return result;
}
vector<cv::Point2f> ImageTool::FilterTool::findChessBoardShow(shared_ptr<cv::Mat> src, cv::Size pattern, int index)
{
    // 固有代码
    cv::Mat tmp;
    vector<cv::Point2f> result;
    int flag = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;
    bool isFind;
    if (src->channels() == 3)
    {
        cv::Mat gray;
        cv::cvtColor(*src, gray, cv::COLOR_BGR2GRAY);
        src->copyTo(tmp);
        // 实现代码
        isFind = cv::findChessboardCorners(gray, pattern, result, flag);
        cv::drawChessboardCorners(tmp, pattern, result, isFind);
    }
    else if (src->channels() == 1)
    {
        // 实现代码
        isFind = cv::findChessboardCorners(*src, pattern, result, flag);
        src->copyTo(tmp);
        cv::drawChessboardCorners(tmp, pattern, result, isFind);
    }
    // 固有代码
    string windowName = "findChessBoard " + std::to_string(index);
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(windowName, tmp);
    return result;
}
