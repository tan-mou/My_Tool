#include "ImageTool.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/utils/logger.hpp>

int ImageTool::cvWindow::base = 50;
int ImageTool::cvWindow::xRatio = 16;
int ImageTool::cvWindow::yRatio = 9;
// ImageTool
shared_ptr<cv::Mat> ImageTool::getCvShared(cv::Mat &&src)
{
    return std::make_shared<cv::Mat>(src);
}
shared_ptr<cv::Mat> ImageTool::getCvShared(cv::Mat &src)
{
    return std::make_shared<cv::Mat>(src);
}
shared_ptr<cv::Mat> ImageTool::getCvGray(shared_ptr<cv::Mat> src)
{
    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>());
    cv::cvtColor(*src, *result, cv::COLOR_BGR2GRAY);
    return result;
}
shared_ptr<cv::Mat> ImageTool::getCvShared(string path)
{
    return std::make_shared<cv::Mat>(cv::imread(path));
}

void ImageTool::noCvInfoLog()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

void ImageTool::showImage(shared_ptr<cv::Mat> src, string name)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(name, *src);
}
void ImageTool::showImage(cv::Mat src, string name)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(name, src);
}
void ImageTool::showImage(cv::Mat src, const char *name)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(name, src);
}
cv::Point ImageTool::getIntersection(cv::Point orign1, cv::Point direction1, cv::Point orign2, cv::Point direction2)
{
    cv::Mat B(2, 1, CV_32FC1);
    cv::Mat A(2, 2, CV_32FC1);
    B.at<float>(0, 0) = orign2.x - orign1.x;
    B.at<float>(1, 0) = orign2.y - orign1.y;
    A.at<float>(0, 0) = direction1.x;
    A.at<float>(1, 0) = direction1.y;
    A.at<float>(0, 1) = -direction2.x;
    A.at<float>(1, 1) = -direction2.y;
    cv::Mat t;
    cv::Point result;
    cv::solve(A, B, t);
    float t1 = t.at<float>(0, 0);
    float t2 = t.at<float>(1, 0);
    result = orign1 + t1 * direction1;
    return result;
}

void ImageTool::drawContours(shared_ptr<cv::Mat> src, vector<vector<cv::Point>> contours, string name)
{
    cv::Mat tmpShow;
    src->copyTo(tmpShow);

    // 固有代码
    if (tmpShow.channels() == 1)
        cv::cvtColor(tmpShow, tmpShow, cv::COLOR_GRAY2BGR);
    polylines(tmpShow, contours, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(name, tmpShow);
    cv::imwrite("D:/Document/Code/My_Tool/contours.png",tmpShow);
}
void ImageTool::drawPoints(shared_ptr<cv::Mat> src, vector<cv::Point2f> points, string name)
{
    // show circle
    cv::Mat tmpShow;
    src->copyTo(tmpShow);
    if (tmpShow.channels() == 1)
        cv::cvtColor(tmpShow, tmpShow, cv::COLOR_GRAY2BGR);

    for (auto l : points)
    {
        cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
    }
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(name, tmpShow);
}
