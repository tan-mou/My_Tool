#include "BirdView.h"
#include <iostream>
#include <opencv2/calib3d.hpp>
shared_ptr<cv::Mat> BirdView::generateMap2D(shared_ptr<CarPositionPARA> para,
cv::Mat intrinsic,cv::Mat distortCoeff,cv::Mat rvec,cv::Mat tvec,
unsigned int imageWidth,unsigned int imageHeight,unsigned int type)
{

#if IS_OUTPUT_UV_WORLD
    string objectFilePath ="/set/output/object/world.txt";
    std::ofstream OutFile1(objectFilePath.c_str());
    string uvFilePath ="/set/output/uv/uv.txt";
    std::ofstream OutFile2(uvFilePath.c_str());
#endif
    vector<cv::Point3f> objectPos;
    unsigned int mapHeight = imageHeight;
    unsigned int mapWidth = imageWidth / 2;
    // 这里的坐标系是世界坐标系
    cv::Point2f initalWorldPoint = para->initalWorldPoint;
    cv::Point2f carModelInitPoint = para->carModelInitPoint;
    float carWidth = para->carWidth;
    float carHeight = para->carHeight;
    float worldHeight = para->worldHeight;
    float worldWidth = para->worldWidth;
    float spliceSize = para->spliceSize;


    // std::cout << "rvec2:" << rvec << std::endl;
    float carMinX = carModelInitPoint.x;
    float carMaxX = carModelInitPoint.x + carWidth;
    float carMaxY = carModelInitPoint.y;
    float carMinY = carModelInitPoint.y - carHeight;

    float initMinX = initalWorldPoint.x;
    float initMaxX = initalWorldPoint.x + worldWidth;
    float initMaxY = initalWorldPoint.y;
    float initMinY = initalWorldPoint.y - worldHeight;
    for (int i = 0; i < mapHeight; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            float x = initalWorldPoint.x + j * worldWidth / mapWidth;
            float y = initalWorldPoint.y - i * worldHeight / mapHeight;
#if IS_OUTPUT_UV_WORLD
            OutFile1 << x << "\t" << y << "\n";
#endif
            cv::Point3f currentWorld(x, y, 0);
            objectPos.push_back(currentWorld);
        }
    }

    vector<cv::Point2f> projectPoint;
    cv::fisheye::projectPoints(objectPos, projectPoint, rvec, tvec, intrinsic, distortCoeff);

#if IS_OUTPUT_UV_WORLD

    if (!OutFile2.is_open())
    {
        std::cout << "ERROR::Image::x output file cant open" << std::endl;
    }
    for (auto &i : projectPoint)
    {
        OutFile2 << i.x << "\t" << i.y << "\n";
    }
    OutFile1.close();
    OutFile2.close();
