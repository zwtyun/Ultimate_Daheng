#ifndef ARMOR_H
#define ARMOR_H
#include<iostream>
#include<opencv2/opencv.hpp>
#include"num_match.h"
typedef struct{				//装甲板信息类
    float distance;//距离,cm
    float spin_angle;
    float pitch;
    float yaw;
    int num;		//数字
    cv::Point center;//中心点
    cv::RotatedRect left_lightbar;
    cv::RotatedRect right_lightbar;
    float pitch_compensation;//tan
}armor_data;//装甲板信息

class Armor {
public:
    Armor();
    void Match_Lightbars(std::vector<cv::RotatedRect>&,cv::Mat Input_Img);//主接口
public:
    std::vector<armor_data> armors;//每个元素为一个装甲板描述
private:
        int number;
    num_match num_matcher;

};
#endif // !AMOR_H
