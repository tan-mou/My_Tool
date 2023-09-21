#include "Config.h"
#include "FilterTool.h"
#include "MaskTool.h"
#include "ImageTool.h"
#include "CalibrationTool.h"
#include "ShaderDispatch.h"
#include "VaoDispatch.h"
#include "TextureDispatch.h"
#include "Window.h"
#include "Image.h"
#include "Camera.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <utility>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <fstream>
#include <istream>
#include <thread>
using namespace ImageTool;
#define PI 3.1415926525
#define IS_Test 0
#define IS_OpenGL 1
#define IS_Homony 1
#define IS_20 0
#if IS_Test
int main()
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
    cv::Mat kern1(3, 3, CV_8UC1, (unsigned char *)kernData);
    noCvInfoLog();
    string realCarPath0 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src0.png";
    string realCarPath1 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src1.png";
    string realCarPath2 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src2.png";
    string realCarPath3 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src3.png";
    string idealCarPath0 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/front.png";
    string idealCarPath1 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/right.png";
    string idealCarPath2 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/rear.png";
    string idealCarPath3 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/left.png";

    shared_ptr<cv::Mat> src = getCvShared(idealCarPath1);

    // 边缘检测
    shared_ptr<cv::Mat> canny1(std::make_shared<cv::Mat>());
    shared_ptr<cv::Mat> canny2(std::make_shared<cv::Mat>());

    // cv::Canny(*src, *canny1, 290, 100);
    // for (int i = 0; i < src->rows; i++)
    // {
    //     for (int j = 0; j < src->cols; j++)
    //     {
    //         if (canny1->at<uchar>(i, j) == 255)
    //         {
    //             src->at<cv::Vec3b>(i, j)[0] = 255;
    //             src->at<cv::Vec3b>(i, j)[1] = 255;
    //             src->at<cv::Vec3b>(i, j)[2] = 255;
    //             // gray->at<uchar>(i,j)=255;
    //         }
    //     }
    // }

    shared_ptr<cv::Mat> gray = getCvGray(src);
    showImage(gray, "gray1");

    // cv::addWeighted(*gray,1.3,*gray,0,20,*gray);
    // src2的中点
    // int para=(int)gray->at<uchar>(248,370);
    // src1的中点
    int para = (int)gray->at<uchar>(265, 386);

    int para2 = (int)gray->at<uchar>(276, 645);
    int para3 = (int)gray->at<uchar>(320, 139);
    std::cout << para << std::endl;
    std::cout << para2 << std::endl;
    std::cout << para3 << std::endl;
    for (int i = 0; i < gray->rows; i++)
    {
        for (int j = 0; j < gray->cols; j++)
        {

            int dis = abs(gray->at<uchar>(i, j) - para);
            if (gray->at<uchar>(i, j) != 255)
            {
                if (dis < 50)
                {
                    gray->at<uchar>(i, j) = 0;
                }
                else
                {
                    gray->at<uchar>(i, j) = 125;
                }
            }
        }
    }
    showImage(gray, "gray2");

    // cv::Canny(*gray, *canny2, 300, 100);
    // // canny=FilterTool::filter2D(canny,kern1);
    // for (int i = 0; i < src->rows; i++)
    // {
    //     for (int j = 0; j < src->cols; j++)
    //     {
    //         if (canny2->at<uchar>(i, j) == 255)
    //         {
    //             // src->at<cv::Vec3b>(i, j)[0] = 255;
    //             // src->at<cv::Vec3b>(i, j)[1] = 255;
    //             // src->at<cv::Vec3b>(i, j)[2] = 255;
    //             gray->at<uchar>(i,j)=255;
    //         }
    //     }
    // }
    // showImage(gray,"gray3");

    //   cv::Vec3b para1=src->at<cv::Vec3b>(266,630);
    //     cv::Vec3b para2=src->at<cv::Vec3b>(276,645);
    //     cv::Vec3b para3=src->at<cv::Vec3b>(232,596);
    //     std::cout<<para1<<std::endl;
    //     std::cout<<para2<<std::endl;
    //     std::cout<<para3<<std::endl;

    //     auto black = [&](cv::Vec3b para,float range)
    //     {
    //         for (int i = 0; i < src->rows; i++)
    //         {
    //             for (int j = 0; j < src->cols; j++)
    //             {
    //                 cv::Vec3b tmp = src->at<cv::Vec3b>(i, j);
    //                 int xDis = abs(tmp[0] - para[0]);
    //                 int yDis = abs(tmp[1] - para[1]);
    //                 int zDis = abs(tmp[2] - para[2]);
    //                 if (xDis < range && yDis < range && zDis < range)
    //                 {
    //                     src->at<cv::Vec3b>(i, j)[0] = 0;
    //                     src->at<cv::Vec3b>(i, j)[1] = 0;
    //                     src->at<cv::Vec3b>(i, j)[2] = 0;
    //                 }
    //             }
    //         }
    //     };
    //     black(para1,30);
    //     black(para2,30);
    //     black(para3,30);

    // showImage(src, "src");
    // showImage(canny,"canny");
    // cv::addWeighted(*gray,1,*canny,1,0,*gray);

    vector<cv::Point2f> corner;
    CalibrationTool::findCorner(src, corner, cv::Size(2, 2));
    shared_ptr<cv::Mat> srcM = MaskTool::putCornerMask(src, corner);
    vector<cv::Point2f> corner2;
    CalibrationTool::findCorner(srcM, corner2, cv::Size(2, 2));

    for (auto i : corner2)
    {
        std::cout << i << std::endl;
    }
    drawPoints(src, corner, "corner");
    drawPoints(src, corner2, "corner2");

    cv::Mat kernErode = cv::Mat::eye(3, 3, CV_8UC1);
    shared_ptr<cv::Mat> binay = FilterTool::adaptiveBinaryThresholdShow(src, 101, 5, true, 0);
    cv::GaussianBlur(*binay, *binay, cv::Size(5, 5), 0);
    // cv::GaussianBlur(*binay,*binay,cv::Size(5,5),0);

    // binay=FilterTool::filter2D(binay,kern1);
    // binay=FilterTool::filter2D(binay,kern1);
    showImage(binay, "binay");

    // cv::dilate(*binay,*binay,kernErode);
    // cv::erode(*gray,*gray,kernErode);
    // cv::erode(*binay,*binay,kernErode);

    vector<vector<cv::Point>> contour;
    vector<vector<cv::Point>> square;
    CalibrationTool::findContours(binay, contour);
    // int accurate=3;
    // for(auto i:contour)
    // {
    //     vector<cv::Point>approx;
    //     cv::approxPolyDP(i, approx, cv::arcLength(i, true) * accurate * 0.01, true);
    //     if(fabs(cv::contourArea(i)) > 1000)
    //         square.push_back(approx);
    // }
    CalibrationTool::findSquare(contour, square, 500, 3);
    drawContours(binay, contour, "Contours1");

    // showImage(src, "src");
    cv::waitKey(0);
}
#else
#if IS_OpenGL

