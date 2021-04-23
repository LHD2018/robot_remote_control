#ifndef CAMIMGTHREAD_H
#define CAMIMGTHREAD_H

/*
 * 相机图像传输线程
 */

#include "imgtrancefer.h"
#include <QThread>

class CamImgThread : public QThread
{
    Q_OBJECT
public:
    explicit CamImgThread(QObject *parent = nullptr);

    // 图像传输通用类
    ImgTrancefer *cam_img;

    ~CamImgThread();

signals:

protected:
    void run();

};

#endif // CAMIMGTHREAD_H
