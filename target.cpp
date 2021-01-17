#include"target.h"
#include"serrial.h"
#include"Armor.h"
#include<iostream>
#include<opencv2/opencv.hpp>
#include "common.h"
#include"predict.h"
using namespace  cv;
using namespace std;

Target_class::Target_class()
{
    targets.reserve(16);
    enemies.reserve(8);
    enemies.resize(8);//设置9个空位
}

void Target_class::add_from_armors(std::vector<armor_data> a)
{
    for(int i=0;i<(int)a.size();i++)
    {
        if(!enemies[a[i].num].armor1.empty())//Armor 1 not empty
        {
          if(match(a[i],enemies[a[i].num].armor1.back())&&( enemies[a[i].num].flag_armor1==false))//能匹配上1号装甲
            {
               enemies[a[i].num].armor1.push_back(a[i]);
                enemies[a[i].num].flag_armor1=true;//已写入
                continue;
            }
        }

        if(!enemies[a[i].num].armor2.empty())//Armor 2 not empty
        {
          if(match(a[i],enemies[a[i].num].armor2.back())&&( enemies[a[i].num].flag_armor2==false))//能匹配上2号装甲
            {
               enemies[a[i].num].armor2.push_back(a[i]);
                enemies[a[i].num].flag_armor2=true;//已写入
                continue;
            }
        }
//cant match
        if(enemies[a[i].num].armor1.empty())
        {
            enemies[a[i].num].armor1.push_back(a[i]);
             enemies[a[i].num].flag_armor1=true;//已写入
             continue;
        }
        if(enemies[a[i].num].armor2.empty())
        {
            enemies[a[i].num].armor2.push_back(a[i]);
             enemies[a[i].num].flag_armor2=true;//已写入
             continue;
        }


    }
 //   cout<<"1.size:"<<enemies[5].armor1.size()<<endl;
  //  cout<<"2.size:"<<enemies[5].armor2.size()<<endl;
}

void Target_class::add_from_lightbars(std::vector<cv::RotatedRect> & lightbar_v)
{

    for(int i=0;i<(int)enemies.size();i++)
    {
//一号装甲
        if((!enemies[i].armor1.empty())&&(enemies[i].flag_armor1==false))//非空非写入
        {
           for(int j=0;j<(int)(lightbar_v.size())-1;j++)
           {
                if(match(enemies[i].armor1.back().left_lightbar,lightbar_v[j]))
                   {
                    for(int k=j+1;k<lightbar_v.size();k++)
                    {
                        if(match(enemies[i].armor1.back().right_lightbar,lightbar_v[k])&&(enemies[i].flag_armor1==false))//左右灯条匹配
                        {
                     //       cout<<"match from light bars"<<endl;
                     //       cout<<"num:"<<i<<endl;
                             armor_data am;
                             am.num=i;
                             am.left_lightbar=lightbar_v[j];
                             am.right_lightbar=lightbar_v[k];
                             am.center.x = (am.left_lightbar.center.x + am.right_lightbar.center.x) / 2;
                             am.center.y = (am.left_lightbar.center.y + am.right_lightbar.center.y) / 2;
                             enemies[i].armor1.push_back(am);
                              enemies[i].flag_armor1=true;
                             lightbar_v.erase(lightbar_v.begin()+k);
                               lightbar_v.erase(lightbar_v.begin()+j);
                           goto armor2;
                        }
                    }
                }

           }

        }
        armor2:;
        //二号装甲
        if((!enemies[i].armor2.empty())&&(enemies[i].flag_armor2==false))//非空非写入
        {
           for(int j=0;j<(int)(lightbar_v.size())-1;j++)
           {
                if(match(enemies[i].armor2.back().left_lightbar,lightbar_v[j]))
                   {
                    for(int k=j+1;k<lightbar_v.size();k++)
                    {
                        if(match(enemies[i].armor1.back().right_lightbar,lightbar_v[k])&&(enemies[i].flag_armor2==false))//左右灯条匹配
                        {
                          //  cout<<"match from light bars"<<endl;
                        //    cout<<"num:"<<i<<endl;
                             armor_data am;
                             am.num=i;
                             am.left_lightbar=lightbar_v[j];
                             am.right_lightbar=lightbar_v[k];
                             am.center.x = (am.left_lightbar.center.x + am.right_lightbar.center.x) / 2;
                             am.center.y = (am.left_lightbar.center.y + am.right_lightbar.center.y) / 2;
                             enemies[i].armor2.push_back(am);
                              enemies[i].flag_armor2=true;
                             lightbar_v.erase(lightbar_v.begin()+k);
                               lightbar_v.erase(lightbar_v.begin()+j);
                            goto loop;
                        }
                    }
                }

           }

        }
        loop:continue;
    }
}

inline bool Target_class::match(armor_data data_new,armor_data data_ori)
{
    return  true;
    float thr_dis=(data_ori.right_lightbar.center.x-data_ori.left_lightbar.center.x)/4;//基于灯条距离的动态阈值
    if(abs(data_ori.center.x-data_new.center.x)>thr_dis)//数字相同则只需考虑x方向问题,过大则不匹配
    {
            return false;
    }
    return true;
}

inline bool Target_class::match(cv::RotatedRect ori, cv::RotatedRect exm)
{
    float dis_x;
    float dis_y;
    dis_x=ori.center.x-exm.center.x;
    dis_y=ori.center.y-exm.center.y;
//cout<<"match lightbars"<<endl;
    if( (dis_x*dis_x+dis_y*dis_y)>(ori.size.height*ori.size.height/4))//灯条距离过大
    {
        return false;
    }
    float height_scale=abs(ori.size.height/exm.size.height);
    if((height_scale>1.5)||(height_scale<0.66))
    {
        return false;
    }

return true;
}

