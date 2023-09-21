#include "Image.h"
#include "BirdView.h"
#include "ImageTool.h"
#include "CalibrationTool.h"
#include "MaskTool.h"
#include "Camera.h"
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
shared_ptr<CarPositionPARA> Image::para = std::make_shared<CarPositionPARA>();
unsigned int Image::imageWidth=720;
unsigned int Image::imageHeight=480;
using namespace ImageTool;
// 普陀山四目，12点标定布排序方法

void Image::addCamera(Camera camera)
{
    this->camera = std::make_shared<Camera>(camera);
}
void Image::addCamera(shared_ptr<Camera> camera)
{
    this->camera = camera;
}
void Image::initCarPositionPARA(std::string xmlPath)
{
    int widthScale=2000;
    int heightScale=2000;
    para->initalWorldPoint = cv::Point2f(-2600 - widthScale, 4450 + widthScale);
    para->carModelInitPoint = cv::Point2f(-1200 + 200 + 200, 2850 - 400);
    para->carWidth = 2400 - 800;
    para->carHeight = 5700 - 800;
    para->worldWidth = 5200 + widthScale*2;
    para->worldHeight = 8700 + widthScale*2;
    para->spliceSize = 500;
}
void Image::sortCorner(vector<cv::Point2f> &outputCorner, int groupSize)
{
    auto increaseX = [](cv::Point2f p1, cv::Point2f p2)
    {
        return p1.x < p2.x;
    };
    auto decreaseY = [](cv::Point2f p1, cv::Point2f p2)
    {
        return p1.y > p2.y;
    };

    std::sort(outputCorner.begin(), outputCorner.end(), decreaseY);
    int currentIndex = 0;
    // 思路：现根据y进行一次分组，然后再根据x进行排序，最后将所有组别混合在一起
    // 组
    vector<vector<cv::Point2f>> group;
    for (int i = 0; i < outputCorner.size() / groupSize; i++)
    {
        vector<cv::Point2f> tmpGroup;
        for (int j = 0; j < groupSize; j++)
        {
            int index = i * groupSize + j;
            tmpGroup.push_back(outputCorner[index]);
        }
        group.push_back(tmpGroup);
    }
    outputCorner.clear();
    for (auto i : group)
    {
        std::sort(i.begin(), i.end(), increaseX);
        outputCorner.insert(outputCorner.end(), i.begin(), i.end());
    }
}
// 普陀山四目标定，12点的世界坐标位置获取
void Image::getWorldObject(vector<cv::Point> &outputCorner, cv::Point initPoint, int distance, cv::Size patternSize, unsigned int imageType)
{
    outputCorner.clear();
    if (imageType == IMAGE_FRONT)
    {
        // 获取左右的邻域点
        auto getNeighbourhoodX = [](cv::Point pointCenter, int distance)
        {
            cv::Point pointLeft = pointCenter - cv::Point(distance, 0);
            cv::Point pointRight = pointCenter + cv::Point(distance, 0);
            return vector<cv::Point>{pointLeft, pointCenter, pointRight};
        };
        int height = patternSize.height;
        for (int i = 0; i < height; i++)
        {
            cv::Point currentPoint = initPoint + cv::Point(0, i * distance);
            vector<cv::Point> tmpGroup = getNeighbourhoodX(currentPoint, distance);
            outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
        }
    }
    // 获取后相机的世界坐标
    else if (imageType == IMAGE_REAR)
    {
        // 获取左右的邻域点
        auto getNeighbourhoodX = [](cv::Point pointCenter, int distance)
        {
            cv::Point pointLeft = pointCenter + cv::Point(distance, 0);
            cv::Point pointRight = pointCenter - cv::Point(distance, 0);
            return vector<cv::Point>{pointLeft, pointCenter, pointRight};
        };
        int height = patternSize.height;
        for (int i = 0; i < height; i++)
        {
            cv::Point currentPoint = initPoint - cv::Point(0, i * distance);
            vector<cv::Point> tmpGroup = getNeighbourhoodX(currentPoint, distance);
            outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
        }
    }
    // 获取右相机的世界坐标系坐标
    else if (imageType == IMAGE_RIGHT)
    {
        // 获取左右的邻域点
        auto getNeighbourhoodY = [](cv::Point pointCenter, int distance)
        {
            cv::Point pointLeft = pointCenter + cv::Point(0, distance);
            cv::Point pointRight = pointCenter - cv::Point(0, distance);
            return vector<cv::Point>{pointLeft, pointCenter, pointRight};
        };
        int height = patternSize.height;
        for (int i = 0; i < height; i++)
        {
            cv::Point currentPoint = initPoint + cv::Point(i * distance, 0);
            vector<cv::Point> tmpGroup = getNeighbourhoodY(currentPoint, distance);
            outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
        }
    }
    // 获取左相机的世界坐标系坐标
    else if (imageType == IMAGE_LEFT)
    {
        // 获取左右的邻域点
        auto getNeighbourhoodY = [](cv::Point pointCenter, int distance)
        {
            cv::Point pointLeft = pointCenter - cv::Point(0, distance);
            cv::Point pointRight = pointCenter + cv::Point(0, distance);
            return vector<cv::Point>{pointLeft, pointCenter, pointRight};
        };
        int height = patternSize.height;
        for (int i = 0; i < height; i++)
        {
            cv::Point currentPoint = initPoint - cv::Point(i * distance, 0);
            vector<cv::Point> tmpGroup = getNeighbourhoodY(currentPoint, distance);
            outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
        }
    }
}
void Image::autoCalibration(shared_ptr<cv::Mat> src, string storePath)
{
    /*
    重要函数
    实现思路为：
    1.先寻找34布的角点，计算一次内参
    2.根据34布的角点先生成一次鸟瞰图
    3.在对鸟瞰图去找22的角点，然后反映射回其在图像中的角点，然后再重新计算一次内参
    */
    cv::Point initPos;
    float distance = 300;
    cv::Size patternSize=cv::Size(3,4);
    string fileName;
    if (imageType == IMAGE_FRONT)
    {
        fileName = "front";
        initPos = cv::Point(0, 3150);
    }
    else if (imageType == IMAGE_RIGHT)
    {
        fileName = "right";
        initPos = cv::Point(1350, 0);
    }
    else if (imageType == IMAGE_REAR)
    {
        fileName = "rear";
        initPos = cv::Point(0, -3150);
    }
    else if (imageType == IMAGE_LEFT)
    {
        fileName = "left";
        initPos = cv::Point(-1350, 0);
    }
    shared_ptr<cv::Mat> undistorImage = std::make_shared<cv::Mat>();
    cv::Mat newIntrinsic = cv::Mat::zeros(3, 3, CV_32F);
    camera->intrinsic.copyTo(newIntrinsic);
    newIntrinsic.at<float>(0, 0);
    newIntrinsic.at<float>(1, 1);
    shared_ptr<cv::Mat> cvImageData = std::make_shared<cv::Mat>();
    cv::flip(*src, *cvImageData, 0);

    vector<cv::Point2f> cornerPixel;
    vector<cv::Point> cornerWorld;
    shared_ptr<cv::Mat> gray = getCvGray(src);

    vector<cv::Point2f> cornerPixel2;
    vector<cv::Point2f> cornerPixel3;

    bool isFind = CalibrationTool::findCorner(src, cornerPixel, patternSize);
    sortCorner(cornerPixel, patternSize.width);
    getWorldObject(cornerWorld, initPos, distance, patternSize, imageType);
    if (isFind)
        cornerSubPix(*gray, cornerPixel, cv::Size(4, 4), cv::Size(5, 5),
                     cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));

    sortCorner(cornerPixel, patternSize.width);

