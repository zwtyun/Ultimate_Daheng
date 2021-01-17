#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>

#include"Lightbar.h"
#include"Armor.h"
#include"cam.h"
#include"target.h"
#include"serrial.h"
#include"common.h"
using namespace cv;
using namespace std;


int main()
{
    int state=GXInitLib();


    float end_yaw=0;
    float end_pitch=0;
/***************??????*****************/
int count=0;
    Lightbar lightbars;
    Armor armor;
   // VideoCapture cap(0);
  //  VideoCapture cap(3);
   cam cap;
   Target_class Targets;
    serrial usb;
   usb.serrial_open();
    float p=292.0;
    float y=208.0;
   // Mat origin;
  //  Mat origin=imread("/home/saber/图片/test.png");
    while(1)
    {

       cap.grap_an_img();

       usb.get_pos();
       //cap>>origin;
      // resize(origin,origin,Size(640,480));
       lightbars.Detect(cap.origin,BLUE);
        armor.Match_Lightbars(lightbars.LightBars,cap.origin);
        Targets.push_data(armor.armors,lightbars.LightBars);
        Targets.updata_data(0.0,0.0);
       Targets.updata_data(usb.pos_yaw,usb.pos_pitch);

        if(!Targets.targets.empty())
        {

                count=0;

                float temp=Targets.targets[0].yaw-usb.pos_yaw;
                float k=abs(temp);

                if(k<10)
                {
                              end_yaw =temp*0.7+usb.pos_yaw;
                }
                else {
                    end_yaw=temp*0.9+usb.pos_yaw;
                }
//pitch
                                 temp=Targets.targets[0].pitch-usb.pos_pitch;
                                 k=abs(temp);
                                if(k<10)
                                {
                                              end_pitch =temp*0.7+usb.pos_pitch;

                                }
                                else
                                {
                                    end_pitch=temp+usb.pos_pitch;
                                }
               cout<<"end_yaw "<<end_yaw<<"end_pitch "<<end_pitch<<endl;
               usb.send_angle_1(end_yaw,end_pitch);
          }
        else
        {
            usb.send_angle_2();
        }

        Mat to_show=cap.origin.clone();
        for(int i=0;i<Targets.targets.size();i++)
        {

            Point temp=Targets.targets[i].center;
            circle(to_show,temp,10,Scalar(0,255,0),10);

        }

        for(int i=0;i<Targets.targets.size();i++)
        {

           cout<<"distance:"<<Targets.targets[i].distance<<endl;
        }
        imshow("t",to_show);
        count++;
        waitKey(1);
    }
}