float resHeight[1041] =
{
0,0.0017,0.0034,0.0051,0.0068,0.0085,0.0102,0.01191,0.01361,0.01531,0.01701,0.01871,0.02041,0.02211,0.02381,0.02551,0.02722,0.02892,0.03062,0.03232,0.03402,0.03572,
0.03743,0.03913,0.04083,0.04253,0.04423,0.04594,0.04764,0.04934,0.05104,0.05275,0.05445,0.05615,0.05786,0.05956,0.06126,0.06297,0.06467,0.06637,0.06808,0.06978,0.07149,
0.07319,0.0749,0.0766,0.07831,0.08001,0.08172,0.08342,0.08513,0.08684,0.08854,0.09025,0.09196,0.09366,0.09537,0.09708,0.09879,0.1005,0.1022,0.10391,0.10562,0.10733,
0.10904,0.11075,0.11246,0.11417,0.11588,0.11759,0.1193,0.12102,0.12273,0.12444,0.12615,0.12786,0.12958,0.13129,0.133,0.13472,0.13643,0.13815,0.13986,0.14158,0.14329,
0.14501,0.14673,0.14844,0.15016,0.15188,0.1536,0.15531,0.15703,0.15875,0.16047,0.16219,0.16391,0.16563,0.16735,0.16907,0.1708,0.17252,0.17424,0.17596,0.17769,0.17941,
0.18114,0.18286,0.18459,0.18631,0.18804,0.18976,0.19149,0.19322,0.19495,0.19668,0.1984,0.20013,0.20186,0.20359,0.20532,0.20706,0.20879,0.21052,0.21225,0.21399,0.21572,
0.21745,0.21919,0.22092,0.22266,0.2244,0.22613,0.22787,0.22961,0.23135,0.23309,0.23483,0.23657,0.23831,0.24005,0.24179,0.24353,0.24527,0.24702,0.24876,0.25051,0.25225,
0.254,0.25574,0.25749,0.25924,0.26098,0.26273,0.26448,0.26623,0.26798,0.26973,0.27148,0.27324,0.27499,0.27674,0.2785,0.28025,0.28201,0.28376,0.28552,0.28728,0.28903,
0.29079,0.29255,0.29431,0.29607,0.29783,0.29959,0.30136,0.30312,0.30488,0.30665,0.30841,0.31018,0.31194,0.31371,0.31548,0.31725,0.31901,0.32078,0.32255,0.32432,0.3261,
0.32787,0.32964,0.33141,0.33319,0.33496,0.33674,0.33852,0.34029,0.34207,0.34385,0.34563,0.34741,0.34919,0.35097,0.35275,0.35454,0.35632,0.3581,0.35989,0.36167,0.36346,
0.36525,0.36704,0.36882,0.37061,0.3724,0.37419,0.37599,0.37778,0.37957,0.38137,0.38316,0.38496,0.38675,0.38855,0.39035,0.39214,0.39394,0.39574,0.39754,0.39934,0.40115,
0.40295,0.40475,0.40656,0.40836,0.41017,0.41198,0.41378,0.41559,0.4174,0.41921,0.42102,0.42283,0.42465,0.42646,0.42827,0.43009,0.4319,0.43372,0.43554,0.43735,0.43917,0.44099,
0.44281,0.44463,0.44645,0.44828,0.4501,0.45192,0.45375,0.45558,0.4574,0.45923,0.46106,0.46289,0.46472,0.46655,0.46838,0.47021,0.47204,0.47388,0.47571,0.47755,0.47939,
0.48122,0.48306,0.4849,0.48674,0.48858,0.49042,0.49226,0.49411,0.49595,0.49779,0.49964,0.50149,0.50333,0.50518,0.50703,0.50888,0.51073,0.51258,0.51443,0.51629,0.51814,
0.52,0.52185,0.52371,0.52556,0.52742,0.52928,0.53114,0.533,0.53486,0.53673,0.53859,0.54045,0.54232,0.54418,0.54605,0.54792,0.54978,0.55165,0.55352,0.55539,0.55726,0.55914,
0.56101,0.56288,0.56476,0.56663,0.56851,0.57039,0.57227,0.57415,0.57602,0.57791,0.57979,0.58167,0.58355,0.58544,0.58732,0.58921,0.59109,0.59298,0.59487,0.59676,0.59865,
0.60054,0.60243,0.60432,0.60622,0.60811,0.61001,0.6119,0.6138,0.6157,0.61759,0.61949,0.62139,0.62329,0.6252,0.6271,0.629,0.63091,0.63281,0.63472,0.63662,0.63853,0.64044,
0.64235,0.64426,0.64617,0.64808,0.64999,0.65191,0.65382,0.65574,0.65765,0.65957,0.66149,0.66341,0.66533,0.66725,0.66917,0.67109,0.67301,0.67493,0.67686,0.67878,0.68071,
0.68263,0.68456,0.68649,0.68842,0.69035,0.69228,0.69421,0.69614,0.69808,0.70001,0.70195,0.70388,0.70582,0.70776,0.70969,0.71163,0.71357,0.71551,0.71745,0.7194,0.72134,
0.72328,0.72523,0.72717,0.72912,0.73106,0.73301,0.73496,0.73691,0.73886,0.74081,0.74276,0.74471,0.74667,0.74862,0.75058,0.75253,0.75449,0.75644,0.7584,0.76036,0.76232,
0.76428,0.76624,0.7682,0.77016,0.77213,0.77409,0.77606,0.77802,0.77999,0.78196,0.78392,0.78589,0.78786,0.78983,0.7918,0.79377,0.79575,0.79772,0.79969,0.80167,0.80364,
0.80562,0.80759,0.80957,0.81155,0.81353,0.81551,0.81749,0.81947,0.82145,0.82343,0.82542,0.8274,0.82939,0.83137,0.83336,0.83534,0.83733,0.83932,0.84131,0.8433,0.84529,
0.84728,0.84927,0.85126,0.85326,0.85525,0.85725,0.85924,0.86124,0.86323,0.86523,0.86723,0.86923,0.87123,0.87323,0.87523,0.87723,0.87923,0.88123,0.88324,0.88524,0.88724,
0.88925,0.89125,0.89326,0.89527,0.89728,0.89928,0.90129,0.9033,0.90531,0.90732,0.90933,0.91135,0.91336,0.91537,0.91739,0.9194,0.92142,0.92343,0.92545,0.92747,0.92948,
0.9315,0.93352,0.93554,0.93756,0.93958,0.9416,0.94362,0.94564,0.94767,0.94969,0.95171,0.95374,0.95576,0.95779,0.95981,0.96184,0.96387,0.9659,0.96792,0.96995,0.97198,
0.97401,0.97604,0.97807,0.9801,0.98214,0.98417,0.9862,0.98823,0.99027,0.9923,0.99434,0.99637,0.99841,1.00045,1.00248,1.00452,1.00656,1.00859,1.01063,1.01267,1.01471,
1.01675,1.01879,1.02083,1.02287,1.02492,1.02696,1.029,1.03104,1.03309,1.03513,1.03717,1.03922,1.04126,1.04331,1.04535,1.0474,1.04945,1.05149,1.05354,1.05559,1.05764,
1.05968,1.06173,1.06378,1.06583,1.06788,1.06993,1.07198,1.07403,1.07608,1.07813,1.08019,1.08224,1.08429,1.08634,1.08839,1.09045,1.0925,1.09455,1.09661,1.09866,1.10072,
1.10277,1.10483,1.10688,1.10894,1.11099,1.11305,1.1151,1.11716,1.11922,1.12127,1.12333,1.12539,1.12744,1.1295,1.13156,1.13362,1.13567,1.13773,1.13979,1.14185,1.14391,
1.14597,1.14802,1.15008,1.15214,1.1542,1.15626,1.15832,1.16038,1.16244,1.1645,1.16656,1.16862,1.17068,1.17274,1.1748,1.17685,1.17891,1.18097,1.18303,1.18509,1.18715,
1.18921,1.19127,1.19333,1.19539,1.19745,1.19951,1.20157,1.20363,1.20569,1.20775,1.20981,1.21187,1.21393,1.21599,1.21805,1.22011,1.22216,1.22422,1.22628,1.22834,1.2304,
1.23246,1.23451,1.23657,1.23863,1.24069,1.24274,1.2448,1.24686,1.24891,1.25097,1.25303,1.25508,1.25714,1.25919,1.26125,1.2633,1.26536,1.26741,1.26947,1.27152,1.27357,
1.27563,1.27768,1.27973,1.28178,1.28383,1.28588,1.28793,1.28998,1.29203,1.29408,1.29613,1.29818,1.30023,1.30228,1.30432,1.30637,1.30842,1.31046,1.31251,1.31455,1.3166,
1.31864,1.32068,1.32273,1.32477,1.32681,1.32885,1.33089,1.33293,1.33497,1.33701,1.33905,1.34108,1.34312,1.34516,1.34719,1.34923,1.35126,1.35329,1.35533,1.35736,1.35939,
1.36142,1.36345,1.36548,1.3675,1.36953,1.37156,1.37358,1.37561,1.37763,1.37965,1.38168,1.3837,1.38572,1.38774,1.38976,1.39177,1.39379,1.39581,1.39782,1.39983,1.40185,
1.40386,1.40587,1.40788,1.40989,1.4119,1.4139,1.41591,1.41791,1.41992,1.42192,1.42392,1.42592,1.42792,1.42992,1.43191,1.43391,1.4359,1.43789,1.43989,1.44188,1.44387,
1.44585,1.44784,1.44983,1.45181,1.45379,1.45577,1.45775,1.45973,1.46171,1.46369,1.46566,1.46763,1.46961,1.47158,1.47354,1.47551,1.47748,1.47944,1.4814,1.48337,1.48533,
1.48728,1.48924,1.4912,1.49315,1.4951,1.49705,1.499,1.50095,1.50289,1.50483,1.50678,1.50872,1.51065,1.51259,1.51453,1.51646,1.51839,1.52032,1.52225,1.52417,1.5261,1.52802,
1.52994,1.53186,1.53377,1.53569,1.5376,1.53951,1.54142,1.54332,1.54523,1.54713,1.54903,1.55093,1.55282,1.55472,1.55661,1.5585,1.56038,1.56227,1.56415,1.56603,1.56791,1.56979,
1.57166,1.57353,1.5754,1.57727,1.57913,1.581,1.58286,1.58471,1.58657,1.58842,1.59027,1.59212,1.59397,1.59581,1.59765,1.59949,1.60132,1.60316,1.60499,1.60681,1.60864,1.61046,
1.61228,1.6141,1.61591,1.61772,1.61953,1.62134,1.62314,1.62494,1.62674,1.62853,1.63033,1.63212,1.6339,1.63569,1.63747,1.63924,1.64102,1.64279,1.64456,1.64632,1.64809,1.64985,
1.6516,1.65336,1.65511,1.65685,1.6586,1.66034,1.66208,1.66381,1.66554,1.66727,1.669,1.67072,1.67243,1.67415,1.67586,1.67757,1.67927,1.68097,1.68267,1.68437,1.68606,1.68775,
1.68943,1.69111,1.69279,1.69446,1.69613,1.6978,1.69946,1.70112,1.70112,1.70112,1.70772,1.70772,1.70936,1.71099,1.71263,1.71425,1.71588,1.7175,1.71912,1.72073,1.72234,1.72395,
1.72555,1.72715,1.72874,1.73033,1.73191,1.7335,1.73507,1.73665,1.73822,1.73978,1.74134,1.7429,1.74445,1.746,1.74754,1.74908,1.75062,1.75215,1.75368,1.7552,1.75672,1.75823,
1.75974,1.76125,1.76275,1.76424,1.76574,1.76722,1.7687,1.77018,1.77166,1.77312,1.77459,1.77605,1.7775,1.77895,1.7804,1.78184,1.78327,1.78471,1.78613,1.78755,1.78897,1.79038,
1.79179,1.79319,1.79459,1.79598,1.79737,1.79875,1.80013,1.8015,1.80286,1.80423,1.80558,1.80694,1.80828,1.80962,1.81096,1.81229,1.81362,1.81494,1.81625,1.81756,1.81887,1.82017,
1.82146,1.82275,1.82403,1.82531,1.82658,1.82785,1.82911,1.83037,1.83162,1.83286,1.8341,1.83534,1.83656,1.83779,1.839,1.84021,1.84142,1.84262,1.84381,1.845,1.84618,1.84736,
1.84853,1.8497,1.85086,1.85201,1.85316,1.8543,1.85543,1.85656,1.85769,1.8588,1.85992,1.86102,1.86212,1.86321,1.8643,1.86538,1.86646,1.86753,1.86859,1.86964,1.87069,1.87174,
1.87278,1.87381,1.87483,1.87585,1.87686,1.87787,1.87887,1.87986,1.88085,1.88183,1.8828,1.88377,1.88473,1.88568,1.88663,1.88757,1.8885,1.88943
};
float cameraAngle[1041] =
{
0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
3,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,4,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,5,5.1,5.2,5.3,5.4,5.5,5.6,5.7,5.8,5.9,
6,6.1,6.2,6.3,6.4,6.5,6.6,6.7,6.8,6.9,7,7.1,7.2,7.3,7.4,7.5,7.6,7.7,7.8,7.9,8,8.1,8.2,8.3,8.4,8.5,8.6,8.7,8.8,8.9,
9,9.1,9.2,9.3,9.4,9.5,9.6,9.7,9.8,9.9,10,10.1,10.2,10.3,10.4,10.5,10.6,10.7,10.8,10.9,11,11.1,11.2,11.3,11.4,11.5,
11.6,11.7,11.8,11.9,12,12.1,12.2,12.3,12.4,12.5,12.6,12.7,12.8,12.9,13,13.1,13.2,13.3,13.4,13.5,13.6,13.7,13.8,13.9,
14,14.1,14.2,14.3,14.4,14.5,14.6,14.7,14.8,14.9,15,15.1,15.2,15.3,15.4,15.5,15.6,15.7,15.8,15.9,16,16.1,16.2,16.3,
16.4,16.5,16.6,16.7,16.8,16.9,17,17.1,17.2,17.3,17.4,17.5,17.6,17.7,17.8,17.9,18,18.1,18.2,18.3,18.4,18.5,18.6,18.7,
18.8,18.9,19,19.1,19.2,19.3,19.4,19.5,19.6,19.7,19.8,19.9,20,20.1,20.2,20.3,20.4,20.5,20.6,20.7,20.8,20.9,21,21.1,21.2,
21.3,21.4,21.5,21.6,21.7,21.8,21.9,22,22.1,22.2,22.3,22.4,22.5,22.6,22.7,22.8,22.9,23,23.1,23.2,23.3,23.4,23.5,23.6,
23.7,23.8,23.9,24,24.1,24.2,24.3,24.4,24.5,24.6,24.7,24.8,24.9,25,25.1,25.2,25.3,25.4,25.5,25.6,25.7,25.8,25.9,26,26.1,
26.2,26.3,26.4,26.5,26.6,26.7,26.8,26.9,27,27.1,27.2,27.3,27.4,27.5,27.6,27.7,27.8,27.9,28,28.1,28.2,28.3,28.4,28.5,28.6,
28.7,28.8,28.9,29,29.1,29.2,29.3,29.4,29.5,29.6,29.7,29.8,29.9,30,30.1,30.2,30.3,30.4,30.5,30.6,30.7,30.8,30.9,31,
31.1,31.2,31.3,31.4,31.5,31.6,31.7,31.8,31.9,32,32.1,32.2,32.3,32.4,32.5,32.6,32.7,32.8,32.9,33,33.1,33.2,33.3,33.4,
33.5,33.6,33.7,33.8,33.9,34,34.1,34.2,34.3,34.4,34.5,34.6,34.7,34.8,34.9,35,35.1,35.2,35.3,35.4,35.5,35.6,35.7,35.8,
35.9,36,36.1,36.2,36.3,36.4,36.5,36.6,36.7,36.8,36.9,37,37.1,37.2,37.3,37.4,37.5,37.6,37.7,37.8,37.9,38,38.1,38.2,38.3,
38.4,38.5,38.6,38.7,38.8,38.9,39,39.1,39.2,39.3,39.4,39.5,39.6,39.7,39.8,39.9,40,40.1,40.2,40.3,40.4,40.5,40.6,40.7,40.8,
40.9,41,41.1,41.2,41.3,41.4,41.5,41.6,41.7,41.8,41.9,42,42.1,42.2,42.3,42.4,42.5,42.6,42.7,42.8,42.9,43,43.1,43.2,43.3,
43.4,43.5,43.6,43.7,43.8,43.9,44,44.1,44.2,44.3,44.4,44.5,44.6,44.7,44.8,44.9,45,45.1,45.2,45.3,45.4,45.5,45.6,45.7,45.8,
45.9,46,46.1,46.2,46.3,46.4,46.5,46.6,46.7,46.8,46.9,47,47.1,47.2,47.3,47.4,47.5,47.6,47.7,47.8,47.9,48,48.1,48.2,48.3,
48.4,48.5,48.6,48.7,48.8,48.9,49,49.1,49.2,49.3,49.4,49.5,49.6,49.7,49.8,49.9,50,50.1,50.2,50.3,50.4,50.5,50.6,50.7,50.8,
50.9,51,51.1,51.2,51.3,51.4,51.5,51.6,51.7,51.8,51.9,52,52.1,52.2,52.3,52.4,52.5,52.6,52.7,52.8,52.9,53,53.1,53.2,53.3,
53.4,53.5,53.6,53.7,53.8,53.9,54,54.1,54.2,54.3,54.4,54.5,54.6,54.7,54.8,54.9,55,55.1,55.2,55.3,55.4,55.5,55.6,55.7,55.8,
55.9,56,56.1,56.2,56.3,56.4,56.5,56.6,56.7,56.8,56.9,57,57.1,57.2,57.3,57.4,57.5,57.6,57.7,57.8,57.9,58,58.1,58.2,58.3,
58.4,58.5,58.6,58.7,58.8,58.9,59,59.1,59.2,59.3,59.4,59.5,59.6,59.7,59.8,59.9,60,60.1,60.2,60.3,60.4,60.5,60.6,60.7,60.8,
60.9,61,61.1,61.2,61.3,61.4,61.5,61.6,61.7,61.8,61.9,62,62.1,62.2,62.3,62.4,62.5,62.6,62.7,62.8,62.9,63,63.1,63.2,63.3,
63.4,63.5,63.6,63.7,63.8,63.9,64,64.1,64.2,64.3,64.4,64.5,64.6,64.7,64.8,64.9,65,65.1,65.2,65.3,65.4,65.5,65.6,65.7,65.8,
65.9,66,66.1,66.2,66.3,66.4,66.5,66.6,66.7,66.8,66.9,67,67.1,67.2,67.3,67.4,67.5,67.6,67.7,67.8,67.9,68,68.1,68.2,68.3,
68.4,68.5,68.6,68.7,68.8,68.9,69,69.1,69.2,69.3,69.4,69.5,69.6,69.7,69.8,69.9,70,70.1,70.2,70.3,70.4,70.5,70.6,70.7,70.8,
70.9,71,71.1,71.2,71.3,71.4,71.5,71.6,71.7,71.8,71.9,72,72.1,72.2,72.3,72.4,72.5,72.6,72.7,72.8,72.9,73,73.1,73.2,73.3,
73.4,73.5,73.6,73.7,73.8,73.9,74,74.1,74.2,74.3,74.4,74.5,74.6,74.7,74.8,74.9,75,75.1,75.2,75.3,75.4,75.5,75.6,75.7,75.8,
75.9,76,76.1,76.2,76.3,76.4,76.5,76.6,76.7,76.8,76.9,77,77.1,77.2,77.3,77.4,77.5,77.6,77.7,77.8,77.9,78,78.1,78.2,78.3,
78.4,78.5,78.6,78.7,78.8,78.9,79,79.1,79.2,79.3,79.4,79.5,79.6,79.7,79.8,79.9,80,80.1,80.2,80.3,80.4,80.5,80.6,80.7,80.8,
80.9,81,81.1,81.2,81.3,81.4,81.5,81.6,81.7,81.8,81.9,82,82.1,82.2,82.3,82.4,82.5,82.6,82.7,82.8,82.9,83,83.1,83.2,83.3,
83.4,83.5,83.6,83.7,83.8,83.9,84,84.1,84.2,84.3,84.4,84.5,84.6,84.7,84.8,84.9,85,85.1,85.2,85.3,85.4,85.5,85.6,85.7,85.8,
85.9,86,86.1,86.2,86.3,86.4,86.5,86.6,86.7,86.8,86.9,87,87.1,87.2,87.3,87.4,87.5,87.6,87.7,87.8,87.9,88,88.1,88.2,88.3,
88.4,88.5,88.6,88.7,88.8,88.9,89,89.1,89.2,89.3,89.4,89.5,89.6,89.7,89.8,89.9,90,90.1,90.2,90.3,90.4,90.5,90.6,90.7,90.8,
90.9,91,91.1,91.2,91.3,91.4,91.5,91.6,91.7,91.8,91.9,92,92.1,92.2,92.3,92.4,92.5,92.6,92.7,92.8,92.9,93,93.1,93.2,93.3,93.4,
93.5,93.6,93.7,93.8,93.9,94,94.1,94.2,94.3,94.4,94.5,94.6,94.7,94.8,94.9,95,95.1,95.2,95.3,95.4,95.5,95.6,95.7,95.8,95.9,
96,96.1,96.2,96.3,96.4,96.5,96.6,96.7,96.8,96.9,97,97.1,97.2,97.3,97.4,97.5,97.6,97.7,97.8,97.9,98,98.1,98.2,98.3,98.4,
98.5,98.6,98.7,98.8,98.9,99,99.1,99.2,99.3,99.4,99.5,99.6,99.7,99.8,99.9,100,100.1,100.2,100.3,100.4,100.5,100.6,100.7,
100.8,100.9,101,101.1,101.2,101.3,101.4,101.5,101.6,101.7,101.8,101.9,102,102.1,102.2,102.3,102.4,102.5,102.6,102.7,102.8,
102.9,103,103.1,103.2,103.3,103.4,103.5,103.6,103.7,103.8,103.9,104,
};
// // 普陀山四目，12点标定布排序方法
// void sortCorner(vector<cv::Point2f> &outputCorner, int groupSize)
// {
//     auto increaseX = [](cv::Point2f p1, cv::Point2f p2)
//     {
//         return p1.x < p2.x;
//     };
//     auto decreaseY = [](cv::Point2f p1, cv::Point2f p2)
//     {
//         return p1.y > p2.y;
//     };