// 寻找之后的20个角点
#if IS_20
    bool isFind2 = CalibrationTool::findCorner(src, cornerPixel2, cv::Size(2, 2));
    shared_ptr<cv::Mat> tmpSrc;
    bool isFind3 = false;
    if (isFind2)
    {
        tmpSrc = MaskTool::putCornerMask(src, cornerPixel2);
        isFind3 = CalibrationTool::findCorner(tmpSrc, cornerPixel3, cv::Size(2, 2));
    }
    sortCorner(cornerPixel2, 2);
    sortCorner(cornerPixel3, 2);
    if (isFind2)
        cornerSubPix(*gray, cornerPixel2, cv::Size(4, 4), cv::Size(5, 5),
                     cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));
    if (isFind3)
        cornerSubPix(*gray, cornerPixel3, cv::Size(4, 4), cv::Size(5, 5),
                     cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));
    if (imageType == IMAGE_FRONT)
    {

        // // show circle
        // cv::Mat tmpShow;
        // src->copyTo(tmpShow);
        // for (auto l : cornerPixel2)
        // {
        //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // }
        // for (auto l : cornerPixel3)
        // {
        //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // }
        // string windowName2 = "findSquare0 ";
        // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
        // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
        // cv::imshow(windowName2, tmpShow);

        // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
        // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());

        // cornerWorld.push_back(cv::Point(1800, 3450));
        // cornerWorld.push_back(cv::Point(2200, 3450));
        // cornerWorld.push_back(cv::Point(1800, 3850));
        // cornerWorld.push_back(cv::Point(2200, 3850));

        // cornerWorld.push_back(cv::Point(-2200, 3450));
        // cornerWorld.push_back(cv::Point(-1800, 3450));
        // cornerWorld.push_back(cv::Point(-2200, 3850));
        // cornerWorld.push_back(cv::Point(-1800, 3850));
    }
    else if (imageType == IMAGE_RIGHT)
    {
        // // show circle
        // cv::Mat tmpShow;
        // src->copyTo(tmpShow);
        // for (auto l : cornerPixel2)
        // {
        //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // }
        // // for (auto l : cornerPixel3)
        // // {
        // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // // }
        // string windowName2 = "findSquare1 ";
        // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
        // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
        // cv::imshow(windowName2, tmpShow);

        // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
        // // // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());
        // cornerPixel.push_back(cv::Point(626,253));
        // cornerPixel.push_back(cv::Point(632,253));
        // cornerPixel.push_back(cv::Point(611,240));
        // cornerPixel.push_back(cv::Point(618,239));

        // //实车2右标定布，大块角点
        // cornerPixel.push_back(cv::Point(634,279));
        // cornerPixel.push_back(cv::Point(650,267));
        // cornerPixel.push_back(cv::Point(576,222));
        // cornerPixel.push_back(cv::Point(607,223));

        // //实车4右标定布，大块角点

        // cornerWorld.push_back(cv::Point(1800, 3850));
        // cornerWorld.push_back(cv::Point(1800, 3450));
        // cornerWorld.push_back(cv::Point(2200, 3850));
        // cornerWorld.push_back(cv::Point(2200, 3450));

        // // cornerWorld.push_back(cv::Point(1800, -3450));
        // // cornerWorld.push_back(cv::Point(1800, -3850));
        // // cornerWorld.push_back(cv::Point(2200, -3450));
        // // cornerWorld.push_back(cv::Point(2200, -3850));

        // cornerWorld.push_back(cv::Point(1800-600, -3450+600));
        // cornerWorld.push_back(cv::Point(1800-600, -3850-600));
        // cornerWorld.push_back(cv::Point(2200+400, -3450+600));
        // cornerWorld.push_back(cv::Point(2200+400, -3850-600));
    }
    else if (imageType == IMAGE_LEFT)
    {
        // // show circle
        // cv::Mat tmpShow;
        // src->copyTo(tmpShow);
        // for (auto l : cornerPixel2)
        // {
        //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // }
        // // // for (auto l : cornerPixel3)
        // // // {
        // // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // // // }
        // string windowName2 = "findSquare2 ";
        // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
        // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
        // cv::imshow(windowName2, tmpShow);

        // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
        // // // // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());

        // // cornerPixel.push_back(cv::Point(88,250));
        // // cornerPixel.push_back(cv::Point(95,250));
        // // cornerPixel.push_back(cv::Point(103,236));
        // // cornerPixel.push_back(cv::Point(110,239));

        //  //实车2的大标定布坐标
        // // cornerPixel.push_back(cv::Point(63, 268));
        // // cornerPixel.push_back(cv::Point(79, 278));
        // // cornerPixel.push_back(cv::Point(106, 226));
        // // cornerPixel.push_back(cv::Point(137, 224));

        // //实车4小标定坐标
        // cornerPixel.push_back(cv::Point(88, 243));
        // cornerPixel.push_back(cv::Point(94, 244));
        // cornerPixel.push_back(cv::Point(104, 230));
        // cornerPixel.push_back(cv::Point(110, 231));

        // // 实车4的大标定布坐标
        // cornerPixel.push_back(cv::Point(61, 259));
        // cornerPixel.push_back(cv::Point(77, 271));
        // cornerPixel.push_back(cv::Point(109, 217));
        // cornerPixel.push_back(cv::Point(140, 217));

        // // // // 左上角四个标定布的角点
        // cornerWorld.push_back(cv::Point(-1800, 3450));
        // cornerWorld.push_back(cv::Point(-1800, 3850));
        // cornerWorld.push_back(cv::Point(-2200, 3450));
        // cornerWorld.push_back(cv::Point(-2200, 3850));
        // // // // // //左下角四个标定布的角点
        // cornerWorld.push_back(cv::Point(-1800, -3850));
        // cornerWorld.push_back(cv::Point(-1800, -3450));
        // cornerWorld.push_back(cv::Point(-2200, -3850));
        // cornerWorld.push_back(cv::Point(-2200, -3450));

        // // // // //左下角大标定布的四个角
        // cornerWorld.push_back(cv::Point(-1800+600, -3850-600));
        // cornerWorld.push_back(cv::Point(-1800+600, -3450+600));
        // cornerWorld.push_back(cv::Point(-2200-400, -3850-600));
        // cornerWorld.push_back(cv::Point(-2200-400, -3450+600));
    }
    else if (imageType == IMAGE_REAR)
    {
        // // show circle
        // cv::Mat tmpShow;
        // src->copyTo(tmpShow);
        // for (auto l : cornerPixel2)
        // {
        //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // }
        // // for (auto l : cornerPixel3)
        // // {
        // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
        // // }
        // string windowName2 = "findSquare3 ";
        // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
        // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
        // cv::imshow(windowName2, tmpShow);

        // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());

        // cornerWorld.push_back(cv::Point(2200, -3450));
        // cornerWorld.push_back(cv::Point(1800, -3450));
        // cornerWorld.push_back(cv::Point(2200, -3850));
        // cornerWorld.push_back(cv::Point(1800, -3850));

        // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());
        // cornerWorld.push_back(cv::Point(-1800, -3450));
        // cornerWorld.push_back(cv::Point(-2200, -3450));
        // cornerWorld.push_back(cv::Point(-1800, -3850));
        // cornerWorld.push_back(cv::Point(-2200, -3850));
    }

