#ifndef LIDARIMGTHREAD_H
#define LIDARIMGTHREAD_H


/*
 * 雷达图像传输线程
 */

#include "imgtrancefer.h"
#include <QThread>

class LidarImgThread : public QThread
{
    Q_OBJECT
public:
    explicit LidarImgThread(QObject *parent = nullptr);

    // 图像传输通用类
    ImgTrancefer *lidar_img;

    ~LidarImgThread();

signals:

protected:
    void run();

};

#endif // LIDARIMGTHREAD_H
