#include "imgtrancefer.h"

ImgTrancefer::ImgTrancefer(QObject *parent) : QObject(parent)
{

}

void ImgTrancefer::setUrl(QString url){
    rtmp_url = url.toLatin1();
}


void ImgTrancefer::transImg(){

    av_register_all();
    avformat_network_init();

    iCtx = avformat_alloc_context();
    int ret;

    ret = avformat_open_input(&iCtx, rtmp_url.data(), NULL, NULL);
    if(ret != 0){
        emit CamIMGLog(QString("open input faild!"));
        qDebug() << "open input faild!";
        return;
    }
    if(avformat_find_stream_info(iCtx, NULL) < 0){
        emit CamIMGLog(QString("find stream faild!"));
        qDebug() << "find stream faild!";
        return;
    }

    for(int i =0;i < iCtx->nb_streams;i++){
        if(iCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = i;

            break;
        }
    }

    videoCtx = iCtx->streams[videoStream]->codec;
    //查找视频解码器
    codec = avcodec_find_decoder(videoCtx->codec_id);
    if (!codec){
        emit CamIMGLog(QString("find decoder faild!"));
        qDebug() << "find decoder faild!";
        return;
    }

    videoCtx->time_base.num = 1;
    videoCtx->time_base.den = 25;


    //打开视频解码器
    ret = avcodec_open2(videoCtx, codec, NULL);
    if (ret != 0){
        emit CamIMGLog(QString("open codec faild!"));
        qDebug() << "open codec faild!";
        return;
    }


    if (!iCtx){
        qDebug() << "iCtx error!";
        return;
    }

    AVPacket *pkt = av_packet_alloc();
    yuv = av_frame_alloc();
    uchar *outSpace = new uchar[1920 * 1080 * 4];
    while(1){

        ret = av_read_frame(iCtx, pkt);
        if(ret != 0){
            emit CamIMGLog(QString("read frame faild!"));
            qDebug() << "read frame faild!";
            continue;
        }
        if(pkt->stream_index != videoStream) continue;

        ret = avcodec_send_packet(videoCtx, pkt);
        av_packet_unref(pkt);
        if(ret != 0){
            qDebug() << "send packet faild!";
            continue;
        }

        ret = avcodec_receive_frame(videoCtx, yuv);
        if(ret != 0){
            emit CamIMGLog(QString("get frame faild!"));
            qDebug() << "get frame faild!";
            continue;
        }

        //转码输出空间
        uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
        data[0] = (uint8_t *)outSpace;
        //图像宽度
        int linesize[AV_NUM_DATA_POINTERS] = { 0 };
        linesize[0] = outWidth * 4;

        if(vCtx == NULL){
            //设置转码器
            vCtx = sws_getCachedContext(vCtx,
                yuv->width, yuv->height, (AVPixelFormat)yuv->format,
                outWidth, outHeight, AV_PIX_FMT_BGRA,
                SWS_BICUBIC, NULL, NULL, NULL
            );
        }

        if (!vCtx){
            qDebug() << "codec context faild!" ;
            return;
        }
        //转码，成功后返回转码图像的高
        int h = sws_scale(vCtx, (const uint8_t **)yuv->data, yuv->linesize, 0, videoCtx->height,
            data, linesize);

        if(h <= 0){
            qDebug() << "scale frame faild!";
            return;
        }

        emit SendImg(QImage(outSpace, outWidth, outHeight, QImage::Format::Format_RGB32));
        av_free_packet(pkt);
    }

}

ImgTrancefer::~ImgTrancefer(){
    av_free(yuv);
    avcodec_close(videoCtx);
    av_free(iCtx);
}