#endif

    camera->initCameraExtrinsic(cornerPixel, cornerWorld);

    shared_ptr<cv::Mat> uv = generateMap2D(para, camera->intrinsic, camera->distortCoeff, camera->rvec, camera->tvec, imageWidth, imageHeight, imageType);
    cv::Mat BIRD(uv->rows, uv->cols, CV_8UC3);
    cv::Mat uvDecimal(uv->rows, uv->cols, CV_8UC4);
    cv::Mat uvInteger(uv->rows, uv->cols, CV_8UC4);
    // 这里的uv和cvImageData都是翻转过的
    for (int i = 0; i < uv->rows; i++)
    {
        for (int j = 0; j < uv->cols; j++)
        {

            // 生成的源
            cv::Vec4f index = uv->at<cv::Vec4f>(i, j);
            int x = index[2];
            int y = index[1];
            int alpha = index[3];
            // 生成的小数源
            int minx = (float)(index[2] - x) * 255.0f;
            int miny = (float)(index[1] - y) * 255.0f;

            uvInteger.at<cv::Vec4b>(i, j)[0] = 0;
            uvInteger.at<cv::Vec4b>(i, j)[1] = y;
            uvInteger.at<cv::Vec4b>(i, j)[2] = x;
            uvInteger.at<cv::Vec4b>(i, j)[3] = alpha;

            uvDecimal.at<cv::Vec4b>(i, j)[0] = 0;
            uvDecimal.at<cv::Vec4b>(i, j)[1] = miny;
            uvDecimal.at<cv::Vec4b>(i, j)[2] = minx;
            uvDecimal.at<cv::Vec4b>(i, j)[3] = alpha;

            float rx = x + (float)minx / 255;
            float ry = y + (float)miny / 255;

            int xIndex = rx / 255 * imageWidth;
            int yIndex = ry / 255 * imageHeight;
            if (imageType == IMAGE_LEFT || imageType == IMAGE_RIGHT)
            {
                // 限制区域
                if (j > 150 && imageType == IMAGE_LEFT)
                {
                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                }
                else if (j < 150 && imageType == IMAGE_RIGHT)
                {
                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                }
                else
                {
                    if (i > 180 && i < 320)
                    {
                        BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                        BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                        BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                    }

                    else
                    {
                        if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
                        {
                            int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
                            int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
                            int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

                            BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
                            BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
                            BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
                        }
                        else
                        {
                            BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                            BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                            BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                        }
                    }
                }
            }
            else
            {
                if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
                {
                    int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
                    int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
                    int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
                }
                else
                {
                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                }
            }
        }
    }
    // 重新寻找角点
