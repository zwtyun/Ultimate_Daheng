#include"Lightbar.h"
#include<math.h>
#include"cam.h"
#include"common.h"

using namespace cv;
using namespace std;

Lightbar::Lightbar()
{
    LightBars.reserve(100);
    kernel= getStructuringElement(MORPH_ELLIPSE, Size(1, 2));
}

void Lightbar::Detect(Mat Input_Img,bool color)//????BGR???
{
    Mat temp_mat;
    cvtColor(Input_Img, temp_mat, COLOR_BGR2HSV);
    inRange(temp_mat, Scalar(0, 0, 240), Scalar(255, 255, 255), mask);
    //inRange(Input_Img, Scalar(0, 0, 150), Scalar(100, 100, 255),mask);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);//???????
    vector<vector<Point> > contours;
    vector<Vec4i> hierarcy;
    findContours(mask, contours, hierarcy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    LightBars.clear();
    LightBars.resize(contours.size()); //??????С??????μ???
    Point2f rect[4];
    float temp;
    /***************************???????*********************************/
    for (int i = 0; i < contours.size(); i++)
    {

        LightBars.push_back(minAreaRect(contours[i]));  //?????????????С???????
        if ((LightBars.back().size.height < 2) || (LightBars.back().size.width <2))
        {
            //contours.erase(contours.begin()+i);
            LightBars.pop_back();
        //	i--;
            continue;
        }
        if (LightBars.back().size.width > LightBars.back().size.height)
        {
            float temp;
            temp = LightBars.back().size.width;
            LightBars.back().size.width = LightBars.back().size.height;
            LightBars.back().size.height= temp;
            LightBars.back().angle += 90.0;
        }

    }

    for (unsigned int i = 0; i < LightBars.size(); i++)
    {
        if (((abs(LightBars[i].angle) > 30) && (LightBars[i].size.area() > 15)) ||(LightBars[i].size.height<3)
            || ((abs(LightBars[i].angle) > 30) && ((LightBars[i].size.height / LightBars[i].size.width) > 1))
            ||( LightBars[i].size.area()<5)
            )
        {
            LightBars.erase(LightBars.begin() + i);
            i--;
        }

    }


    if(!LightBars.empty())
    {
      for (int i = 0; i < LightBars.size()-1; i++)
        {
            for ( int j = i+1; j < LightBars.size(); j++)
            {
                if(LightBars[i].center.x>LightBars[j].center.x)
                {
                    std::swap(LightBars[i],LightBars[j]);
                }
            }
        }
    }
imshow("mask",mask);
}