#endif
    // caculate uv
    // the rule of uv is opengl ruler not opencv ruler
    // 因为对数据图片已经进行转置统一了，所以按照OpenGL的来

    shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>(mapHeight, mapWidth, CV_32FC4));
    auto normalize = [](cv::Point2f &point)
    {
        float length = sqrt(pow(point.x, 2) + pow(point.y, 2));
        point.x /= length;
        point.y /= length;
    };
    auto cross = [](cv::Point2f p1, cv::Point2f p2)
    {
        cv::Point3i P1(p1.x, p1.y, 0);
        cv::Point3i P2(p2.x, p2.y, 0);
        return (P1.cross(P2)).z;
    };
    auto getLength = [](cv::Point2f P)
    {
        return sqrt(P.x * P.x + P.y * P.y);
    };
    // L2是直线的方向，L1是直线上一点到P点的方向
    auto getPointLineDistance = [=](cv::Point2f L1, cv::Point2f L2)
    {
        normalize(L2);
        float distanceProject = L1.dot(L2);
        float distance = sqrt(pow(getLength(L1), 2) - pow(distanceProject, 2));
        return distance;
    };
    // 生成四个角的透明度方法
    // ABCD是逆时针的四个范围点
    // isAhead表示是凭借区域是上图还是下图，isUpper表示是上半部分的图，还是下半部的图
    auto genateBorderAlpha = [=](cv::Point2f A, cv::Point2f B, cv::Point2f C, cv::Point2f D, cv::Point2f O, bool isAhead)
    {
        cv::Point2f directionBC = C - B;
        cv::Point2f directionAD = D - A;
        normalize(directionBC);
        normalize(directionAD);

        float z1 = cross(O - B, C - B);
        float z2 = cross(O - A, D - A);
        // 如果在拼接区内，根据点到AD，BC的距离去判断透明度
        if (z1 <= 0 && z2 >= 0)
        {
            // O到AD的距离
            float distanceAD = getPointLineDistance(O - A, directionAD);
            // O到BC的距离
            float distanceBC = getPointLineDistance(O - B, directionBC);
            // 离bc越远，后的alpha越大
            float alphaBehind = distanceBC / (distanceAD + distanceBC);
            // 离ad越远，前的alpha越大
            float alphaAhead = distanceAD / (distanceAD + distanceBC);
            if (isAhead)
            {
                return alphaAhead * 255.0f;
            }
            else
            {
                return alphaBehind * 255.0f;
            }
        }
        // 上半部分，下显示0
        else if (z1 >= 0 && !isAhead)
        {
            return 0.0f;
        }
        // 下半部分，上显示0
        else if (z2 <= 0 && isAhead)
        {
            return 0.0f;
        }
        else
            return 255.0f;
    };
    for (int i = 0; i < mapHeight; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            float x = initalWorldPoint.x + j * worldWidth / mapWidth;
            float y = initalWorldPoint.y - i * worldHeight / mapHeight;
            // if(x!=0)
            //     continue;
            int index = i * mapWidth + j;
            //归一化投影点的坐标
            projectPoint[index].x /= imageWidth;
            //y轴要转到OpenGL坐标系
            projectPoint[index].y = 1 - projectPoint[index].y / imageHeight;

            //放大到0-255中，以存储
            projectPoint[index].x *= 255.0f;
            projectPoint[index].y *=255.0f;
            // 当前点
            cv::Point2f O = cv::Point2f(x, y); // current Point
            // 如果在矩形区域内，直接将map置空
            if (x > carMinX && x < carMaxX && y > carMinY && y < carMaxY)
            {
                result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 0);
            }
            else
            {
                if (type == MAP_FRONT_AREA)
                {
                    if (y <= carMaxY)
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 0);
                    }
                    else
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 255);
                    }
                    // 限制在左上角的区域
                    if (O.x <= carMinX && O.y >= carMaxY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = carMinX - spliceSize;
                        A.y = carMaxY;
                        B.x = carMinX;
                        B.y = carMaxY + spliceSize;

                        C.x = initMinX + spliceSize;
                        C.y = initMaxY;
                        D.x = initMinX;
                        D.y = initMaxY - spliceSize;
                        float alpha = genateBorderAlpha(A, B, C, D, O, true);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                    // 限制在右上角的区域
                    else if (O.x >= carMaxX && O.y >=carMaxY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = initMaxX;
                        A.y = initMaxY - spliceSize;
                        B.x = initMaxX - spliceSize;
                        B.y = initMaxY;

                        C.x = carMaxX;
                        C.y = carMaxY + spliceSize;
                        D.x = carMaxX + spliceSize;
                        D.y = carMaxY;
                        float alpha = genateBorderAlpha(A, B, C, D, O, true);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                }
                else if (type == MAP_REAR_AREA)
                {
                    if (y >= carMinY)
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 0);
                    }
                    else
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 255);
                    }
                    // 限制在左下角的区域
                    if (O.x <= carMinX && O.y <= carMinY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = carMinX;
                        A.y = carMinY - spliceSize;
                        B.x = carMinX - spliceSize;
                        B.y = carMinY;

                        C.x = initMinX;
                        C.y = initMinY + spliceSize;
                        D.x = initMinX + spliceSize;
                        D.y = initMinY;
                        float alpha = genateBorderAlpha(A, B, C, D, O, false);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                    // 限制在右下角的区域
                    else if (O.x >= carMaxX && O.y <= carMinY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = initMaxX - spliceSize;
                        A.y = initMinY;
                        B.x = initMaxX;
                        B.y = initMinY - spliceSize;

                        C.x = carMaxX + spliceSize;
                        C.y = carMinY;
                        D.x = carMaxX;
                        D.y = carMinY - spliceSize;
                        float alpha = genateBorderAlpha(A, B, C, D, O, false);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                }
                else if (type == MAP_LEFT_AREA)
                {
                    if (x >= carMinX)
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 0);
                    }
                    else
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 255);
                    }
                    // 限制在左上角的区域
                    if (O.x <= carMinX && O.y >=carMaxY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = carMinX - spliceSize;
                        A.y = carMaxY;
                        B.x = carMinX;
                        B.y = carMaxY + spliceSize;

                        C.x = initMinX + spliceSize;
                        C.y = initMaxY;
                        D.x = initMinX;
                        D.y = initMaxY - spliceSize;
                        float alpha = genateBorderAlpha(A, B, C, D, O, false);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                    // 限制在左下角的区域
                    if (O.x <= carMinX && O.y <= carMinY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = carMinX;
                        A.y = carMinY - spliceSize;
                        B.x = carMinX - spliceSize;
                        B.y = carMinY;

                        C.x = initMinX;
                        C.y = initMinY + spliceSize;
                        D.x = initMinX + spliceSize;
                        D.y = initMinY;
                        float alpha = genateBorderAlpha(A, B, C, D, O, true);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                }
                else if (type == MAP_RIGHT_AREA)
                {
                    if (x <= carMaxX)
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 0);
                    }
                    else
                    {
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , 255);
                    }
                    // 限制在右上角的区域
                    if (O.x >= carMaxX && O.y >= carMaxY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = initMaxX;
                        A.y = initMaxY - spliceSize;
                        B.x = initMaxX - spliceSize;
                        B.y = initMaxY;

                        C.x = carMaxX;
                        C.y = carMaxY + spliceSize;
                        D.x = carMaxX + spliceSize;
                        D.y = carMaxY;
                        float alpha = genateBorderAlpha(A, B, C, D, O, false);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                    // 限制在右下角的区域
                    if (O.x >= carMaxX && O.y <= carMinY)
                    {
                        cv::Point2f A, B, C, D;
                        // ABCD 为拼接区域的四个点
                        A.x = initMaxX - spliceSize;
                        A.y = initMinY;
                        B.x = initMaxX;
                        B.y = initMinY - spliceSize;

                        C.x = carMaxX + spliceSize;
                        C.y = carMinY;
                        D.x = carMaxX;
                        D.y = carMinY - spliceSize;
                        float alpha = genateBorderAlpha(A, B, C, D, O, true);
                        result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y , projectPoint[index].x , alpha);
                    }
                }
                else
                {
                    std::cout<<"ERROR IN generateMap Function ::type Error"<<std::endl;
                    return nullptr;
                }
            }
        }
    }
    return result;
}

