#ifndef AUTO_ATTACK
#define AUTO_ATTACK
#include"Armor.h"
#include<iostream>
#include<opencv2/opencv.hpp>


#define Guard_ID 6//哨兵
#define Base_ID 7//基地


typedef  struct
{
    //每辆车最多检测两块装甲
    std::vector<armor_data> armor1;
     std::vector<armor_data> armor2;
     //计数清除装甲
     bool flag_armor1=false;
     bool flag_armor2=false;
}enemy;

class Target_class{
private:
    std::vector<enemy> enemies;//敌人合集

public:
    Target_class();
        std::vector<armor_data> targets;//预测后的装甲
    void push_data(std::vector<armor_data>armor_v,std::vector<cv::RotatedRect>&rec_v);
    int attack();//attack the most valuable target
    void updata_data(float pos_yaw,float pos_pitch);//更新目标数据
private:
    inline void cal_distance(armor_data &armor,float pos_pitch);
   inline void clean_up();//扫尾工作
   inline  void add_from_armors(std::vector<armor_data> a);
   inline  void add_from_lightbars(std::vector<cv::RotatedRect>&);

   inline bool match(cv::RotatedRect ori,cv::RotatedRect exm);//灯条匹配
   inline bool match(armor_data ori,armor_data exm);//装甲板匹配
};

#endif // AUTO_ATTACK
