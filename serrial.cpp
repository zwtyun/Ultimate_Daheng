#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include<iostream>
#include<pthread.h>

#include"serrial.h"
using namespace  std;

void serrial::serrial_open()
{
    fd=open("/dev/ttyUSB0",O_RDWR );
    if (-1 == fd)
        {
            perror("Can't Open Serial Port ttyUSB0");
             //exit(-1);
            fd=open("/dev/ttyUSB1",O_RDWR );
            if(fd==-1)
            {
                perror("Can't Open Serial Port ttyUSB1");
                exit(-1);
            }
        }

     struct  termios Opt;
     tcgetattr(fd, &Opt);
     cfsetispeed(&Opt,B115200);     /*设置为19200Bps*/
     cfsetospeed(&Opt,B115200);


    Opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    Opt.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    Opt.c_oflag  &= ~OPOST;   /*Output*/
    Opt.c_cflag &= ~PARENB;
    Opt.c_cflag &= ~CSTOPB;
    Opt.c_cflag &= ~CSIZE;

    Opt.c_cflag |= CS8;
         tcflush (fd, TCIOFLUSH);
     tcsetattr(fd,TCSANOW,&Opt);
     Data_to_send[0]=0xFF;
     Data_to_send[1]=0x11;
     cout<<"init serrial successful"<<endl;
   //  pthread_t recieve;
  //  pthread_create(&recieve,NULL,pth,NULL);

}

int serrial::send_angle_1(float yaw, float pitch)
{

    //int temp=yaw/360*8191.0;;
    int temp=(int)(yaw*8191.0/360.0)+8191;
    //int temp=yaw/360*8191.0;
    temp%=8191;
    Data_to_send[3]=temp&0xff;
    Data_to_send[2]=(temp&0xff00)>>8;
    temp=(int)(pitch*8191.0/360.0)+8191;
        //int temp=yaw/360*8191.0;
        temp%=8191;
        Data_to_send[5]=temp&0xff;
        Data_to_send[4]=(temp&0xff00)>>8;

           tcflush (fd, TCOFLUSH);
           int n= write(fd,&Data_to_send[0],8);
           cout<<n<<endl;
           if(n==-1)
           {
               serrial_open();
           }

           // cout<<"yaw1: "<<Data_to_send[2]<<" yaw2: "<<Data_to_send[3]<<" pitch1 "<<Data_to_send[4]<<" pitch2 "<<Data_to_send[5]<<endl;
  //  cout<<"send data"<<endl;
    return  1;
}

int serrial::send_angle_2()
{
    for(int i=2;i<8;i++)
    {
        Data_to_send[i]=0x00;
    }
    tcflush(fd,TCOFLUSH);
    int m=write(fd,&Data_to_send[0],8);
    cout<<m<<endl;
    if(m==-1)
    {
        serrial_open();
    }
}

int serrial::get_pos()
{
        cout<<"emd"<<endl;
    //reset  header
    Data_read[0]=0;
    Data_read[1]=0;
     tcflush (fd, TCIFLUSH);
    while(1)
    {
        if((Data_read[0]==0xFF)&&(Data_read[1]==0x11))
        {
            break;
        }
        Data_read[0]=Data_read[1];
         read(fd,&Data_read[1],1);
        //cout<<hex<<(((int)Data_read[1])&0xFF)<<endl;
    }

    read(fd,Data_read+2,6);
    pos_pitch=((float)((int)Data_read[4]<<8)+Data_read[5])*360.0/8191.0;
    pos_yaw=((float)((int)Data_read[2]<<8)+Data_read[3])*360.0/8191.0;
    buffer_yaw.push_back(pos_yaw);
    if(buffer_yaw.size()>6)
    {
        buffer_yaw.erase(buffer_yaw.begin());
    }


     buffer_pitch.push_back(pos_pitch);
     if(buffer_pitch.size()>6)
     {
         buffer_pitch.erase(buffer_pitch.begin());
     }

     pos_pitch=buffer_pitch.back();
      pos_yaw=buffer_yaw.back();

        return  1;
}

void serrial::lock_date()
{
    if(buffer_yaw.empty())
    {
        return;
    }
      pos_pitch=buffer_pitch.back();
       pos_yaw=buffer_yaw.back();
}
