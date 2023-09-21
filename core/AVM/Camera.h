#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <opencv2/calib3d.hpp>
using std::shared_ptr;
using std::string;
using std::vector;
class Camera
{
public:
    Camera(cv::Mat intrinsic, cv::Mat distortCoeff)
    {
        this->intrinsic = intrinsic;
        this->distortCoeff = distortCoeff;
    }
    Camera(float dx_f, float dy_f, float cx, float cy, float k1, float k2, float k3, float k4)
    {
        intrinsic = cv::Mat::zeros(3, 3, CV_32F);
        intrinsic.at<float>(0, 0) = dx_f;
        intrinsic.at<float>(1, 1) = dy_f;
        intrinsic.at<float>(0, 2) = cx;
        intrinsic.at<float>(1, 2) = cy;
        intrinsic.at<float>(2, 2) = 1;
        distortCoeff = cv::Mat(4, 1, CV_32F);
        distortCoeff.at<float>(0, 0) = k1;
        distortCoeff.at<float>(1, 0) = k2;
        distortCoeff.at<float>(2, 0) = k3;
        distortCoeff.at<float>(3, 0) = k4;
    }
    // 根据像素和世界坐标，建立相机的外参
    void initCameraExtrinsic(const vector<cv::Point2f> pixelPos, const vector<cv::Point> worldPos)
    {
        cv::Mat zeroDistCoeff = cv::Mat::zeros(1, 5, CV_32F);
        cv::Mat eyeMatrix = cv::Mat::eye(3, 3, CV_32F);
        vector<cv::Point3f> worldF3;
        // 从cv::Point转换到2f
        vector<cv::Point2f> pixelF2;
        vector<cv::Point2f> pixelF2_2;
        vector<cv::Point3f> worldF3_2;

        vector<cv::Point2f> pixelF2_3;
        vector<cv::Point3f> worldF3_3;
        int j=0;

        for (auto i : pixelPos)
        {
            pixelF2.push_back(cv::Point2f(i.x, i.y));

            if(j<12);
                // pixelF2.push_back(cv::Point2f(i.x, i.y));
            else
                pixelF2_2.push_back(cv::Point2f(i.x, i.y));
            // else if(j<20)
            //     pixelF2_3.push_back(cv::Point2f(i.x, i.y));

            j++;

        }
       int m = 0;
        for (auto i : worldPos)
        {
            worldF3.push_back(cv::Point3f(i.x, i.y, 0));
            if(m<12);
                // worldF3.push_back(cv::Point3f(i.x, i.y, 0));
            else
                worldF3_2.push_back(cv::Point3f(i.x, i.y, 0));
            // else if(m<20)
            //     worldF3_3.push_back(cv::Point3f(i.x, i.y, 0));
            m++;
        }
        vector<cv::Point2f> undistortCorner;
        vector<cv::Point2f> undistortCorner_2;
        vector<cv::Point2f> undistortCorner_3;

        cv::fisheye::undistortPoints(pixelF2, undistortCorner, intrinsic, distortCoeff,intrinsic);
        cv::fisheye::undistortPoints(pixelF2_2, undistortCorner_2, intrinsic, distortCoeff,intrinsic);
        cv::fisheye::undistortPoints(pixelF2_3, undistortCorner_3, intrinsic, distortCoeff,intrinsic);
        rvec.empty();
        tvec.empty();
        // rvec,tvec是外参数据
        cv::solvePnP(worldF3, undistortCorner, intrinsic, zeroDistCoeff, rvec, tvec);
        if(worldF3_2.size()>=4)
            cv::solvePnP(worldF3_2, undistortCorner_2, intrinsic, zeroDistCoeff, rvec, tvec,true);

        // if(worldF3_3.size()>=4)
        //     cv::solvePnP(worldF3_3, undistortCorner_3, intrinsic, zeroDistCoeff, rvec, tvec,true);

        // cv::Mat homony=cv::findHomography(worldF3,pixelF2);
        vector<cv::Point2f> reHomonyCorner;
        vector<cv::Point2f> reProjectCorners;
        cv::fisheye::projectPoints(worldF3, reProjectCorners, rvec, tvec, intrinsic, distortCoeff);
        float reProjectionError = 0;
        auto k = pixelF2.begin();
        for (auto j : reProjectCorners)
        {
            float X = j.x - k->x;
            float Y = j.y - k->y;
            // std::cout << sqrt(X * X + Y * Y) << std::endl;
            reProjectionError += sqrt(X * X + Y * Y) / pixelF2.size();
            k++;
        }
        std::cout << "extrinsic reProjectionError:" << reProjectionError << std::endl;
    }

public:
    // 摄像机的内参
    cv::Mat intrinsic;
    cv::Mat distortCoeff;
    // 摄像机的外参
    cv::Mat rvec;
    cv::Mat tvec;
};