#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutdialog.h"
#include "mqttdialog.h"
#include "camimgthread.h"
#include "lidarimgthread.h"
#include "socketworker.h"
#include "mqttpubsub.h"
#include <QMainWindow>
#include <QWebEngineView>
#include <QtWebEngineWidgets>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    // 加载配置
    void loadConfig();
    // 保存配置
    void saveConfig();

    ~MainWindow();

private:
    QString mqtt_server_ip;
    qint16 mqtt_server_port;
    QString mqtt_client_id;
    QString mqtt_username;
    QString mqtt_password;

    const QString ONOFF_TOPIC = "ONOFF_ROBOT";
    const QString INFO_TOPIC = "ROBOT_INFO";
    const QString SYNC_TOPIC = "SYNC_ROBOT";
    // 需要展示的信息
    struct ShowInfo r_info;
    // mqtt
    MQTTPubSub *mqttpubsub;

    int m_state = -1;

    CamImgThread *camimg_thread;        // 相机视频流线程
    LidarImgThread *lidarimg_thread;    // 雷达视频流线程

    JoyThread *joy_thread;              // 控制器线程

    QThread sock_thread;                // 手柄读取线程
    SocketWorker *sock_worker;

    QSettings *config_ini;           // 参数读写

    Ui::MainWindow *ui;

    // 关于窗口
    AboutDialog *about_dialog;

    // 设置MQTT窗口
    MQTTDialog *mqtt_dialog;

    // 设置按钮
    void setButton(QPushButton *button, QString pix_url, QString btn_text="");

    // mqtt 初始化
    void initMQTT();

private slots:
    // 显示相机视频流图像
    void displayCamImg(QImage img);
    // 显示雷达视频流图像
    void displayLidarImg(QImage img);
    // 显示log
    void displayLog(QString log);

    // 检查网络连接
    void checkNet(const QHostInfo &);

    void on_camImg_btn_clicked();
    void on_gps_btn_clicked();
    void on_joy_btn_clicked();
    void on_connect_btn_clicked();
    void on_disconnect_btn_clicked();
    void on_lidarImg_btn_clicked();

    // 展示信息
    void showRobotInfo(QVariant s_var);

    void sendJoy(QVariant c_var);

signals:



};
#endif // MAINWINDOW_H
