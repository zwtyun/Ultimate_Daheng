#ifndef LIGHTBAR_H
#define LIGHTBAR_H
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#define MaxDis 3		//连通区域最大阈值
#define Max_Angle 15	//连通区域最大偏差角
#define MixArea 6		//连通区域最小面积
#define BLUE true
#define RED false
/*************************************/

class Lightbar
    {
        private	:
            cv::Mat kernel;//用来形态学滤波的核
            cv::Mat mask;//灯条二值图

        public:
            std::vector<cv::RotatedRect> LightBars;//每个灯条就是一个旋转矩阵
        public:
            Lightbar();
            void Detect(cv::Mat Input_Img,bool color);//程序主接口
    };


#endif // LIGHTBAR_H