inline void Target_class::clean_up()
{
    for(int i=0;i<enemies.size();i++)
    {
        if(enemies[i].flag_armor1==false)
        {
            enemies[i].armor1.clear();
        }
        if(enemies[i].flag_armor2==false)
        {
            enemies[i].armor2.clear();
        }

        if( enemies[i].armor1.size()>3)
            enemies[i].armor1.erase(enemies[i].armor1.begin());

        if( enemies[i].armor2.size()>3)
            enemies[i].armor2.erase(enemies[i].armor2.begin());

        enemies[i].flag_armor1=false;
        enemies[i].flag_armor2=false;
    }
}

void Target_class::push_data(std::vector<armor_data>armor_v,std::vector<cv::RotatedRect>&rec_v)
{

        add_from_armors(armor_v);
       add_from_lightbars(rec_v);
        targets.clear();
        clean_up();

}

void Target_class::updata_data(float pos_yaw,float pos_pitch)
{
    for(int i=0;i<enemies.size();i++)
    {
         float para;
        if(!enemies[i].armor1.empty())
        {
            cal_distance(enemies[i].armor1.back(),pos_pitch);
            enemies[i].armor1.back().pitch_compensation=v*v/G/(enemies[i].armor1.back().distance/100)-sqrt(pow(v,4)/G/G/(enemies[i].armor1.back().distance/100)/(enemies[i].armor1.back().distance/100)-1);
            int y=Angle_Parament*enemies[i].armor1.back().pitch_compensation;
            cout<<"y:"<<y<<endl;
            cout<<"pitch_compensation:"<<enemies[i].armor1.back().pitch_compensation<<endl;
           enemies[i].armor1.back().yaw=atan((enemies[i].armor1.back().center.x-Half_Img_Size_Width)/Angle_Parament)*(-57.32);

            enemies[i].armor1.back().yaw+=pos_yaw;
            enemies[i].armor1.back().pitch=atan((enemies[i].armor1.back().center.y-Half_Img_Size_Height-y)/Angle_Parament)*57.32+pos_pitch;
            targets.push_back(enemies[i].armor1.back());
            //cal_distance(targets.back(),pos_pitch);
            //targets.back().pitch_compensation=atan(v*v/G/(targets.back().distance/100)-sqrtf(pow(v,4)/G/G/(targets.back().distance/100)/(targets.back().distance/100)-1))*57.32;

           targets.back().yaw=Linear_predict(enemies[i].armor1,targets.back().distance*Time_Parament);
        }
        if(!enemies[i].armor2.empty())
        {
            cal_distance(enemies[i].armor2.back(),pos_pitch);
            enemies[i].armor2.back().pitch_compensation=v*v/G/(enemies[i].armor2.back().distance/100)-sqrtf(pow(v,4)/G/G/(enemies[i].armor2.back().distance/100)/(enemies[i].armor2.back().distance/100)-1);
            int y=Angle_Parament*enemies[i].armor2.back().pitch_compensation;
            cout<<"y:"<<y<<endl;
          enemies[i].armor2.back().yaw=atan((enemies[i].armor2.back().center.x-Half_Img_Size_Width)/Angle_Parament)*(-57.32);

            enemies[i].armor2.back().yaw+=pos_yaw;
            enemies[i].armor2.back().pitch=atan((enemies[i].armor2.back().center.y-Half_Img_Size_Height-y)/Angle_Parament)*57.32+pos_pitch;
            targets.push_back(enemies[i].armor2.back());
            //cal_distance(targets.back(),pos_pitch);
            //targets.back().pitch_compensation=atan(v*v/G/(targets.back().distance/100)-sqrtf(pow(v,4)/G/G/(targets.back().distance/100)/(targets.back().distance/100)-1))*57.32;

            targets.back().yaw=Linear_predict(enemies[i].armor2,targets.back().distance*Time_Parament);

        }
    }
}
inline void Target_class::cal_distance(armor_data &armor,float pos_pitch)
{

        float para;
        if(armor.num==Base_ID)
        {
            para=65.0;
        }
        else if (armor.num==Guard_ID) {
            para=-15.0;
        }
        else {
            para=15.0;
        }
        para+=pos_pitch;

        if(armor.left_lightbar.size.height>armor.right_lightbar.size.height)
        {
            float staight_bar=armor.left_lightbar.size.height*cos(armor.left_lightbar.angle/57.32);
            armor.distance=(Distance_Parament*Angle_Parament/cos(para/57.32)/armor.left_lightbar.size.height);
        }
        else {
            float staight_bar=armor.right_lightbar.size.height*cos(armor.right_lightbar.angle/57.32);
            armor.distance=(Distance_Parament*Angle_Parament/cos(para/57.32)/armor.right_lightbar.size.height);
        }
 }

int Target_class::attack()//attack the most valuable target
{
    int index=0;
    for(int i=0;i<targets.size();i++)
    {
      if( ( abs((int)targets[index].center.x-Half_Img_Size_Width)+abs((int)targets[index].center.y-Half_Img_Size_Height))
                >( abs((int)targets[i].center.x-Half_Img_Size_Width)+abs((int)targets[i].center.y-Half_Img_Size_Height)))
      {
          index=i;
      }
    }
    for(int i=0;i<targets.size();i++)
    {
        if((targets[index].num==targets[i].num)&&(i!=index))
        {
            if(abs(targets[index].spin_angle)>abs(targets[i].spin_angle))
            {
                return  index;
            }
            else return i;
        }
    }
    return index;
}
