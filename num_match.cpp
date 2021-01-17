#include "num_match.h"
#include<opencv2/opencv.hpp>
#include"common.h"
using namespace cv;
using namespace std;

num_match::num_match()
{
    //模板载入
    module_imgs.clear();
    module_imgs.reserve(9);
    int k = 0;
    Mat img;
    for (k = 0; k < 8; k++)
    {
        string ImagePath = "/home/star/Ultimate111Daheng/Ultimate/StandardNumber/" + to_string(k+1) + ".bmp";
        img = imread(ImagePath, IMREAD_GRAYSCALE);
        if (img.empty())
        {
            cout << "Loading Number Picture fail";
            exit(1);
        }

        resize(img, img, Module_Img_Size,0,0, INTER_NEAREST);
        threshold(img, img, 0, 255, THRESH_OTSU);//灰度图像二值化
        module_imgs.push_back(img);
    }
    //目标锚点设置
    dst_corner.push_back(Point2f(0, 0));
    dst_corner.push_back(Point2f(0, 50));
    dst_corner.push_back(Point2f(50, 50));
    dst_corner.push_back(Point2f(50, 0));
}


int num_match::match(cv::RotatedRect lightsrc_corner1, cv::RotatedRect lightsrc_corner2,cv::Mat Gray_Img)
{

    //确定左右边
    RotatedRect L, R;
    if (lightsrc_corner1.center.x < lightsrc_corner2.center.x)
    {
        L = lightsrc_corner1;
        R = lightsrc_corner2;
    }
    else
    {
        R = lightsrc_corner1;
        L = lightsrc_corner2;
    }
    //找出大致的区域以减少运算
    L.size.height*=2.0;
      R.size.height*=2.0;

    Point2f temp[4];
    vector<Point2f> src_corner;
    L.points(temp);
    for (int i = 0; i < 4; i++)
    {
        src_corner.push_back(temp[i]);
    }
    R.points(temp);
    for (int i = 0; i < 4; i++)
    {
        src_corner.push_back(temp[i]);
    }
    src_corner.erase(src_corner.begin() + 7);
    src_corner.erase(src_corner.begin() + 6);
    src_corner.erase(src_corner.begin() + 1);
    src_corner.erase(src_corner.begin());
    for(int i=0;i<src_corner.size();i++)
    {
        if(src_corner[i].x<0)
        {
            src_corner[i].x=0;
        }
        if(src_corner[i].x>Gray_Img.cols-1)
        {
            src_corner[i].x=Gray_Img.cols-1;
        }
        if(src_corner[i].y<0)
        {
            src_corner[i].y=0;
        }
        if(src_corner[i].y>Gray_Img.rows-1)
        {
            src_corner[i].y=Gray_Img.rows-1;
        }
    }
    Rect rec = boundingRect(src_corner);

    //防越界
    Mat roi = Gray_Img(rec);
    if((roi.cols<5)||(roi.rows<5))
    {
        return -1;
    }

    for (int i = 0; i < 4; i++)
    {
        src_corner[i].x -= rec.tl().x;
        src_corner[i].y -= rec.tl().y;
    }

    Mat facing_roi;
    Mat warpMatrix = getPerspectiveTransform(src_corner, dst_corner);
        warpPerspective(roi, facing_roi, warpMatrix, Module_Img_Size);

    facing_roi = facing_roi(Rect(Point(10, 0), Point(40, 50)));//最后一步消除灯条的影响
    //此处得出数字最小矩形
    if(img_cut(facing_roi)==false)//图形不规整
        return -1;
    //计算相同率
    cal_possibility(facing_roi);
    //透视变换
    int index=0;
    float max_possibility=0.0;

    for (int i = 0; i <8; i++)
    {
        if (match_possibility[i] > max_possibility)
        {
            index = i ;
            max_possibility = match_possibility[i];
        }
    }

    if (max_possibility > good_match_thr)
    {
        return index;
    }
return -1;
}

inline bool num_match::img_cut(Mat &roi)
{
    //  imshow("roi",roi);
    threshold(roi, roi, 0, 255, THRESH_OTSU);
    Point tl, br;//左上角，右上角
    //左边沿
    for (int i = 0; i < roi.cols - 2; i++)
    {
        for (int j = 0; j < roi.rows - 1; j++)
        {
            if (roi.at<uchar>(j, i) > 0)
            {
                tl.x = i;
                i = roi.cols;
                break;
            }
        }
    }
    //右边沿
    for (int i = roi.cols - 1; i > tl.x; i--)
    {
        for (int j = 0; j < roi.rows - 1; j++)
        {
            if (roi.at<uchar>(j, i) > 0)
            {
                br.x = i;
                i = tl.x;
                break;
            }
        }
    }
    //上边沿

    for (int i = 0; i < roi.rows - 1; i++)
    {
        for (int j = tl.x; j <= br.x; j++)
        {
            if (roi.at<uchar>(i, j) > 0)
            {
                tl.y = i;
                i = roi.rows;
                break;
            }
        }
    }
    //下边沿
    for (int i = roi.rows - 1; i>tl.y; i--)
    {
        for (int j = tl.x; j <= br.x; j++)
        {
            if (roi.at<uchar>(i, j) > 0)
            {
                br.y = i;
                i = tl.y;
                break;
            }

        }

    }

    roi = roi(Rect(tl, br));
      //imshow("ro1i",roi);
    if((roi.cols<10)||(roi.rows<20)||(roi.cols>40))
    {
       return false;
    }

    return true;
}

inline void num_match::cal_possibility(Mat roi)
{
    Mat temp;
    uint16_t count;//相同位数统计

    resize(roi, roi, Module_Img_Size,0,0, INTER_NEAREST);
    for (int i = 0; i < module_imgs.size(); i++)
    {
        count = 0;
        bitwise_xor(roi, module_imgs[i], temp);
        //统计相同像素数
        for (int j = 0; j < roi.cols-1; j++)
        {
            for (int k = 0; k < roi.rows; k++)
            {
                if (temp.at<uchar>(k, j) == 0)
                    count++;
            }
        }
        match_possibility[i] = (float)count / Total_Point;
    }

}