//     std::sort(outputCorner.begin(), outputCorner.end(), decreaseY);
//     int currentIndex = 0;
//     // 思路：现根据y进行一次分组，然后再根据x进行排序，最后将所有组别混合在一起
//     // 组
//     vector<vector<cv::Point2f>> group;
//     for (int i = 0; i < outputCorner.size() / groupSize; i++)
//     {
//         vector<cv::Point2f> tmpGroup;
//         for (int j = 0; j < groupSize; j++)
//         {
//             int index = i * groupSize + j;
//             tmpGroup.push_back(outputCorner[index]);
//         }
//         group.push_back(tmpGroup);
//     }
//     outputCorner.clear();
//     for (auto i : group)
//     {
//         std::sort(i.begin(), i.end(), increaseX);
//         outputCorner.insert(outputCorner.end(), i.begin(), i.end());
//     }
// }
// // 普陀山四目标定，12点的世界坐标位置获取
// void getWorldObject(vector<cv::Point> &outputCorner, cv::Point initPoint, int distance, cv::Size patternSize, unsigned int imageType)
// {
//     outputCorner.clear();
//     if (imageType == IMAGE_FRONT)
//     {
//         // 获取左右的邻域点
//         auto getNeighbourhoodX = [](cv::Point pointCenter, int distance)
//         {
//             cv::Point pointLeft = pointCenter - cv::Point(distance, 0);
//             cv::Point pointRight = pointCenter + cv::Point(distance, 0);
//             return vector<cv::Point>{pointLeft, pointCenter, pointRight};
//         };
//         int height = patternSize.height;
//         for (int i = 0; i < height; i++)
//         {
//             cv::Point currentPoint = initPoint + cv::Point(0, i * distance);
//             vector<cv::Point> tmpGroup = getNeighbourhoodX(currentPoint, distance);
//             outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
//         }
//     }
//     // 获取后相机的世界坐标
//     else if (imageType == IMAGE_REAR)
//     {
//         // 获取左右的邻域点
//         auto getNeighbourhoodX = [](cv::Point pointCenter, int distance)
//         {
//             cv::Point pointLeft = pointCenter + cv::Point(distance, 0);
//             cv::Point pointRight = pointCenter - cv::Point(distance, 0);
//             return vector<cv::Point>{pointLeft, pointCenter, pointRight};
//         };
//         int height = patternSize.height;
//         for (int i = 0; i < height; i++)
//         {
//             cv::Point currentPoint = initPoint - cv::Point(0, i * distance);
//             vector<cv::Point> tmpGroup = getNeighbourhoodX(currentPoint, distance);
//             outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
//         }
//     }
//     // 获取右相机的世界坐标系坐标
//     else if (imageType == IMAGE_RIGHT)
//     {
//         // 获取左右的邻域点
//         auto getNeighbourhoodY = [](cv::Point pointCenter, int distance)
//         {
//             cv::Point pointLeft = pointCenter + cv::Point(0, distance);
//             cv::Point pointRight = pointCenter - cv::Point(0, distance);
//             return vector<cv::Point>{pointLeft, pointCenter, pointRight};
//         };
//         int height = patternSize.height;
//         for (int i = 0; i < height; i++)
//         {
//             cv::Point currentPoint = initPoint + cv::Point(i * distance, 0);
//             vector<cv::Point> tmpGroup = getNeighbourhoodY(currentPoint, distance);
//             outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
//         }
//     }
//     // 获取左相机的世界坐标系坐标
//     else if (imageType == IMAGE_LEFT)
//     {
//         // 获取左右的邻域点
//         auto getNeighbourhoodY = [](cv::Point pointCenter, int distance)
//         {
//             cv::Point pointLeft = pointCenter - cv::Point(0, distance);
//             cv::Point pointRight = pointCenter + cv::Point(0, distance);
//             return vector<cv::Point>{pointLeft, pointCenter, pointRight};
//         };
//         int height = patternSize.height;
//         for (int i = 0; i < height; i++)
//         {
//             cv::Point currentPoint = initPoint - cv::Point(i * distance, 0);
//             vector<cv::Point> tmpGroup = getNeighbourhoodY(currentPoint, distance);
//             outputCorner.insert(outputCorner.end(), tmpGroup.begin(), tmpGroup.end());
//         }
//     }
// }
// class Camera
// {
// public:
//     Camera(cv::Mat intrinsic, cv::Mat distortCoeff)
//     {
//         this->intrinsic = intrinsic;
//         this->distortCoeff = distortCoeff;
//     }
//     Camera(float dx_f, float dy_f, float cx, float cy, float k1, float k2, float k3, float k4)
//     {
//         intrinsic = cv::Mat::zeros(3, 3, CV_32F);
//         intrinsic.at<float>(0, 0) = dx_f;
//         intrinsic.at<float>(1, 1) = dy_f;
//         intrinsic.at<float>(0, 2) = cx;
//         intrinsic.at<float>(1, 2) = cy;
//         intrinsic.at<float>(2, 2) = 1;
//         distortCoeff = cv::Mat(4, 1, CV_32F);
//         distortCoeff.at<float>(0, 0) = k1;
//         distortCoeff.at<float>(1, 0) = k2;
//         distortCoeff.at<float>(2, 0) = k3;
//         distortCoeff.at<float>(3, 0) = k4;
//     }
//     // 根据像素和世界坐标，建立相机的外参
//     void initCameraExtrinsic(const vector<cv::Point2f> pixelPos, const vector<cv::Point> worldPos)
//     {
//         cv::Mat zeroDistCoeff = cv::Mat::zeros(1, 5, CV_32F);
//         cv::Mat eyeMatrix = cv::Mat::eye(3, 3, CV_32F);
//         vector<cv::Point3f> worldF3;
//         // 从cv::Point转换到2f
//         vector<cv::Point2f> pixelF2;
//         vector<cv::Point2f> pixelF2_2;
//         vector<cv::Point3f> worldF3_2;

