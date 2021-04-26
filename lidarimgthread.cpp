#include "lidarimgthread.h"

LidarImgThread::LidarImgThread(QObject *parent) : QThread(parent)
{
    lidar_img = new ImgTrancefer();
}

void LidarImgThread::run(){

    lidar_img->transImg();
}


LidarImgThread::~LidarImgThread(){
    delete lidar_img;
}