#if 1
    if (imageType == IMAGE_LEFT || imageType == IMAGE_RIGHT)
    {
        vector<cv::Point2f> reCorner1;
        vector<cv::Point2f> reCorner2;
        bool isFind1=CalibrationTool::findCorner(getCvShared(BIRD), reCorner1, cv::Size(2, 2));
        shared_ptr<cv::Mat> BIRD_Mask = MaskTool::putCornerMask(getCvShared(BIRD), reCorner1);
        bool isFind2=CalibrationTool::findCorner(BIRD_Mask, reCorner2, cv::Size(2, 2));
        vector<cv::Point2f> reCornerFront;
        vector<cv::Point2f> reCornerBehind;

        vector<cv::Point2f> cornerResultFront;
        vector<cv::Point2f> cornerResultBehind;
        if (isFind1 && isFind2)
        {
            if (reCorner1[0].y < reCorner2[0].y)
            {
                reCornerFront = reCorner1;
                reCornerBehind = reCorner2;
            }
            else
            {
                reCornerFront = reCorner2;
                reCornerBehind = reCorner1;
            }
            for (auto i : reCornerFront)
            {
                int m = i.y;
                int n = i.x;
                // 生成的源
                cv::Vec4f index = uv->at<cv::Vec4f>(m, n);
                int x = index[2];
                int y = index[1];
                int alpha = index[3];
                // 生成的小数源
                int minx = (float)(index[2] - x) * 255.0f;
                int miny = (float)(index[1] - y) * 255.0f;
                float rx = x + (float)minx / 255;
                float ry = y + (float)miny / 255;
                int xIndex = rx / 255 * imageWidth;
                int yIndex = ry / 255 * imageHeight;
                cornerResultFront.push_back(cv::Point2f(xIndex, imageHeight - yIndex));
            }

            for (auto i : reCornerBehind)
            {
                int m = i.y;
                int n = i.x;
                // 生成的源
                cv::Vec4f index = uv->at<cv::Vec4f>(m, n);
                int x = index[2];
                int y = index[1];
                int alpha = index[3];
                // 生成的小数源
                int minx = (float)(index[2] - x) * 255.0f;
                int miny = (float)(index[1] - y) * 255.0f;
                float rx = x + (float)minx / 255;
                float ry = y + (float)miny / 255;
                int xIndex = rx / 255 * imageWidth;
                int yIndex = ry / 255 * imageHeight;
                cornerResultBehind.push_back(cv::Point2f(xIndex, imageHeight - yIndex));
            }
            sortCorner(cornerResultFront, 2);
            sortCorner(cornerResultBehind, 2);
            if (imageType == IMAGE_LEFT)
            {
                cornerPixel.insert(cornerPixel.end(), cornerResultFront.begin(), cornerResultFront.end());
                cornerPixel.insert(cornerPixel.end(), cornerResultBehind.begin(), cornerResultBehind.end());

                // 左上角四个标定布的角点
                cornerWorld.push_back(cv::Point(-1800, 3450));
                cornerWorld.push_back(cv::Point(-1800, 3850));
                cornerWorld.push_back(cv::Point(-2200, 3450));
                cornerWorld.push_back(cv::Point(-2200, 3850));
                // 左下角四个标定布的角点
                cornerWorld.push_back(cv::Point(-1800, -3850));
                cornerWorld.push_back(cv::Point(-1800, -3450));
                cornerWorld.push_back(cv::Point(-2200, -3850));
                cornerWorld.push_back(cv::Point(-2200, -3450));
            }
            else if (imageType == IMAGE_RIGHT)
            {
                // drawPoints(getCvShared(BIRD), reCornerFront, "birdViewPoint3");
                // drawPoints(getCvShared(BIRD), reCornerBehind, "birdViewPoint4");
                cornerPixel.insert(cornerPixel.end(), cornerResultFront.begin(), cornerResultFront.end());
                cornerPixel.insert(cornerPixel.end(), cornerResultBehind.begin(), cornerResultBehind.end());

                // 右上角
                cornerWorld.push_back(cv::Point(1800, 3850));
                cornerWorld.push_back(cv::Point(1800, 3450));
                cornerWorld.push_back(cv::Point(2200, 3850));
                cornerWorld.push_back(cv::Point(2200, 3450));

                // 右下角
                cornerWorld.push_back(cv::Point(1800, -3450));
                cornerWorld.push_back(cv::Point(1800, -3850));
                cornerWorld.push_back(cv::Point(2200, -3450));
                cornerWorld.push_back(cv::Point(2200, -3850));
            }
            // 如果左右两边的点都找到了，则更新外参，否则不更新

            camera->initCameraExtrinsic(cornerPixel, cornerWorld);
        }
        uv = generateMap2D(para, camera->intrinsic, camera->distortCoeff, camera->rvec, camera->tvec, imageWidth, imageHeight, imageType);
        // 这里的uv和cvImageData都是翻转过的
        for (int i = 0; i < uv->rows; i++)
        {
            for (int j = 0; j < uv->cols; j++)
            {
                // 生成的源
                cv::Vec4f index = uv->at<cv::Vec4f>(i, j);
                int x = index[2];
                int y = index[1];
                int alpha = index[3];
                // 生成的小数源
                int minx = (float)(index[2] - x) * 255.0f;
                int miny = (float)(index[1] - y) * 255.0f;
                uvInteger.at<cv::Vec4b>(i, j)[0] = 0;
                uvInteger.at<cv::Vec4b>(i, j)[1] = y;
                uvInteger.at<cv::Vec4b>(i, j)[2] = x;
                uvInteger.at<cv::Vec4b>(i, j)[3] = alpha;
                uvDecimal.at<cv::Vec4b>(i, j)[0] = 0;
                uvDecimal.at<cv::Vec4b>(i, j)[1] = miny;
                uvDecimal.at<cv::Vec4b>(i, j)[2] = minx;
                uvDecimal.at<cv::Vec4b>(i, j)[3] = alpha;
                float rx = x + (float)minx / 255;
                float ry = y + (float)miny / 255;
                int xIndex = rx / 255 * imageWidth;
                int yIndex = ry / 255 * imageHeight;
                if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
                {
                    int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
                    int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
                    int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
                }
                else
                {
                    BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                    BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
                }
            }
        }
    }