//         vector<cv::Point2f> pixelF2_3;
//         vector<cv::Point3f> worldF3_3;
//         int j=0;

//         for (auto i : pixelPos)
//         {
//             pixelF2.push_back(cv::Point2f(i.x, i.y));

//             if(j<12);
//                 // pixelF2.push_back(cv::Point2f(i.x, i.y));
//             else
//                 pixelF2_2.push_back(cv::Point2f(i.x, i.y));
//             // else if(j<20)
//             //     pixelF2_3.push_back(cv::Point2f(i.x, i.y));

//             j++;
            
//         }
//        int m = 0;
//         for (auto i : worldPos)
//         {
//             worldF3.push_back(cv::Point3f(i.x, i.y, 0));
//             if(m<12);
//                 // worldF3.push_back(cv::Point3f(i.x, i.y, 0));
//             else
//                 worldF3_2.push_back(cv::Point3f(i.x, i.y, 0));
//             // else if(m<20)
//             //     worldF3_3.push_back(cv::Point3f(i.x, i.y, 0));
//             m++;
//         }
//         vector<cv::Point2f> undistortCorner;
//         vector<cv::Point2f> undistortCorner_2;
//         vector<cv::Point2f> undistortCorner_3;

//         cv::fisheye::undistortPoints(pixelF2, undistortCorner, intrinsic, distortCoeff,intrinsic);
//         cv::fisheye::undistortPoints(pixelF2_2, undistortCorner_2, intrinsic, distortCoeff,intrinsic);
//         cv::fisheye::undistortPoints(pixelF2_3, undistortCorner_3, intrinsic, distortCoeff,intrinsic);
//         rvec.empty();
//         tvec.empty();
//         // rvec,tvec是外参数据
//         cv::solvePnP(worldF3, undistortCorner, intrinsic, zeroDistCoeff, rvec, tvec);
//         if(worldF3_2.size()>=4)
//             cv::solvePnP(worldF3_2, undistortCorner_2, intrinsic, zeroDistCoeff, rvec, tvec,true);

//         // if(worldF3_3.size()>=4)
//         //     cv::solvePnP(worldF3_3, undistortCorner_3, intrinsic, zeroDistCoeff, rvec, tvec,true);

//         // cv::Mat homony=cv::findHomography(worldF3,pixelF2);
//         vector<cv::Point2f> reHomonyCorner;
//         vector<cv::Point2f> reProjectCorners;
//         cv::fisheye::projectPoints(worldF3, reProjectCorners, rvec, tvec, intrinsic, distortCoeff);
//         float reProjectionError = 0;
//         auto k = pixelF2.begin();
//         for (auto j : reProjectCorners)
//         {
//             float X = j.x - k->x;
//             float Y = j.y - k->y;
//             // std::cout << sqrt(X * X + Y * Y) << std::endl;
//             reProjectionError += sqrt(X * X + Y * Y) / pixelF2.size();
//             k++;
//         }
//         std::cout << "extrinsic reProjectionError:" << reProjectionError << std::endl;
//     }

// public:
//     // 摄像机的内参
//     cv::Mat intrinsic;
//     cv::Mat distortCoeff;
//     // 摄像机的外参
//     cv::Mat rvec;
//     cv::Mat tvec;
// };
// class Image
// {
// public:
//     Image(unsigned int imageType)
//     {
//         this->imageType = imageType;
//     };
//     void addCamera(Camera camera)
//     {
//         this->camera = std::make_shared<Camera>(camera);
//     }
//     void addCamera(shared_ptr<Camera> camera)
//     {
//         this->camera = camera;
//     }

// public:
//     // 起始点
//     static cv::Point2f initalWorldPoint;
//     // 范围
//     static float worldWidth;
//     static float worldHeight;
//     // 车模范围
//     static cv::Point2f carModelInitPoint;
//     static float carWidth;
//     static float carHeight;

//     // 拼接区域的大小因子
//     static float spliceSize;

//     // 车长
//     static unsigned int imageWidth;
//     static unsigned int imageHeight;
//     shared_ptr<Camera> camera;
//     unsigned int imageType;
// };
// cv::Point2f Image::initalWorldPoint = cv::Point2f(-2600-800, 4450+800);
// cv::Point2f Image::carModelInitPoint = cv::Point2f(-1200+200+200, 2850-400);
// float Image::carWidth = 2400-800;
// float Image::carHeight = 5700-800;
// float Image::worldWidth = 5200+1600;
// float Image::worldHeight = 8700+1600;
// float Image::spliceSize = 500;
// int imageWidth = 720;
// int imageHeight = 480;
// unsigned int Image::imageWidth = 720;
// unsigned int Image::imageHeight = 480;
// struct CarPositionPARA
// {
//     //左上角的世界坐标位置
//     cv::Point2f initalWorldPoint;
//     //车模左上角世界坐标位置
//     cv::Point2f carModelInitPoint;
//     //车宽
//     float carWidth;
//     //车高
//     float carHeight;
//     //鸟瞰图中世界坐标的宽
//     float worldWidth;
//     //鸟瞰图中世界坐标的高
//     float worldHeight;
//     //拼接区的大小
//     float spliceSize;
// };
// //控制是否输出世界坐标和对应的uv坐标的宏
// #define IS_OUTPUT_UV_WORLD 0
// shared_ptr<cv::Mat> generateMap(shared_ptr<CarPositionPARA> para,unsigned int imageWidth,unsigned int imageHeight,unsigned int imageType)
// {

