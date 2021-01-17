#include"Armor.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include"common.h"
#include"num_match.h"
using namespace std;
using namespace cv;

Armor::Armor()
{
    armors.reserve(20);
}

void Armor::Match_Lightbars(vector<RotatedRect>&Lightbars,Mat input_Img)
{
    Mat gray;

    cvtColor(input_Img, gray, COLOR_BGR2GRAY);
    armors.clear();

    Point2f d[4];

            for (int i = 0; i < (int)Lightbars.size() - 1; i++)
            {
                for (int j = i + 1; j < Lightbars.size(); j++)
                {
                    float t = ((Lightbars[j].center.x - Lightbars[i].center.x)/abs(Lightbars[i].center.y - Lightbars[j].center.y)  );

                    if ((abs(Lightbars[i].angle - Lightbars[j].angle) > 30.0)//两灯条角度相差过大
                        || (t < (tan(65.0 / 180.0*PI)))//两灯条中心连线角度过大
                        || ((Lightbars[j].center.x - Lightbars[i].center.x) > (5 * ((Lightbars[i].size.height > Lightbars[j].size.height) ? Lightbars[i].size.height : Lightbars[j].size.height)))//中心距离之比过大
                        )
                    {
                        continue;
                    }

                    if ((number=num_matcher.match(Lightbars[i], Lightbars[j], gray)) > -1)//有数字
                    {
                      //  cout<<"num:"<<number<<endl;
                        armor_data am;
                        am.num = number;
                        am.center.x = (Lightbars[i].center.x + Lightbars[j].center.x) / 2;
                        am.center.y = (Lightbars[i].center.y + Lightbars[j].center.y) / 2;
                       am.left_lightbar=Lightbars[i];
                       am.right_lightbar=Lightbars[j];
                        armors.push_back(am);
                        Lightbars.erase(Lightbars.begin() + j);
                        Lightbars.erase(Lightbars.begin() + i);
                        i--;
                        break;
                    }
                }


        }
}
