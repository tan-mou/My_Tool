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
    namespace CalibrationTool
    {
        // 四边形类，能够找到包装了四个顶点，和是否连接过的数据
        struct Square;
        // 我们的寻找棋盘格角点方法
        bool findCorner(const shared_ptr<cv::Mat> src, vector<cv::Point2f> &corners, cv::Size pattern,bool isShow=false,int index=0);
        /*
            @param
            @param squaresInput是要找到的所有四边形轮廓
            @param squaresOutput是找到的连接起来的四边形轮廓的集合
            @param squareIndex是当前要连接的四边形的索引
            @param findIndex是当前要连接的四边形连接的出发点，如果为1,2,3,4表示已经连接的点，如果为-1则为初始没有连接的四边形，将会从0开始连接四边形
            @brief 连接四边形
        */
        void linkSquare(vector<shared_ptr<Square>> &squaresInput, vector<shared_ptr<Square>> &squaresOutput, int squareIndex);
        /*
            @param squaresInput是连接起来的四边形轮廓的集合
            @param cornersOutput输出这些四边形轮廓交界处角点
            @brief 根据四边形轮廓返回交界处的角点

        */
        void getSquareCorner(const vector<shared_ptr<Square>> &squaresInput, vector<cv::Point2f> &cornersOutput);
        /*
            @param squaresInput是连接起来的四边形轮廓的集合
            @param cornersOutput输出这些四边形轮廓交界处角点
            @param patternSize棋盘格大小
            @brief 若角点数比需求的角点数少一，则使用缺省四边形去推测这个角点
        */
        void getSquareCornerLackOne(const vector<shared_ptr<Square>> &squaresInput, vector<cv::Point2f> &cornersOutput);
        /*
            @param squaresInput是连接起来的四边形轮廓的集合
            @param cornersOutput输出这些四边形轮廓交界处角点
            @param patternSize棋盘格大小
            @brief 若角点数比需求的角点数少二，则使用缺省四边形去推测这个角点
        */
        void getSquareCornerLackTwo(const vector<shared_ptr<Square>> &squaresInput, vector<cv::Point2f> &cornersOutput);
        /*
            @param squaresInput是连接起来的四边形轮廓的集合
            @param cornersOutput输出这些四边形轮廓交界处角点
            @param patternSize棋盘格大小
            @brief 若角点数比需求的角点数少一，则使用缺省四边形去推测这个角点，

        */
        void getSquareCornerLackOne2X2(const vector<shared_ptr<Square>> &SquaresInput, vector<cv::Point2f> &cornersOutput);
        /*
            @param contours:输入轮廓
            @param squares:输出四边形找到的四边形
            @param sqaureMinAcreage:允许的最小四边形面积
            @param accurate:近似四边形的精度
            @brief 找四边形轮廓
        */
        void findSquare(const vector<vector<cv::Point>> &contours, vector<vector<cv::Point>> &squares, int sqaureMinAcreage, int accurate);
        /*
            @param src:输入图像
            @param countours:输出的轮廓
            @brief 寻找所有轮廓
        */
        void findContours(const shared_ptr<cv::Mat> src, vector<vector<cv::Point>> &contours);
        void findContoursShow(const shared_ptr<cv::Mat> src, vector<vector<cv::Point>> &contours, int index = 0);

    }
}