//     vector<cv::Point3f> objectPos;
//     string objectFilePath = string(CMAKE_SOURCE_DIR) + "/set/output/object/world.txt";
//     std::ofstream OutFile1(objectFilePath.c_str());
//     string uvFilePath = string(CMAKE_SOURCE_DIR) + "/set/output/uv/uv.txt";
//     std::ofstream OutFile2(uvFilePath.c_str());

//     unsigned int mapHeight = imageHeight;
//     unsigned int mapWidth = imageWidth / 2;
//     // 这里的坐标系是世界坐标系
//     cv::Point2f initalWorldPoint = para->initalWorldPoint;
//     cv::Point2f carModelInitPoint = para->carModelInitPoint;
//     float carWidth = para->carWidth;
//     float carHeight = para->carHeight;
//     float worldHeight = para->worldHeight;
//     float worldWidth = para->worldWidth;
//     float spliceSize = para->spliceSize;


//     cv::Mat intrinsic = I->camera->intrinsic;
//     cv::Mat distortCoeff = I->camera->distortCoeff;
//     cv::Mat rvec = I->camera->rvec;
//     cv::Mat tvec = I->camera->tvec;


//     // std::cout << "rvec2:" << rvec << std::endl;
//     float carMinX = carModelInitPoint.x;
//     float carMaxX = carModelInitPoint.x + carWidth;
//     float carMaxY = carModelInitPoint.y;
//     float carMinY = carModelInitPoint.y - carHeight;

//     float initMinX = initalWorldPoint.x;
//     float initMaxX = initalWorldPoint.x + worldWidth;
//     float initMaxY = initalWorldPoint.y;
//     float initMinY = initalWorldPoint.y - worldHeight;
//     for (int i = 0; i < mapHeight; i++)
//     {
//         for (int j = 0; j < mapWidth; j++)
//         {
//             float x = initalWorldPoint.x + j * worldWidth / mapWidth;
//             float y = initalWorldPoint.y - i * worldHeight / mapHeight;
// #if IS_OUTPUT_UV_WORLD
//             OutFile1 << x << "\t" << y << "\n";
// #endif
//             cv::Point3f currentWorld(x, y, 0);
//             objectPos.push_back(currentWorld);
//         }
//     }

//     OutFile1.close();
//     vector<cv::Point2f> projectPoint;
//     cv::fisheye::projectPoints(objectPos, projectPoint, rvec, tvec, intrinsic, distortCoeff);

// #if IS_OUTPUT_UV_WORLD

//     if (!OutFile2.is_open())
//     {
//         std::cout << "ERROR::Image::x output file cant open" << std::endl;
//     }
//     for (auto &i : projectPoint)
//     {
//         OutFile2 << i.x << "\t" << i.y << "\n";
//     }
// #endif
//     OutFile2.close();
//     // caculate uv
//     // the rule of uv is opengl ruler not opencv ruler
//     // 因为对数据图片已经进行转置统一了，所以按照OpenGL的来

//     shared_ptr<cv::Mat> result(std::make_shared<cv::Mat>(mapHeight, mapWidth, CV_32FC4));
//     auto normalize = [](cv::Point2f &point)
//     {
//         float length = sqrt(pow(point.x, 2) + pow(point.y, 2));
//         point.x /= length;
//         point.y /= length;
//     };
//     auto cross = [](cv::Point2f p1, cv::Point2f p2)
//     {
//         cv::Point3i P1(p1.x, p1.y, 0);
//         cv::Point3i P2(p2.x, p2.y, 0);
//         return (P1.cross(P2)).z;
//     };
//     auto getLength = [](cv::Point2f P)
//     {
//         return sqrt(P.x * P.x + P.y * P.y);
//     };
//     // L2是直线的方向，L1是直线上一点到P点的方向
//     auto getPointLineDistance = [=](cv::Point2f L1, cv::Point2f L2)
//     {
//         normalize(L2);
//         float distanceProject = L1.dot(L2);
//         float distance = sqrt(pow(getLength(L1), 2) - pow(distanceProject, 2));
//         return distance;
//     };
//     // 生成四个角的透明度方法
//     // ABCD是逆时针的四个范围点
//     // isAhead表示是凭借区域是上图还是下图，isUpper表示是上半部分的图，还是下半部的图
//     auto genateBorderAlpha = [=](cv::Point2f A, cv::Point2f B, cv::Point2f C, cv::Point2f D, cv::Point2f O, bool isAhead)
//     {
//         cv::Point2f directionBC = C - B;
//         cv::Point2f directionAD = D - A;
//         normalize(directionBC);
//         normalize(directionAD);

//         float z1 = cross(O - B, C - B);
//         float z2 = cross(O - A, D - A);
//         // 如果在拼接区内，根据点到AD，BC的距离去判断透明度
//         if (z1 <= 0 && z2 >= 0)
//         {
//             // O到AD的距离
//             float distanceAD = getPointLineDistance(O - A, directionAD);
//             // O到BC的距离
//             float distanceBC = getPointLineDistance(O - B, directionBC);
//             // 离bc越远，后的alpha越大
//             float alphaBehind = distanceBC / (distanceAD + distanceBC);
//             // 离ad越远，前的alpha越大
//             float alphaAhead = distanceAD / (distanceAD + distanceBC);
//             if (isAhead)
//             {
//                 return alphaAhead * 255.0f;
//             }
//             else
//             {
//                 return alphaBehind * 255.0f;
//             }
//         }
//         // 上半部分，下显示0
//         else if (z1 >= 0 && !isAhead)
//         {
//             return 0.0f;
//         }
//         // 下半部分，上显示0
//         else if (z2 <= 0 && isAhead)
//         {
//             return 0.0f;
//         }
//         else
//             return 255.0f;
//     };
//     for (int i = 0; i < mapHeight; i++)
//     {
//         for (int j = 0; j < mapWidth; j++)
//         {
//             float x = initalWorldPoint.x + j * worldWidth / mapWidth;
//             float y = initalWorldPoint.y - i * worldHeight / mapHeight;
//             // if(x!=0)
//             //     continue;
//             int index = i * mapWidth + j;
//             projectPoint[index].x /= imageWidth;
//             // 转到OpenGL坐标系
//             projectPoint[index].y = 1 - projectPoint[index].y / imageHeight;

//             // 当前点
//             cv::Point2f O = cv::Point2f(x, y); // current Point
//             // 如果在矩形区域内，直接将map置空
//             if (x > carMinX && x < carMaxX && y > carMinY && y < carMaxY)
//             {
//                 result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 0);
//             }
//             else
//             {
//                 if (imageType == IMAGE_FRONT)
//                 {
//                     if (y <= carMaxY)
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 0);
//                     }
//                     else
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 255);
//                     }
//                     // 限制在左上角的区域
//                     if (O.x <= carMinX && O.y >= carMaxY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = carMinX - spliceSize;
//                         A.y = carMaxY;
//                         B.x = carMinX;
//                         B.y = carMaxY + spliceSize;

//                         C.x = initMinX + spliceSize;
//                         C.y = initMaxY;
//                         D.x = initMinX;
//                         D.y = initMaxY - spliceSize;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, true);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                     // 限制在右上角的区域
//                     else if (O.x >= carMaxX && O.y >=carMaxY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = initMaxX;
//                         A.y = initMaxY - spliceSize;
//                         B.x = initMaxX - spliceSize;
//                         B.y = initMaxY;

//                         C.x = carMaxX;
//                         C.y = carMaxY + spliceSize;
//                         D.x = carMaxX + spliceSize;
//                         D.y = carMaxY;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, true);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                 }
//                 else if (imageType == IMAGE_REAR)
//                 {
//                     if (y >= carMinY)
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 0);
//                     }
//                     else
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 255);
//                     }
//                     // 限制在左下角的区域
//                     if (O.x <= carMinX && O.y <= carMinY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = carMinX;
//                         A.y = carMinY - spliceSize;
//                         B.x = carMinX - spliceSize;
//                         B.y = carMinY;

//                         C.x = initMinX;
//                         C.y = initMinY + spliceSize;
//                         D.x = initMinX + spliceSize;
//                         D.y = initMinY;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, false);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                     // 限制在右下角的区域
//                     else if (O.x >= carMaxX && O.y <= carMinY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = initMaxX - spliceSize;
//                         A.y = initMinY;
//                         B.x = initMaxX;
//                         B.y = initMinY - spliceSize;

//                         C.x = carMaxX + spliceSize;
//                         C.y = carMinY;
//                         D.x = carMaxX;
//                         D.y = carMinY - spliceSize;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, false);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                 }
//                 else if (imageType == IMAGE_LEFT)
//                 {
//                     if (x >= carMinX)
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 0);
//                     }
//                     else
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 255);
//                     }
//                     // 限制在左上角的区域
//                     if (O.x <= carMinX && O.y >=carMaxY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = carMinX - spliceSize;
//                         A.y = carMaxY;
//                         B.x = carMinX;
//                         B.y = carMaxY + spliceSize;

//                         C.x = initMinX + spliceSize;
//                         C.y = initMaxY;
//                         D.x = initMinX;
//                         D.y = initMaxY - spliceSize;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, false);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                     // 限制在左下角的区域
//                     if (O.x <= carMinX && O.y <= carMinY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = carMinX;
//                         A.y = carMinY - spliceSize;
//                         B.x = carMinX - spliceSize;
//                         B.y = carMinY;

//                         C.x = initMinX;
//                         C.y = initMinY + spliceSize;
//                         D.x = initMinX + spliceSize;
//                         D.y = initMinY;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, true);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                 }
//                 else if (imageType == IMAGE_RIGHT)
//                 {
//                     if (x <= carMaxX)
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 0);
//                     }
//                     else
//                     {
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, 255);
//                     }
//                     // 限制在右上角的区域
//                     if (O.x >= carMaxX && O.y >= carMaxY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = initMaxX;
//                         A.y = initMaxY - spliceSize;
//                         B.x = initMaxX - spliceSize;
//                         B.y = initMaxY;

