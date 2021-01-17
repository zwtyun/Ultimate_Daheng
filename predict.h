#ifndef PREDICT_H
#define PREDICT_H
#include<iostream>
#include<vector>
#include"Armor.h"
float Linear_predict(std::vector<armor_data> data,float parament)
{
//std::cout<<"begin predict"<<std::endl;
            if(data.size()<3)
            {
                return data.back().yaw;
            }
            float b=0.0;
            float a=0.0;
           float ave_x=0.0,ave_y=0.0;
            float temp=0.0;
            for(int i=0;i<data.size();i++)
            {
                ave_x+=i;
                ave_y+=data[i].yaw;
                b+=i*data[i].yaw;
                temp+=i*i;
            }
            ave_x/=data.size();
            ave_y/=data.size();

        b-=data.size()*ave_x*ave_y;
        temp-=data.size()*ave_x*ave_x;
        b/=temp;
a=ave_y-b*ave_x;
float answer= (parament+data.size())*b+a;
if((abs(answer-data.back().yaw)>10.0))
{
    std::cout<<"too large"<<std::endl;
    return data.back().yaw;

}
//std::cout<<"predict"<<std::endl;
return answer;
}
#endif // PREDICT_H