vector<shared_ptr<cv::Mat>> BirdView::generateMap3D(shared_ptr<CarPositionPARA> para,
                                                    cv::Mat intrinsic, cv::Mat distortCoeff, cv::Mat rvec, cv::Mat tvec,
                                                    unsigned int imageWidth, unsigned int imageHeight, unsigned int type)
{

    vector<cv::Point3f> objectPos;
    unsigned int mapHeight = imageHeight;
    unsigned int mapWidth = imageWidth / 2;
    // 这里的坐标系是世界坐标系
    cv::Point2f initalWorldPoint = para->initalWorldPoint;
    cv::Point2f carModelInitPoint = para->carModelInitPoint;
    float carWidth = para->carWidth;
    float carHeight = para->carHeight;
    float worldHeight = para->worldHeight;
    float worldWidth = para->worldWidth;
    float spliceSize = para->spliceSize;

    // std::cout << "rvec2:" << rvec << std::endl;
    float carMinX = carModelInitPoint.x;
    float carMaxX = carModelInitPoint.x + carWidth;
    float carMaxY = carModelInitPoint.y;
    float carMinY = carModelInitPoint.y - carHeight;

    float initMinX = initalWorldPoint.x;
    float initMaxX = initalWorldPoint.x + worldWidth;
    float initMaxY = initalWorldPoint.y;
    float initMinY = initalWorldPoint.y - worldHeight;

    return vector<shared_ptr<cv::Mat>>{nullptr};


}