//                         C.x = carMaxX;
//                         C.y = carMaxY + spliceSize;
//                         D.x = carMaxX + spliceSize;
//                         D.y = carMaxY;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, false);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                     // 限制在右下角的区域
//                     if (O.x >= carMaxX && O.y <= carMinY)
//                     {
//                         cv::Point2f A, B, C, D;
//                         // ABCD 为拼接区域的四个点
//                         A.x = initMaxX - spliceSize;
//                         A.y = initMinY;
//                         B.x = initMaxX;
//                         B.y = initMinY - spliceSize;

//                         C.x = carMaxX + spliceSize;
//                         C.y = carMinY;
//                         D.x = carMaxX;
//                         D.y = carMinY - spliceSize;
//                         float alpha = genateBorderAlpha(A, B, C, D, O, true);
//                         result->at<cv::Vec4f>(i, j) = cv::Vec4f(0, projectPoint[index].y * 255, projectPoint[index].x * 255, alpha);
//                     }
//                 }
//                 else
//                 {
//                     std::cout<<"ERROR IN generateMap Function ::imageType Error"<<std::endl;
//                     return nullptr;
//                 }
//             }
//         }
//     }
//     return result;
// }
// int groupSize = 3;
// //临时的方法，之后请整理修改
// void generateMapTmp(shared_ptr<cv::Mat> src,Image image,unsigned int imageType,cv::Size patternSize,cv::Point initPos,int distance,string storePath)
// {
//     /*
//     重要函数
//     实现思路为：
//     1.先寻找34布的角点，计算一次内参
//     2.根据34布的角点先生成一次鸟瞰图
//     3.在对鸟瞰图去找22的角点，然后反映射回其在图像中的角点，然后再重新计算一次内参
//     */
//     string fileName;
//     if (imageType == IMAGE_FRONT)
//     {
//         fileName = "front";
//     }
//     else if (imageType == IMAGE_RIGHT)
//     {
//         fileName = "right";
//     }
//     else if (imageType == IMAGE_REAR)
//     {
//         fileName = "rear";
//     }
//     else if (imageType == IMAGE_LEFT)
//     {
//         fileName = "left";
//     }
//     shared_ptr<cv::Mat> undistorImage = std::make_shared<cv::Mat>();
//     cv::Mat newIntrinsic= cv::Mat::zeros(3, 3, CV_32F);
//     image.camera->intrinsic.copyTo(newIntrinsic);
//     newIntrinsic.at<float>(0, 0) ;
//     newIntrinsic.at<float>(1, 1) ;
//     shared_ptr<cv::Mat> cvImageData = std::make_shared<cv::Mat>();
//     cv::flip(*src, *cvImageData, 0);

//     vector<cv::Point2f> cornerPixel;
//     vector<cv::Point> cornerWorld;
//     shared_ptr<cv::Mat> gray=getCvGray(src);

//     vector<cv::Point2f> cornerPixel2;
//     vector<cv::Point2f> cornerPixel3;

//     bool isFind = CalibrationTool::findCorner(src, cornerPixel, patternSize);
//     sortCorner(cornerPixel, patternSize.width);
//     getWorldObject(cornerWorld,initPos , distance, patternSize,imageType);
//     if (isFind)
//         cornerSubPix(*gray, cornerPixel, cv::Size(4, 4), cv::Size(5, 5),
//                      cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));

//     sortCorner(cornerPixel, patternSize.width);

// //寻找之后的20个角点
// #if IS_20
//     bool isFind2=CalibrationTool::findCorner(src,cornerPixel2,cv::Size(2,2));
//     shared_ptr<cv::Mat> tmpSrc;
//     bool isFind3=false;
//     if(isFind2)
//     {
//         tmpSrc = MaskTool::putCornerMask(src, cornerPixel2);
//         isFind3=CalibrationTool::findCorner(tmpSrc,cornerPixel3,cv::Size(2,2));
//     }
//     sortCorner(cornerPixel2, 2);
//     sortCorner(cornerPixel3, 2);
//     if (isFind2)
//     cornerSubPix(*gray, cornerPixel2, cv::Size(4, 4), cv::Size(5, 5),
//                  cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));
//     if (isFind3)
//     cornerSubPix(*gray, cornerPixel3, cv::Size(4, 4), cv::Size(5, 5),
//                  cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1e-6));
//     if(imageType==IMAGE_FRONT)
//     {

//         // // show circle
//         // cv::Mat tmpShow;
//         // src->copyTo(tmpShow);
//         // for (auto l : cornerPixel2)
//         // {
//         //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // }
//         // for (auto l : cornerPixel3)
//         // {
//         //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // }
//         // string windowName2 = "findSquare0 ";
//         // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
//         // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
//         // cv::imshow(windowName2, tmpShow);


//         // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
//         // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());



//         // cornerWorld.push_back(cv::Point(1800, 3450));
//         // cornerWorld.push_back(cv::Point(2200, 3450));
//         // cornerWorld.push_back(cv::Point(1800, 3850));
//         // cornerWorld.push_back(cv::Point(2200, 3850));


//         // cornerWorld.push_back(cv::Point(-2200, 3450));
//         // cornerWorld.push_back(cv::Point(-1800, 3450));
//         // cornerWorld.push_back(cv::Point(-2200, 3850));
//         // cornerWorld.push_back(cv::Point(-1800, 3850));
//     }
//     else if(imageType==IMAGE_RIGHT)
//     {
//         // // show circle
//         // cv::Mat tmpShow;
//         // src->copyTo(tmpShow);
//         // for (auto l : cornerPixel2)
//         // {
//         //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // }
//         // // for (auto l : cornerPixel3)
//         // // {
//         // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // // }
//         // string windowName2 = "findSquare1 ";
//         // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
//         // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
//         // cv::imshow(windowName2, tmpShow);

//         // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
//         // // // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());
//         // cornerPixel.push_back(cv::Point(626,253));
//         // cornerPixel.push_back(cv::Point(632,253));
//         // cornerPixel.push_back(cv::Point(611,240));
//         // cornerPixel.push_back(cv::Point(618,239));


//         // //实车2右标定布，大块角点
//         // cornerPixel.push_back(cv::Point(634,279));
//         // cornerPixel.push_back(cv::Point(650,267));
//         // cornerPixel.push_back(cv::Point(576,222));
//         // cornerPixel.push_back(cv::Point(607,223));

//         // //实车4右标定布，大块角点


//         // cornerWorld.push_back(cv::Point(1800, 3850));
//         // cornerWorld.push_back(cv::Point(1800, 3450));
//         // cornerWorld.push_back(cv::Point(2200, 3850));
//         // cornerWorld.push_back(cv::Point(2200, 3450));

//         // // cornerWorld.push_back(cv::Point(1800, -3450));
//         // // cornerWorld.push_back(cv::Point(1800, -3850));
//         // // cornerWorld.push_back(cv::Point(2200, -3450));
//         // // cornerWorld.push_back(cv::Point(2200, -3850));

//         // cornerWorld.push_back(cv::Point(1800-600, -3450+600));
//         // cornerWorld.push_back(cv::Point(1800-600, -3850-600));
//         // cornerWorld.push_back(cv::Point(2200+400, -3450+600));
//         // cornerWorld.push_back(cv::Point(2200+400, -3850-600));
//     }
//     else if (imageType == IMAGE_LEFT)
//     {
//         // // show circle
//         // cv::Mat tmpShow;
//         // src->copyTo(tmpShow);
//         // for (auto l : cornerPixel2)
//         // {
//         //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // }
//         // // // for (auto l : cornerPixel3)
//         // // // {
//         // // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // // // }
//         // string windowName2 = "findSquare2 ";
//         // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
//         // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
//         // cv::imshow(windowName2, tmpShow);

//         // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());
//         // // // // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());

//         // // cornerPixel.push_back(cv::Point(88,250));
//         // // cornerPixel.push_back(cv::Point(95,250));
//         // // cornerPixel.push_back(cv::Point(103,236));
//         // // cornerPixel.push_back(cv::Point(110,239));


//         //  //实车2的大标定布坐标
//         // // cornerPixel.push_back(cv::Point(63, 268));
//         // // cornerPixel.push_back(cv::Point(79, 278));
//         // // cornerPixel.push_back(cv::Point(106, 226));
//         // // cornerPixel.push_back(cv::Point(137, 224));



//         // //实车4小标定坐标
//         // cornerPixel.push_back(cv::Point(88, 243));
//         // cornerPixel.push_back(cv::Point(94, 244));
//         // cornerPixel.push_back(cv::Point(104, 230));
//         // cornerPixel.push_back(cv::Point(110, 231));

//         // // 实车4的大标定布坐标
//         // cornerPixel.push_back(cv::Point(61, 259));
//         // cornerPixel.push_back(cv::Point(77, 271));
//         // cornerPixel.push_back(cv::Point(109, 217));
//         // cornerPixel.push_back(cv::Point(140, 217));

        
//         // // // // 左上角四个标定布的角点
//         // cornerWorld.push_back(cv::Point(-1800, 3450));
//         // cornerWorld.push_back(cv::Point(-1800, 3850));
//         // cornerWorld.push_back(cv::Point(-2200, 3450));
//         // cornerWorld.push_back(cv::Point(-2200, 3850));
//         // // // // // //左下角四个标定布的角点
//         // cornerWorld.push_back(cv::Point(-1800, -3850));
//         // cornerWorld.push_back(cv::Point(-1800, -3450));
//         // cornerWorld.push_back(cv::Point(-2200, -3850));
//         // cornerWorld.push_back(cv::Point(-2200, -3450));

//         // // // // //左下角大标定布的四个角
//         // cornerWorld.push_back(cv::Point(-1800+600, -3850-600));
//         // cornerWorld.push_back(cv::Point(-1800+600, -3450+600));
//         // cornerWorld.push_back(cv::Point(-2200-400, -3850-600));
//         // cornerWorld.push_back(cv::Point(-2200-400, -3450+600));


//     }
//     else if (imageType == IMAGE_REAR)
//     {
//         // // show circle
//         // cv::Mat tmpShow;
//         // src->copyTo(tmpShow);
//         // for (auto l : cornerPixel2)
//         // {
//         //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // }
//         // // for (auto l : cornerPixel3)
//         // // {
//         // //     cv::circle(tmpShow, l, 3, cv::Scalar(0, 255, 0));
//         // // }
//         // string windowName2 = "findSquare3 ";
//         // cv::namedWindow(windowName2, cv::WINDOW_NORMAL);
//         // cv::resizeWindow(windowName2, cvWindow::base * cvWindow::xRatio, cvWindow::base * cvWindow::yRatio);
//         // cv::imshow(windowName2, tmpShow);

