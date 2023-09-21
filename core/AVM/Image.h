#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <string>
#include <vector>
#include "BirdView.h"
using std::shared_ptr;
using std::string;
using std::vector;
#define IMAGE_FRONT 0
#define IMAGE_RIGHT 1
#define IMAGE_REAR 2
#define IMAGE_LEFT 3

using namespace BirdView;
class Camera;
class Image
{
public:
    Image(unsigned int imageType)
    {
        this->imageType = imageType;
    };
    void addCamera(Camera camera);
    void addCamera(shared_ptr<Camera> camera);
    /**
     @brief 初始化相机的位置信息
    */
    static void initCarPositionPARA(std::string xmlPath);
    /**
    @brief 自动标定相机外参，生成鸟瞰图，同时利用鸟瞰图的角点反推图像角点重新标定内参
    */
    void autoCalibration(shared_ptr<cv::Mat> src,string storePath);
private:
    void generateUVTexture_2D(const shared_ptr<cv::Mat>& uv,const shared_ptr<cv::Mat>& cvImageData,shared_ptr<cv::Mat>& uvInteger,
    shared_ptr<cv::Mat>& uvDecimal,shared_ptr<cv::Mat>& BIRD);
    // 普陀山四目标定，12点的世界坐标位置获取
    void getWorldObject(vector<cv::Point> &outputCorner, cv::Point initPoint, int distance, cv::Size patternSize, unsigned int imageType);
    // 普陀山四目，12点标定布排序方法
    void sortCorner(vector<cv::Point2f> &outputCorner, int groupSize);
    static shared_ptr<CarPositionPARA> para;
    shared_ptr<Camera> camera;
    unsigned int imageType;
    static unsigned int imageWidth;
    static unsigned int imageHeight;
};
