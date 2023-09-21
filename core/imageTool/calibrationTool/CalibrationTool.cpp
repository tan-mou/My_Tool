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
#include <iostream>
#include "CalibrationTool.h"
#include "FilterTool.h"
#include "ImageTool.h"
struct ImageTool::CalibrationTool::Square
{
    Square(vector<cv::Point> CORNERS);
    int getLinkCount()
    {
        int result = 0;
        for (auto i = linkIndexMap.begin(); i != linkIndexMap.end(); i++)
        {
            if (i->second)
                result++;
        }
        return result;
    };
    void cleanLink()
    {
        linkIndexMap[0] = false;
        linkIndexMap[1] = false;
        linkIndexMap[2] = false;
        linkIndexMap[3] = false;
    }
    vector<cv::Point> corners;
    vector<cv::Point> sides;   // 边长
    cv::Point midLineVertical; // 垂直中心线，方向沿着opecv图像坐标y为正方向
    cv::Point midLineHorizon;  // 水平中心线，方向沿着opencv图像坐标x为正方向
    bool isIinked = false;
    map<int, bool> linkIndexMap; // 已经连接过的角点索引

    // 四边形的中点
    cv::Point center;     // 中心点
    double minSideLength; // 最小边长
};
bool ImageTool::CalibrationTool::findCorner(shared_ptr<cv::Mat> src, vector<cv::Point2f> &corners, cv::Size pattern, bool isShow, int index)
{
    unsigned char kernData[3][3] =
        {
            {
                1,
                1,
                1,
            },
            {
                1,
                1,
                1,
            },
            {
                1,
                1,
                1,
            }};
    unsigned char kernData2[3][3] =
        {
            {
                0,
                1,
                0,
            },
            {
                1,
                1,
                1,
            },
            {
                0,
                1,
                0,
            }};
    bool isFind = false;
    // 二值化区间
    int beginBlockSize = 101;
    int endBlockSize = 201;

    // Const
    int beginC = 0;
    int endC = 7;
    // 四边形近似的精确度
    int beginAccurate = 3;
    int endAccurate = 7;
    // 四边形近似的最小面积
    int minArea = 50;

    // 腐蚀次数
    int erosionCount = 1;
    // 两种卷积核
    cv::Mat kern1(3, 3, CV_8UC1, (unsigned char *)kernData);
    cv::Mat kern2(3, 3, CV_8UC1, (unsigned char *)kernData2);
    // 需要检测角点数量
    int findCornerCount = pattern.width * pattern.height;
    shared_ptr<cv::Mat> gray;

    if (src->channels() != 1)
        gray = ImageTool::getCvGray(src);
    else
        gray = src;
    // 第一层循环：修改自适应二值化的blockSize
    for (int i = beginBlockSize; i < endBlockSize; i += 20)
    {
        // 第二层循环：修改自适应二值化的const
        for (int j = beginC; j < endC; j += 2)
        {
            shared_ptr<cv::Mat> binary = FilterTool::adaptiveBinaryThreshold(gray, i, j);
            vector<vector<cv::Point>> squaresData;
            vector<vector<cv::Point>> contoursData;
            // 第三层循环：修改腐蚀次数
            for (int o = 0; o < erosionCount; o++)
            {
                if (o % 2 == 0)
                {
                    binary = FilterTool::filter2D(binary, kern1);
                }
                else
                {
                    binary = FilterTool::filter2D(binary, kern2);
                }
                CalibrationTool::findContours(binary, contoursData);
                // 第四层循环：修改寻找四边形的精确度
                for (int k = beginAccurate; k < endAccurate; k += 1)
                {
                    // 四边形相似
                    CalibrationTool::findSquare(contoursData, squaresData, minArea, k);
                    vector<shared_ptr<Square>> squares;

                    // 根据找到的四边形轮廓初始化Square类
                    for (auto m : squaresData)
                    {
                        Square tmpS(m);
                        squares.push_back(std::make_shared<Square>(tmpS));
                    }
                    // 连接每个四边形
                    for (int n = 0; n < squares.size(); n++)
                    {
                        if (squares[n]->isIinked)
                            continue;
                        vector<shared_ptr<Square>> resultSquare;
                        linkSquare(squares, resultSquare, n);
                        vector<vector<cv::Point>> tmpData;
                        for (auto m : resultSquare)
                        {
                            tmpData.push_back(m->corners);
                        }

                        vector<cv::Point2f> resultCorner;
                        getSquareCorner(resultSquare, resultCorner);
                        // 如果找到了等于棋盘格数量的角点返回找到的角点
                        if (resultCorner.size() == findCornerCount)
                        {
                            if (isShow)
                            {
                                // //固有代码
                                cv::Mat tmpShow2;
                                src->copyTo(tmpShow2);
                                polylines(tmpShow2, squaresData, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
                                string windowName = "squaresData_src " + std::to_string(index);
                                cv::namedWindow(windowName, cv::WINDOW_NORMAL);
                                cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
                                cv::imshow(windowName, tmpShow2);
                            }
                            // 对于角点等于4的情况我们要加限制条件，连接他的四边形数量也要为4
                            if (resultCorner.size() == 4)
                            {
                                if (resultSquare.size() == 4)
                                {
                                    corners = resultCorner;
                                    isFind = true;
                                    break;
                                }
                                else
                                {
                                    continue;
                                }
                            }
                            else
                            {
                                corners = resultCorner;
                                isFind = true;
                                break;
                            }
                        }
                        // 如果找到了等于棋盘格数量减1的数量的角点，做判断，使用边界判断的方法，填充缺失的一个角点
                        else if (resultCorner.size() == findCornerCount - 1 && resultCorner.size() > 3)
                        {
                            // 等于四的时候是特殊情况要特殊处理
                            if (findCornerCount == 4 && resultSquare.size() == 4)
                            {
                                getSquareCornerLackOne2X2(resultSquare, resultCorner);
                            }
                            else
                            {
                                getSquareCornerLackOne(resultSquare, resultCorner);
                            }
                            if (isShow)
                            {
                                // //固有代码
                                cv::Mat tmpShow2;
                                src->copyTo(tmpShow2);
                                polylines(tmpShow2, squaresData, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
                                string windowName = "squaresData_src " + std::to_string(index);
                                cv::namedWindow(windowName, cv::WINDOW_NORMAL);
                                cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
                                cv::imshow(windowName, tmpShow2);
                            }
                            // 固有代码
                            // cv::Mat tmpShow2;
                            // src->copyTo(tmpShow2);
                            // polylines(tmpShow2, tmpData, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
                            // string windowName = "squaresData " + std::to_string(resultCorner.size());
                            // cv::namedWindow(windowName, cv::WINDOW_NORMAL);
                            // cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
                            // cv::imshow(windowName, tmpShow2);

                            corners = resultCorner;
                            isFind = true;

                            break;
                        }
                    }

                    if (isFind)
                        break;
                }
                if (isFind)
                    break;
            }
            if (isFind)
                break;
        }
        if (isFind)
            break;
    }

    return isFind;
}
void ImageTool::CalibrationTool::linkSquare(vector<shared_ptr<Square>> &squaresInput, vector<shared_ptr<Square>> &squaresOutput, int squareBeginIndex)
{
    // 判断两个连接的角点是否在四边形的中点边长内
    auto isInternal = [](shared_ptr<Square> square1, int currentCornerIndex1, shared_ptr<Square> square2, int currentCornerIndex2) -> bool
    {
        // 四边形的四个点
        cv::Point point1 = square1->center;
        cv::Point point2 = ImageTool::getIntersection(square1->center, square1->midLineVertical, square2->center, square2->midLineHorizon);
        cv::Point point3 = square2->center;
        cv::Point point4 = ImageTool::getIntersection(square1->center, square1->midLineHorizon, square2->center, square2->midLineVertical);

        cv::Point side1 = point2 - point1;
        cv::Point side2 = point3 - point2;
        cv::Point side3 = point4 - point3;
        cv::Point side4 = point1 - point4;

        cv::Point corner1 = square1->corners[currentCornerIndex1];
        cv::Point corner2 = square2->corners[currentCornerIndex2];

        auto cross = [](cv::Point p1, cv::Point p2)
        {
            cv::Point3i P1(p1.x, p1.y, 0);
            cv::Point3i P2(p2.x, p2.y, 0);
            return P1.cross(P2);
        };
        auto jugment = [&](cv::Point corner)
        {
            cv::Point3i c1 = cross(corner - point1, side1);
            cv::Point3i c2 = cross(corner - point2, side2);
            cv::Point3i c3 = cross(corner - point3, side3);
            cv::Point3i c4 = cross(corner - point4, side4);

            if (c1.z > 0 && c2.z > 0 && c3.z > 0 && c4.z > 0)
                return true;
            else if (c1.z < 0 && c2.z < 0 && c3.z < 0 && c4.z < 0)
                return true;
            else
                return false;
        };
        bool isInCorner1 = jugment(corner1);
        bool isInCorner2 = jugment(corner2);
        return isInCorner1 && isInCorner2;
    };
    auto getDistance = [](cv::Point p1, cv::Point p2)
    {
        return sqrt(std::pow((p1.x - p2.x), 2) + (std::pow((p1.y - p2.y), 2)));
    };
    auto findMinCorner = [&](shared_ptr<Square> currentSquare, int currentCornerIndex, int &findSquareIndex)
    {
        double minDist = DBL_MAX;
        int minSquareIndex = -1;       // 最小距离的四边形的索引
        int minSquareCornerIndex = -1; // 最小距离的四边形的角点的索引
        // 遍历已经linked外的所有四边形，去寻找合适的角点
        for (int i = squareBeginIndex; i < squaresInput.size(); i++)
        {
            // 去除已经link的四边形
            if (squaresInput[i]->isIinked)
                continue;
            for (int j = 0; j < 4; j++)
            {

                double distance = getDistance(currentSquare->corners[currentCornerIndex], squaresInput[i]->corners[j]);
                if (distance < minDist)
                {
                    minDist = distance;
                    minSquareIndex = i;
                    minSquareCornerIndex = j;
                }
            }
        }
        // 找到了最小的四边形角点的索引,判断距离是否小于两个四边形的最小边长
        if (minSquareIndex != -1)
        {
            if (minDist < currentSquare->minSideLength && minDist < squaresInput[minSquareIndex]->minSideLength)
            {
                // 测试是否在中线包围的四边形范围内
                bool isIn = isInternal(currentSquare, currentCornerIndex, squaresInput[minSquareIndex], minSquareCornerIndex);
                // 通过了测试，则说明最小长的四边形为和当前四边形连接的四边形
                if (isIn)
                    findSquareIndex = minSquareIndex;
            }
        }
    };
    std::queue<shared_ptr<Square>> squareQueue;
    // 使用队列去完成四边形的连接
    // 当四边形入队列时，将其状态改为link
    squaresInput[squareBeginIndex]->isIinked = true;
    squaresOutput.push_back(squaresInput[squareBeginIndex]);
    squareQueue.push(squaresInput[squareBeginIndex]);
    while (!squareQueue.empty())
    {
        shared_ptr<Square> currentSquare = squareQueue.front();
        squareQueue.pop();
        for (int i = 0; i < 4; i++)
        {
            int findSquareIndex = -1;
            findMinCorner(currentSquare, i, findSquareIndex);
            // 如果找到了连接的角点，将此找到的四边形设置为linked，推入队列中
            if (findSquareIndex != -1)
            {
                squaresInput[findSquareIndex]->isIinked = true;
                squaresOutput.push_back(squaresInput[findSquareIndex]);
                squareQueue.push(squaresInput[findSquareIndex]);
            }
        }
    }
}
void ImageTool::CalibrationTool::getSquareCorner(const vector<shared_ptr<Square>> &squaresInput, vector<cv::Point2f> &cornersOutput)
{
    for (auto i : squaresInput)
    {
        i->cleanLink();
    }
    // 判断两个连接的角点是否在四边形的中线内
    auto isInternal = [](shared_ptr<Square> square1, int currentCornerIndex1, shared_ptr<Square> square2, int currentCornerIndex2) -> bool
    {
        // 四边形的四个点
        cv::Point point1 = square1->center;
        cv::Point point2 = ImageTool::getIntersection(square1->center, square1->midLineVertical, square2->center, square2->midLineHorizon);
        cv::Point point3 = square2->center;
        cv::Point point4 = ImageTool::getIntersection(square1->center, square1->midLineHorizon, square2->center, square2->midLineVertical);

        cv::Point side1 = point2 - point1;
        cv::Point side2 = point3 - point2;
        cv::Point side3 = point4 - point3;
        cv::Point side4 = point1 - point4;

        cv::Point corner1 = square1->corners[currentCornerIndex1];
        cv::Point corner2 = square2->corners[currentCornerIndex2];

        auto cross = [](cv::Point p1, cv::Point p2)
        {
            cv::Point3i P1(p1.x, p1.y, 0);
            cv::Point3i P2(p2.x, p2.y, 0);
            return P1.cross(P2);
        };
        auto jugment = [&](cv::Point corner)
        {
            cv::Point3i c1 = cross(corner - point1, side1);
            cv::Point3i c2 = cross(corner - point2, side2);
            cv::Point3i c3 = cross(corner - point3, side3);
            cv::Point3i c4 = cross(corner - point4, side4);
            if (c1.z > 0 && c2.z > 0 && c3.z > 0 && c4.z > 0)
                return true;
            else if (c1.z < 0 && c2.z < 0 && c3.z < 0 && c4.z < 0)
                return true;
            else
                return false;
        };
        bool isInCorner1 = jugment(corner1);
        bool isInCorner2 = jugment(corner2);
        return isInCorner1 && isInCorner2;
    };
    auto getDistance = [](cv::Point p1, cv::Point p2)
    {
        return sqrt(std::pow((p1.x - p2.x), 2) + (std::pow((p1.y - p2.y), 2)));
    };
    auto findMinCorner = [&](shared_ptr<Square> currentSquare, int currentCornerIndex, int &findSquareIndex, int &findSquareCornerIndex)
    {
        double minDist = DBL_MAX;
        int minSquareIndex = -1;       // 最小距离的四边形的索引
        int minSquareCornerIndex = -1; // 最小距离的四边形的角点的索引
        for (int i = 0; i < squaresInput.size(); i++)
        {
            if (squaresInput[i] == currentSquare)
                continue;
            for (int j = 0; j < 4; j++)
            {
                // 如果这个四边形的角点已经被找到过了，则不计算
                if (squaresInput[i]->linkIndexMap[j])
                    continue;
                double distance = getDistance(currentSquare->corners[currentCornerIndex], squaresInput[i]->corners[j]);
                if (distance < minDist)
                {
                    minDist = distance;
                    minSquareIndex = i;
                    minSquareCornerIndex = j;
                }
            }
        }
        // 找到了最小的四边形角点的索引,判断距离是否小于两个四边形的最小边长
        if (minSquareIndex != -1)
        {
            if (minDist < currentSquare->minSideLength && minDist < squaresInput[minSquareIndex]->minSideLength)
            {
                // 测试是否在中线包围的四边形范围内
                bool isIn = isInternal(currentSquare, currentCornerIndex, squaresInput[minSquareIndex], minSquareCornerIndex);
                // 通过了测试，则说明最小长的四边形为和当前四边形连接的四边形
                if (isIn)
                {
                    findSquareIndex = minSquareIndex;
                    findSquareCornerIndex = minSquareCornerIndex;
                }
            }
        }
    };

    for (int i = 0; i < squaresInput.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 如果这个四边形的角点已经被找到过了，则不计算
            if (squaresInput[i]->linkIndexMap[j])
                continue;
            int findSquareIndex = -1;
            int findSquareCornerIndex = -1;
            findMinCorner(squaresInput[i], j, findSquareIndex, findSquareCornerIndex);
            if (findSquareIndex != -1)
            {
                cv::Point2f corner;
                cv::Point point1 = squaresInput[i]->corners[j];
                cv::Point point2 = squaresInput[findSquareIndex]->corners[findSquareCornerIndex];
                squaresInput[i]->linkIndexMap[j] = true;
                squaresInput[findSquareIndex]->linkIndexMap[findSquareCornerIndex] = true;
                float x1 = point1.x;
                float x2 = point2.x;

                corner.x = ((float)point1.x + (float)point2.x) / 2.0f;
                corner.y = ((float)point1.y + (float)point2.y) / 2.0f;
                // std::cout << "x1:" << x1 << std::endl;
                // std::cout << "x2:" << x2<< std::endl;
                // std::cout<<"x"<<corner.x<<std::endl;
                // std::cout<<"y"<<corner.y<<std::endl;

                cornersOutput.push_back(corner);
            }
        }
    }
}
void ImageTool::CalibrationTool::findContours(shared_ptr<cv::Mat> src, vector<vector<cv::Point>> &contours)
{
    contours.clear();
    vector<cv::Vec4i> hierarchy;
    cv::findContours(*src, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
}
void ImageTool::CalibrationTool::findContoursShow(shared_ptr<cv::Mat> src, vector<vector<cv::Point>> &contours, int index)
{
    cv::Mat tmpShow;
    src->copyTo(tmpShow);
    contours.clear();
    vector<cv::Vec4i> hierarchy;
    cv::findContours(*src, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // 固有代码
    cv::cvtColor(tmpShow, tmpShow, cv::COLOR_GRAY2BGR);
    polylines(tmpShow, contours, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
    string windowName = "findSquare " + std::to_string(index);
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
    cv::imshow(windowName, tmpShow);
}
void ImageTool::CalibrationTool::findSquare(const vector<vector<cv::Point>> &contours, vector<vector<cv::Point>> &squares, int sqaureMinAcreage, int accurate)
{
    squares.clear();
    auto angle = [](cv::Point pt1, cv::Point pt2, cv::Point pt0) -> double
    {
        double dx1 = pt1.x - pt0.x;
        double dy1 = pt1.y - pt0.y;
        double dx2 = pt2.x - pt0.x;
        double dy2 = pt2.y - pt0.y;
        return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
    };
    vector<cv::Point> approx;
    for (size_t i = 0; i < contours.size(); i++)
    {
        // 寻找四边形模拟的关键函数
        cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * accurate * 0.01, true);
        if (approx.size() == 4 &&
            fabs(cv::contourArea(approx)) > sqaureMinAcreage &&
            cv::isContourConvex(approx))
        {
            squares.push_back(approx);
        }
        if ((approx.size() == 5) && fabs(cv::contourArea(approx)) > sqaureMinAcreage && cv::isContourConvex(approx))
        {
            // 因为算法判断的问题，有时会将四边形判断成五边形，所以我们将五边形中最近的两个顶点去掉其中一个，作为新的四边形
            double minLength = DBL_MAX;
            // 寻找最小长度
            for (int j = 0; j < approx.size(); j++)
            {
                cv::Point point1 = approx[j % approx.size()];
                cv::Point point2 = approx[(j + 1) % approx.size()];
                double length = std::pow((point1.x - point2.x), 2) + (std::pow((point1.y - point2.y), 2));
                if (length < minLength)
                    minLength = length;
            }
            // 根据最小长度去除其中的一个顶点
            for (auto j = approx.begin(); j != approx.end(); j++)
            {
                cv::Point point1 = *j;
                cv::Point point2;
                if (j + 1 == approx.end())
                    point2 = *(approx.begin());
                else
                    point2 = *(j + 1);
                double length = std::pow((point1.x - point2.x), 2) + (std::pow((point1.y - point2.y), 2));
                if (length == minLength)
                {
                    if (point1.y < point2.y)
                        approx.erase(j);
                    else
                    {
                        if (j + 1 == approx.end())
                            approx.erase(approx.begin());
                        else
                            approx.erase(j + 1);
                    }
                    break;
                }
            }
            // 将approx传到结果中
            squares.push_back(approx);
        }
    }
    // TODO::如果这里有一个调节项approxPolyDP这个作为循环中判断的重要因素
    // TODO::之后需要将连接四边形放到这里
}
void ImageTool::CalibrationTool::getSquareCornerLackOne(const vector<shared_ptr<Square>> &squaresInput, vector<cv::Point2f> &cornersOutput)
{
    // 当寻找的角点数少1时，寻找只连接了3个点的四边形，将其剩下的点作为新的角点，放入结果中
    for (auto i : squaresInput)
    {
        if (i->getLinkCount() == 3)
        {
            for (auto j : i->linkIndexMap)
            {
                // 找到false的点
                if (!j.second)
                {
                    cornersOutput.push_back(i->corners[j.first]);
                    j.second = true;
                }
            }
            break;
        }
    }
}
void ImageTool::CalibrationTool::getSquareCornerLackOne2X2(const vector<shared_ptr<Square>> &SquaresInput, vector<cv::Point2f> &cornersOutput)
{
    auto getDistance = [](cv::Point p1, cv::Point p2)
    {
        return sqrt(std::pow((p1.x - p2.x), 2) + (std::pow((p1.y - p2.y), 2)));
    };
    // 获取四个四边形的中点
    cv::Point centerOFSquares;
    int xC = 0;
    int yC = 0;
    for (int i = 0; i < SquaresInput.size(); i++)
    {
        xC += SquaresInput[i]->center.x / SquaresInput.size();
        yC += SquaresInput[i]->center.y / SquaresInput.size();
    }
    centerOFSquares.x = xC;
    centerOFSquares.y = yC;
    vector<cv::Point> result;
    for (auto i : SquaresInput)
    {
        double minCornerDisToCenter = DBL_MAX;
        int minIndex = -1;
        // 找到只连接了一个角点的四边形
        if (i->getLinkCount() == 1)
        {
            for (int j = 0; j < 4; j++)
            {
                // 获取角点到四个四边形中心的距离
                double distance = getDistance(i->corners[j], centerOFSquares);
                // 找到没有连接，并且距离中心点最近的角点
                if (distance < minCornerDisToCenter && !i->linkIndexMap[j])
                {
                    minIndex = j;
                    minCornerDisToCenter = distance;
                }
            }
            if (minIndex != -1)
                ;
            result.push_back(i->corners[minIndex]);
        }
    }
    cv::Point2f resultCorner;
    float x = 0;
    float y = 0;
    for (int i = 0; i < result.size(); i++)
    {
        x += result[i].x / result.size();
        y += result[i].y / result.size();
    }
    resultCorner.x = x;
    resultCorner.y = y;
    cornersOutput.push_back(resultCorner);
}
ImageTool::CalibrationTool::Square::Square(vector<cv::Point> CORNERS)
{
    auto increaseX = [](cv::Point p1, cv::Point p2)
    {
        return p1.x < p2.x;
    };
    auto increaseY = [](cv::Point p1, cv::Point p2)
    {
        return p1.y < p2.y;
    };

    corners = CORNERS;
    std::sort(corners.begin(), corners.end(), increaseY);
    std::sort(corners.begin(), corners.begin() + 2, increaseX);
    std::sort(corners.begin() + 2, corners.end(), increaseX);
    cv::Point side1 = corners[1] - corners[0];
    cv::Point side2 = corners[3] - corners[2];
    cv::Point side3 = corners[2] - corners[0];
    cv::Point side4 = corners[3] - corners[1];
    sides.push_back(side1);
    sides.push_back(side2);
    sides.push_back(side3);
    sides.push_back(side4);

    midLineVertical = (corners[3] + corners[2]) / 2 - (corners[1] + corners[0]) / 2;
    midLineHorizon = (corners[3] + corners[1]) / 2 - (corners[2] + corners[0]) / 2;

    int x = 0;
    int y = 0;
    for (auto i : corners)
    {
        x += i.x / 4;
        y += i.y / 4;
    }
    center.x = x;
    center.y = y;
    double minLength = DBL_MAX;
    // 寻找最小长度
    for (int j = 0; j < CORNERS.size(); j++)
    {
        cv::Point point1 = CORNERS[j % CORNERS.size()];
        cv::Point point2 = CORNERS[(j + 1) % CORNERS.size()];
        cv::Point side = cv::Point(point2.x - point1.x, point2.y - point2.y);
        sides.push_back(side);
        double length = std::sqrt(std::pow((point1.x - point2.x), 2) + (std::pow((point1.y - point2.y), 2)));
        if (length < minLength)
            minLength = length;
    }
    // 2这个系数是为了减小腐蚀对最小的边的影响
    // 在Automatic Detection of Checkerboards on Blurred and Distorted Images论文中使用的是2*erosion，但我们只进行了一次腐蚀操作，所以使用的是2
    minSideLength = minLength + 4;
    linkIndexMap[0] = false;
    linkIndexMap[1] = false;
    linkIndexMap[2] = false;
    linkIndexMap[3] = false;
}