//         // cornerPixel.insert(cornerPixel.end(), cornerPixel2.begin(), cornerPixel2.end());

//         // cornerWorld.push_back(cv::Point(2200, -3450));
//         // cornerWorld.push_back(cv::Point(1800, -3450));
//         // cornerWorld.push_back(cv::Point(2200, -3850));
//         // cornerWorld.push_back(cv::Point(1800, -3850));

//         // cornerPixel.insert(cornerPixel.end(), cornerPixel3.begin(), cornerPixel3.end());
//         // cornerWorld.push_back(cv::Point(-1800, -3450));
//         // cornerWorld.push_back(cv::Point(-2200, -3450));
//         // cornerWorld.push_back(cv::Point(-1800, -3850));
//         // cornerWorld.push_back(cv::Point(-2200, -3850));
//     }

// #endif

//     image.camera->initCameraExtrinsic(cornerPixel, cornerWorld);

//     shared_ptr<cv::Mat> uv = generateMap(&image);
//     cv::Mat BIRD(uv->rows, uv->cols, CV_8UC3);
//     cv::Mat uvDecimal(uv->rows, uv->cols, CV_8UC4);
//     cv::Mat uvInteger(uv->rows, uv->cols, CV_8UC4);
//     // 这里的uv和cvImageData都是翻转过的
//     for (int i = 0; i < uv->rows; i++)
//     {
//         for (int j = 0; j < uv->cols; j++)
//         {

//             // 生成的源
//             cv::Vec4f index = uv->at<cv::Vec4f>(i, j);
//             int x = index[2];
//             int y = index[1];
//             int alpha = index[3];
//             // 生成的小数源
//             int minx = (float)(index[2] - x) * 255.0f;
//             int miny = (float)(index[1] - y) * 255.0f;

//             uvInteger.at<cv::Vec4b>(i, j)[0] = 0;
//             uvInteger.at<cv::Vec4b>(i, j)[1] = y;
//             uvInteger.at<cv::Vec4b>(i, j)[2] = x;
//             uvInteger.at<cv::Vec4b>(i, j)[3] = alpha;

//             uvDecimal.at<cv::Vec4b>(i, j)[0] = 0;
//             uvDecimal.at<cv::Vec4b>(i, j)[1] = miny;
//             uvDecimal.at<cv::Vec4b>(i, j)[2] = minx;
//             uvDecimal.at<cv::Vec4b>(i, j)[3] = alpha;

//             float rx = x + (float)minx / 255;
//             float ry = y + (float)miny / 255;

//             int xIndex = rx / 255 * imageWidth;
//             int yIndex = ry / 255 * imageHeight;
//             if(imageType==IMAGE_LEFT||imageType==IMAGE_RIGHT)
//             {
//                 //限制区域
//                 if(j>150&&imageType==IMAGE_LEFT)
//                 {
//                     BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
//                 }
//                 else if(j<150&&imageType==IMAGE_RIGHT)
//                 {
//                     BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
//                 }
//                 else
//                 {
//                     if(i>180&&i<320)
//                     {
//                         BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
//                         BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
//                         BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
//                     }

//                     else
//                     {
//                         if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
//                         {
//                             int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
//                             int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
//                             int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

//                             BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
//                             BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
//                             BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
//                         }
//                         else
//                         {
//                             BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
//                             BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
//                             BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
//                         }
//                     }
//                 }

//             }

//         }
//     }
//     //重新寻找角点
// #if 1
//     if(imageType==IMAGE_LEFT||imageType==IMAGE_RIGHT)
//     {
//         vector<cv::Point2f> reCorner1;
//         vector<cv::Point2f> reCorner2;
//         CalibrationTool::findCorner(getCvShared(BIRD), reCorner1, cv::Size(2, 2));
//         shared_ptr<cv::Mat> BIRD_Mask = MaskTool::putCornerMask(getCvShared(BIRD), reCorner1);
//         CalibrationTool::findCorner(BIRD_Mask, reCorner2,cv::Size(2,2));

//         vector<cv::Point2f> reCornerFront;
//         vector<cv::Point2f> reCornerBehind;
//         if(reCorner1[0].y<reCorner2[0].y)
//         {
//             reCornerFront=reCorner1;
//             reCornerBehind=reCorner2;
//         }
//         else
//         {
//             reCornerFront=reCorner2;
//             reCornerBehind=reCorner1;
//         }
//         vector<cv::Point2f> cornerResultFront;
//         vector<cv::Point2f> cornerResultBehind;

//         for(auto i:reCornerFront)
//         {
//             int m=i.y;
//             int n=i.x;
//             // 生成的源
//             cv::Vec4f index = uv->at<cv::Vec4f>(m, n);
//             int x = index[2];
//             int y = index[1];
//             int alpha = index[3];
//             // 生成的小数源
//             int minx = (float)(index[2] - x) * 255.0f;
//             int miny = (float)(index[1] - y) * 255.0f;
//             float rx = x + (float)minx / 255;
//             float ry = y + (float)miny / 255;
//             int xIndex = rx / 255 * imageWidth;
//             int yIndex = ry / 255 * imageHeight;
//             cornerResultFront.push_back(cv::Point2f(xIndex,480-yIndex));
//         }

//         for(auto i:reCornerBehind)
//         {
//             int m = i.y;
//             int n = i.x;
//             // 生成的源
//             cv::Vec4f index = uv->at<cv::Vec4f>(m, n);
//             int x = index[2];
//             int y = index[1];
//             int alpha = index[3];
//             // 生成的小数源
//             int minx = (float)(index[2] - x) * 255.0f;
//             int miny = (float)(index[1] - y) * 255.0f;
//             float rx = x + (float)minx / 255;
//             float ry = y + (float)miny / 255;
//             int xIndex = rx / 255 * imageWidth;
//             int yIndex = ry / 255 * imageHeight;
//             cornerResultBehind.push_back(cv::Point2f(xIndex, 480 - yIndex));
//         }
//         sortCorner(cornerResultFront,2);
//         sortCorner(cornerResultBehind,2);
//         if(imageType==IMAGE_LEFT)
//         {
//             cornerPixel.insert(cornerPixel.end(),cornerResultFront.begin(),cornerResultFront.end());
//             cornerPixel.insert(cornerPixel.end(),cornerResultBehind.begin(),cornerResultBehind.end());

//             // 左上角四个标定布的角点
//             cornerWorld.push_back(cv::Point(-1800, 3450));
//             cornerWorld.push_back(cv::Point(-1800, 3850));
//             cornerWorld.push_back(cv::Point(-2200, 3450));
//             cornerWorld.push_back(cv::Point(-2200, 3850));
//             //左下角四个标定布的角点
//             cornerWorld.push_back(cv::Point(-1800, -3850));
//             cornerWorld.push_back(cv::Point(-1800, -3450));
//             cornerWorld.push_back(cv::Point(-2200, -3850));
//             cornerWorld.push_back(cv::Point(-2200, -3450));
//         }
//         else if(imageType==IMAGE_RIGHT)
//         {
//             drawPoints(getCvShared(BIRD),reCornerFront,"birdViewPoint3");
//             drawPoints(getCvShared(BIRD), reCornerBehind, "birdViewPoint4");
//             cornerPixel.insert(cornerPixel.end(),cornerResultFront.begin(),cornerResultFront.end());
//             cornerPixel.insert(cornerPixel.end(),cornerResultBehind.begin(),cornerResultBehind.end());

//             //右上角
//             cornerWorld.push_back(cv::Point(1800, 3850));
//             cornerWorld.push_back(cv::Point(1800, 3450));
//             cornerWorld.push_back(cv::Point(2200, 3850));
//             cornerWorld.push_back(cv::Point(2200, 3450));

//             //右下角
//             cornerWorld.push_back(cv::Point(1800, -3450));
//             cornerWorld.push_back(cv::Point(1800, -3850));
//             cornerWorld.push_back(cv::Point(2200, -3450));
//             cornerWorld.push_back(cv::Point(2200, -3850));
//         }
//         image.camera->initCameraExtrinsic(cornerPixel, cornerWorld);

//         uv = generateMap(&image);
//         // 这里的uv和cvImageData都是翻转过的
//         for (int i = 0; i < uv->rows; i++)
//         {
//             for (int j = 0; j < uv->cols; j++)
//             {
//                 // 生成的源
//                 cv::Vec4f index = uv->at<cv::Vec4f>(i, j);
//                 int x = index[2];
//                 int y = index[1];
//                 int alpha = index[3];
//                 // 生成的小数源
//                 int minx = (float)(index[2] - x) * 255.0f;
//                 int miny = (float)(index[1] - y) * 255.0f;
//                 uvInteger.at<cv::Vec4b>(i, j)[0] = 0;
//                 uvInteger.at<cv::Vec4b>(i, j)[1] = y;
//                 uvInteger.at<cv::Vec4b>(i, j)[2] = x;
//                 uvInteger.at<cv::Vec4b>(i, j)[3] = alpha;
//                 uvDecimal.at<cv::Vec4b>(i, j)[0] = 0;
//                 uvDecimal.at<cv::Vec4b>(i, j)[1] = miny;
//                 uvDecimal.at<cv::Vec4b>(i, j)[2] = minx;
//                 uvDecimal.at<cv::Vec4b>(i, j)[3] = alpha;
//                 float rx = x + (float)minx / 255;
//                 float ry = y + (float)miny / 255;
//                 int xIndex = rx / 255 * imageWidth;
//                 int yIndex = ry / 255 * imageHeight;
//                 if (xIndex < imageWidth && xIndex > 0 && yIndex < imageHeight && yIndex > 0)
//                 {
//                     int b = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[0];
//                     int g = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[1];
//                     int r = cvImageData->at<cv::Vec3b>(yIndex, xIndex)[2];

//                     BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(b);
//                     BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(g);
//                     BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(r);
//                 }
//                 else
//                 {
//                     BIRD.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(0);
//                     BIRD.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(0);
//                 }
//             }
//         }
//     }

