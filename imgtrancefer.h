#ifndef IMGTRANCEFER_H
#define IMGTRANCEFER_H

/*
 * 图像传输通用类
 * 使用方法：新建一个ImgTrancefer对象，调用他的transImg()函数，一般创建一个线程
 */

#include <QImage>
#include <QDebug>

extern "C"{

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libavdevice/avdevice.h>
}

class ImgTrancefer : public QObject
{
    Q_OBJECT
public:
    AVFormatContext *iCtx = NULL;
    AVFrame *yuv = NULL;
    SwsContext *vCtx = NULL;
    AVCodec *codec =NULL;
    AVCodecContext * videoCtx =NULL;
    int videoStream = -1;
    int outWidth = 1092;
    int outHeight = 680;
    QByteArray rtmp_url;

    // 设置图传url
    void setUrl(QString url);
    // 接收图像
    void transImg();
    explicit ImgTrancefer(QObject *parent = nullptr);
    ~ImgTrancefer();

signals:
    // 将图像发送的主线程，更新ui
    void SendImg(QImage img);
    // 发送log
    void TransIMGLog(QString log);

};

#endif // IMGTRANCEFER_H
