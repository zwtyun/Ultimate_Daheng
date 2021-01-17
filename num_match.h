#ifndef NUM_MATCH_H
#define NUM_MATCH_H
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#define Module_Img_Size cv::Size(50,50)//模板图像大小
#define Total_Point 2500.0              //总点数
#define good_match_thr 0.750            //相似比例阈值

    class num_match
    {
    private:
        std::vector<cv::Mat> module_imgs;//数字模板
        cv::Mat num;//图片切割后的图像
        std::vector<cv::Point2f> dst_corner;
        float match_possibility[9];
    private:
        inline bool img_cut(cv::Mat &gray);//将输入图像剪切只剩数字并存于num中
        inline void cal_possibility(cv::Mat roi);//计算相同率（结果输入match_possibility,越接近1越相似
    public:
        num_match();//初始化
        int match(cv::RotatedRect , cv::RotatedRect , cv::Mat Gray_Img);//总接口

    };




#endif // !NUM_MATCH_H
