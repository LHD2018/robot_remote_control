#include "camimgthread.h"

CamImgThread::CamImgThread(QObject *parent) : QThread(parent)
{
    cam_img = new ImgTrancefer();

}

void CamImgThread::run(){

    cam_img->transImg();
}

CamImgThread::~CamImgThread(){
    delete cam_img;
}
