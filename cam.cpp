#include"cam.h"
#include<opencv2/opencv.hpp>
#include <unistd.h>
#include "GxIAPI.h"
#include "DxImageProc.h"
#include"Armor.h"
#include<iostream>
#include <unistd.h>
using namespace  std;
using namespace cv;

cam::cam()
{
   cout<<"step:"<<0<<endl;
    status = GXInitLib();
    if (status!= GX_STATUS_SUCCESS)
    {
        cout<<"not found tl "<<status<<endl;
        cout<<"error in init lib"<<endl;
            return ;
    }
//枚 举 设 备 列 表
    status = GXUpdateDeviceList(&nDeviceNum, 1000);
    if ((status!= GX_STATUS_SUCCESS)||(nDeviceNum<= 0))
    {
         cout<<"error in  openning device"<<endl;
         cout<<"device num:"<<nDeviceNum<<endl;
         cout<<status<<endl;
            return ;
    }
    cout<<"step:"<<1<<endl;
    //打 开 设 备
    stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode = GX_OPEN_INDEX;
    stOpenParam.pszContent = "1";
    status = GXOpenDevice(&stOpenParam, &hDevice);
    if(status != GX_STATUS_SUCCESS)
    {
        cout<<"error in openDevice"<<endl;
        exit(1);
    }
      cout<<"step:"<<2<<endl;
    //判断相机是否支持bayer格式
        bool m_bColorFilter;
        status = GXIsImplemented(hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_bColorFilter);
        if (m_bColorFilter)
        {
            status = GXGetEnum(hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &pixcolorfilter);
        }
  cout<<"step:"<<3<<endl;
    //buffer size
    status = GXGetInt(hDevice, GX_INT_PAYLOAD_SIZE, &nPayLoadSize);
    if(status != GX_STATUS_SUCCESS)
    {
        cout<<"error in getpayloadsize"<<endl;
        exit(1);
    }
      cout<<"step:"<<4<<endl;
    if(nPayLoadSize<=0)
    {
        cout<<"error  getpayloadsize"<<endl;
        exit(1);
    }
      cout<<"step:"<<5<<endl;
    stFrameData.pImgBuf= malloc((size_t)nPayLoadSize);
    cout<<"nPayLoadSize:"<<nPayLoadSize<<endl;
    // 获取宽度
    status=GXGetInt(hDevice,GX_INT_WIDTH,&width);
    if (status != GX_STATUS_SUCCESS)
    {
        cout<<"error getting img width"<<endl;
        exit(1);
    }
// 获取高度
    status=GXGetInt(hDevice,GX_INT_HEIGHT,&height);
    if (status != GX_STATUS_SUCCESS)
    {
        cout<<"error getting img width"<<endl;
        exit(1);
    }
    cout<<"step:"<<6<<endl;
RGB_buffer=(uchar *)malloc((height * width * 3)*sizeof(uchar));
  cout<<"step:"<<6<<endl;
    //发送开采命令
    status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_START);
    if (status != GX_STATUS_SUCCESS)
    {
        cout<<"error starting grap img"<<endl;
        exit(1);
    }
    cout<<height <<      "!"<<      width<<endl;
    origin=Mat(Size(width,height),CV_8UC3);
    cout<<"successfully init cam"<<endl;
}
cam::~cam()
{
    //发 送 停 止 采 集 命 令
    status = GXSendCommand(hDevice, GX_COMMAND_ACQUISITION_STOP);
    //释 放 图 像 缓 冲 区 buffer
    free(stFrameData.pImgBuf);
    status = GXCloseDevice(hDevice);
    status = GXCloseLib();
}

int cam::grap_an_img()
{
    GXFlushQueue(hDevice);
    while(GXGetImage(hDevice, &stFrameData, 100) != GX_STATUS_SUCCESS)
    {
        sleep(10);
    }

    //successfully grap

    if (stFrameData.nStatus == GX_FRAME_STATUS_SUCCESS)
    {

       DxRaw8toRGB24(stFrameData.pImgBuf,RGB_buffer,(VxUint32)width,(VxUint32)height,RAW2RGB_NEIGHBOUR,DX_PIXEL_COLOR_FILTER(pixcolorfilter),false);
       memcpy(origin.data,RGB_buffer,width*height*3);
       cvtColor(origin,origin,COLOR_RGB2BGR);

    }

}