// #endif

//     cv::imwrite(storePath+ fileName + ".png", BIRD);
//     cv::imwrite(storePath + fileName + "_uv.png", uvInteger);
//     cv::imwrite(storePath + fileName + "_uv_decimal.png", uvDecimal);
// }
int main()
{

    string realCarPath0 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src0.png";
    string realCarPath1 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src1.png";
    string realCarPath2 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src2.png";
    string realCarPath3 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/src3.png";

    noCvInfoLog();
    shared_ptr<cv::Mat> src0 = getCvShared(cv::imread(realCarPath0));
    shared_ptr<cv::Mat> src1 = getCvShared(cv::imread(realCarPath1));
    shared_ptr<cv::Mat> src2 = getCvShared(cv::imread(realCarPath2));
    shared_ptr<cv::Mat> src3 = getCvShared(cv::imread(realCarPath3));

    float focalLength=0.985;
    float cameraAngle_3[1041];
    float cameraAngle_5[1041];
    float cameraAngle_7[1041];
    float cameraAngle_9[1041];
    for(int i=0;i<1041;i++)
    {
        cameraAngle[i]=cameraAngle[i]/180*PI;
        resHeight[i]/=focalLength;
        cameraAngle_3[i]=pow(cameraAngle[i],3);
        cameraAngle_5[i]=pow(cameraAngle[i],5);
        cameraAngle_7[i]=pow(cameraAngle[i],7);
        cameraAngle_9[i]=pow(cameraAngle[i],9);
    }
    //角度矩阵
    cv::Mat A(1041,4,CV_32FC1);
    //像高矩阵
    cv::Mat B(1041,1,CV_32FC1);
    //系数矩阵
    cv::Mat X(4,1,CV_32FC1);
    
    for(int i=0;i<A.rows;i++)
    {
        A.at<float>(i,0)=cameraAngle_3[i];
        A.at<float>(i,1)=cameraAngle_5[i];
        A.at<float>(i,2)=cameraAngle_7[i];
        A.at<float>(i,3)=cameraAngle_9[i];
    }
    for (int i = 0; i < B.rows; i++)
    {
        B.at<float>(i, 0) = resHeight[i]-cameraAngle[i];
    }
    cv::solve(A,B,X,cv::DECOMP_SVD);
    float K0=X.at<float>(0,0);
    float K1=X.at<float>(1,0);
    float K2=X.at<float>(2,0);
    float K3=X.at<float>(3,0);

    std::cout<<X<<std::endl;
    Camera rightCamera(200, 181, 360, 240,
                  K0,
                  K1,
                  K2,
                  K3);

    Camera rearCamera(200, 181, 360, 240,
                  K0,
                  K1,
                  K2,
                  K3);

    Camera frontCamera(200, 181,360, 240,
                  K0,
                  K1,
                  K2,
                  K3);

    Camera leftCamea(200, 181, 360, 240,
                  K0,
                  K1,
                  K2,
                  K3);
    shared_ptr<Image> frontImage=std::make_shared<Image>(IMAGE_FRONT);
    shared_ptr<Image> rightImage=std::make_shared<Image>(IMAGE_RIGHT);
    shared_ptr<Image> rearImage=std::make_shared<Image>(IMAGE_REAR);
    shared_ptr<Image> leftImage=std::make_shared<Image>(IMAGE_LEFT);


    frontImage->addCamera(frontCamera);
    rightImage->addCamera(rightCamera);
    rearImage->addCamera(rearCamera);
    leftImage->addCamera(leftCamea);

    Image::initCarPositionPARA("hello");
    auto calibrate=[](shared_ptr<Image>& image,shared_ptr<cv::Mat>& src,string path)
    {
        image->autoCalibration(src,path);
        // cv::waitKey(0);

    };
    std::thread thread1(calibrate,std::ref(frontImage),std::ref(src0),string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    std::thread thread2(calibrate,std::ref(rightImage),std::ref(src1),string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    std::thread thread3(calibrate,std::ref(rearImage),std::ref(src2),string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    std::thread thread4(calibrate,std::ref(leftImage),std::ref(src3),string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();


    // generateMapTmp(src0,frontImage,IMAGE_FRONT,cv::Size(3,4),cv::Point(0, 3150),300,string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    // generateMapTmp(src1,rightImage,IMAGE_RIGHT,cv::Size(3,4), cv::Point(1350, 0),300,string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    // generateMapTmp(src2,rearImage,IMAGE_REAR,cv::Size(3,4),cv::Point(0, -3150),300,string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");
    // generateMapTmp(src3,leftImage,IMAGE_LEFT,cv::Size(3,4),cv::Point(-1350, 0),300,string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/");



















    // file Path
    string mapPath0 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/front_uv.png";
    string mapDecimalPath0 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/front_uv_decimal.png";

    string mapPath1 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/right_uv.png";
    string mapDecimalPath1 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/right_uv_decimal.png";

    string mapPath2 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/rear_uv.png";
    string mapDecimalPath2 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/rear_uv_decimal.png";

    string mapPath3 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/left_uv.png";
    string mapDecimalPath3 = string(CMAKE_SOURCE_DIR) + "/set/input/putuoshan/left_uv_decimal.png";

    string maksPath = string(CMAKE_SOURCE_DIR) + "/set/input/realCar/noPoint/mask.png";

    string shaderPath = string(CMAKE_SOURCE_DIR) + "/set/shader/plane.Shader";
    string birdViewShaderPath = string(CMAKE_SOURCE_DIR) + "/set/shader/birdView2D.shader";
    noCvInfoLog();
    GLFWwindow *windows = Window::getInstance().getWindows();

    // shader load
    ShaderDispatch::getInstance().addShader("plane", shaderPath);
    unsigned int planeProgram = ShaderDispatch::getInstance().getShader("plane");

    ShaderDispatch::getInstance().addShader("birdView2D", birdViewShaderPath);
    unsigned int birdViewProgram = ShaderDispatch::getInstance().getShader("birdView2D");

    // texture load
    //front
    TextureDispatch::getInstance().addTexture("src0", realCarPath0);
    unsigned int textureID0 = TextureDispatch::getInstance().getTexture("src0");

    TextureDispatch::getInstance().addTexture("map0", mapPath0);
    unsigned int map2ID0 = TextureDispatch::getInstance().getTexture("map0");

    TextureDispatch::getInstance().addTexture("map0Decimal", mapDecimalPath0);
    unsigned int map2DecimalID0 = TextureDispatch::getInstance().getTexture("map0Decimal");

    //right
    TextureDispatch::getInstance().addTexture("src1", realCarPath1);
    unsigned int textureID1 = TextureDispatch::getInstance().getTexture("src1");

    TextureDispatch::getInstance().addTexture("map1", mapPath1);
    unsigned int map2ID1 = TextureDispatch::getInstance().getTexture("map1");

    TextureDispatch::getInstance().addTexture("map1Decimal", mapDecimalPath1);
    unsigned int map2DecimalID1 = TextureDispatch::getInstance().getTexture("map1Decimal");



    //rear
    TextureDispatch::getInstance().addTexture("src2", realCarPath2);
    unsigned int textureID2 = TextureDispatch::getInstance().getTexture("src2");

    TextureDispatch::getInstance().addTexture("map2", mapPath2);
    unsigned int map2ID2 = TextureDispatch::getInstance().getTexture("map2");

    TextureDispatch::getInstance().addTexture("map2Decimal", mapDecimalPath2);
    unsigned int map2DecimalID2 = TextureDispatch::getInstance().getTexture("map2Decimal");

    //left
    TextureDispatch::getInstance().addTexture("src3", realCarPath3);
    unsigned int textureID3= TextureDispatch::getInstance().getTexture("src3");

    TextureDispatch::getInstance().addTexture("map3", mapPath3);
    unsigned int map2ID3 = TextureDispatch::getInstance().getTexture("map3");

    TextureDispatch::getInstance().addTexture("map3Decimal", mapDecimalPath3);
    unsigned int map2DecimalID3 = TextureDispatch::getInstance().getTexture("map3Decimal");


    // vao load
    // float planeVao[] =
    //     {
    //         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    //         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    //         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

    //         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    //         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    //         1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    float planeVao[] =
    {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.f, 0.0f, 0.0f, 1.0f,
             0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f,  1.0f, 0.0f, 1.0f, 1.0f,

            0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };
    unsigned int  planeIndex[]=
    {
        0,1,2,3,
        4,5,6,7
    };
    VaoDispatch::getInstance().addVao("planeVao", planeVao,planeIndex, ArrayTool::getlength(planeVao),ArrayTool::getlength(planeIndex));
    unsigned int vaoID = VaoDispatch::getInstance().getVao("planeVao");

    // render loop
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(windows)) // 渲染循环
    {
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        //front
        glUseProgram(birdViewProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID0);
        glUniform1i(glGetUniformLocation(birdViewProgram, "forntTexture"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, map2ID0);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapFrontTexture"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, map2DecimalID0);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapDecimalFrontTexture"), 2);


        //left
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureID3);
        glUniform1i(glGetUniformLocation(birdViewProgram, "leftTexture"), 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, map2ID3);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapLeftTexture"), 4);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, map2DecimalID3);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapDecimalLeftTexture"), 5);

        //right
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, textureID1);
        glUniform1i(glGetUniformLocation(birdViewProgram, "rightTexture"), 6);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, map2ID1);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapRightTexture"), 7);

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, map2DecimalID1);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapDecimalRightTexture"), 8);


        //rear
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, textureID2);
        glUniform1i(glGetUniformLocation(birdViewProgram, "rearTexture"), 9);

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, map2ID2);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapRearTexture"), 10);

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, map2DecimalID2);
        glUniform1i(glGetUniformLocation(birdViewProgram, "mapDecimalRearTexture"), 11);


        glUniformMatrix4fv(glGetUniformLocation(birdViewProgram, "trans"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glBindVertexArray(vaoID);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLE_STRIP,8,GL_UNSIGNED_INT,0);
        glUseProgram(0);
        glDisable(GL_BLEND);

        // CarExtrinsicCabTool::getInstance().processCalibrate(0);
        // image1->imageEvent();
        glfwSwapBuffers(windows);
        glfwPollEvents();
    }
}
#else

#endif
#endif