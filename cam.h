#ifndef CAM_H
#define CAM_H
#include<iostream>
#include<opencv2/opencv.hpp>
#include"GxIAPI.h"
#include<DxImageProc.h>
#include"Lightbar.h"
#include"Armor.h"
class cam
{
private:
    GX_STATUS status;
    GX_DEV_HANDLE hDevice = nullptr;
    GX_OPEN_PARAM stOpenParam;
    uint32_t nDeviceNum = 0;
    int64_t nPayLoadSize;
    GX_FRAME_DATA stFrameData;
   uchar *RGB_buffer;
    int64_t pixcolorfilter;
   int64_t height;
   int64_t width;
public :
    cv::Mat origin;

public:
    cam();
    ~cam();
    int grap_an_img();
};

#endif // CAM_H