#endif
    cv::imshow(fileName,BIRD);
    cv::imwrite(storePath + fileName + ".png", BIRD);
    cv::imwrite(storePath + fileName + "_uv.png", uvInteger);
    cv::imwrite(storePath + fileName + "_uv_decimal.png", uvDecimal);
}

void Image::generateUVTexture_2D(const shared_ptr<cv::Mat> &uv, const shared_ptr<cv::Mat> &cvImageData,shared_ptr<cv::Mat> &uvInteger,
                          shared_ptr<cv::Mat> &uvDecimal, shared_ptr<cv::Mat> &BIRD)
{

    // 这里的uv和cvImageData都是翻转过的
    for (int i = 0; i < uv->rows; i++)
    {
        for (int j = 0; j < uv->cols; j++)
        {
            // 生成的源
            cv::Vec4f index = uv->at<cv::Vec4f>(i, j);
            int x = index[2];
            int y = index[1];
            int alpha = index[3];
            // 生成的小数源
            int minx = (float)(index[2] - x) * 255.0f;
            int miny = (float)(index[1] - y) * 255.0f;
            uvInteger->at<cv::Vec4b>(i, j)[0] = 0;
            uvInteger->at<cv::Vec4b>(i, j)[1] = y;
            uvInteger->at<cv::Vec4b>(i, j)[2] = x;
            uvInteger->at<cv::Vec4b>(i, j)[3] = alpha;
            uvDecimal->at<cv::Vec4b>(i, j)[0] = 0;
            uvDecimal->at<cv::Vec4b>(i, j)[1] = miny;
            uvDecimal->at<cv::Vec4b>(i, j)[2] = minx;
            uvDecimal->at<cv::Vec4b>(i, j)[3] = alpha;
            float rx = x + (float)minx / 255;
            float ry = y + (float)miny / 255;
            int xIndex = rx / 255 * imageWidth;
            int yIndex = ry / 255 * imageHeight;
            if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
            {
                int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
                int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
                int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

                BIRD->at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
                BIRD->at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
                BIRD->at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
            }
            else
            {
                BIRD->at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
                BIRD->at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
                BIRD->at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
            }
        }
    }
}