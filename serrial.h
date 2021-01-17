#ifndef SERRIAL_H
#define SERRIAL_H
#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include<iostream>
#include <vector>
#include<string>
#define PITCH_OFFSET 292.0
class serrial
{
private:
    std::vector<float> buffer_yaw;
     std::vector<float> buffer_pitch;
    int fd;
    uint8_t Data_to_send[8];
    uint8_t Data_read[8];
public:
    float pos_pitch=0.0;
    float pos_yaw=0.0;
   void  serrial_open();
    int send_angle_1(float yaw,float pitch);
    int send_angle_2();
     int get_pos();
    void lock_date();
     static void *pth(void *_this)
     {
         serrial *__this=(serrial *)_this;
       __this-> get_pos();
     }
};
#endif // SERRIAL_H
