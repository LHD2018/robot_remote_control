#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/res/logo.png"));


    about_dialog = new AboutDialog(this);
    about_dialog->setModal(true);

    mqtt_dialog = new MQTTDialog(this);
    mqtt_dialog->setModal(true);

    config_ini = new QSettings(QCoreApplication::applicationDirPath() + "/other/config.ini", QSettings::IniFormat);
    loadConfig();

    connect(ui->action_save, &QAction::triggered, [=](){
        saveConfig();
    });

    connect(ui->action_about, &QAction::triggered, [=](){

        about_dialog->show();
    });

    connect(ui->action_mqtt, &QAction::triggered, [=](){
        mqtt_dialog->show();
    });

    qRegisterMetaType<QVariant>("QVariant");//注册一种信号的参数类型

    setButton(ui->on_btn, ":/res/start.png", QStringLiteral("开启机器人"));
    setButton(ui->off_btn, ":/res/stop.png", QStringLiteral("关闭机器人"));
    setButton(ui->sync_btn, ":/res/sync.png", QStringLiteral("同步信息"));
    ui->battery_bar->setValue(100);

    // 检测网络状况
    QHostInfo::lookupHost("www.baidu.com", this, SLOT(checkNet(QHostInfo)));

    camimg_thread = new CamImgThread();
    lidarimg_thread = new LidarImgThread();
    joy_thread = new JoyThread();

    sock_worker = new SocketWorker;
    sock_worker->moveToThread(&sock_thread);
    connect(&sock_thread, &QThread::finished, sock_worker, &QObject::deleteLater);
    // socket日志
    connect(sock_worker, SIGNAL(SocketLog(QString)), this, SLOT(displayLog(QString)));
    // 更新信息
    connect(sock_worker, SIGNAL(DisplayInfo(QVariant)), this, SLOT(showRobotInfo(QVariant)));
    // 连接手柄数据和socket
    connect(joy_thread, &JoyThread::JoySignal_row, this, &MainWindow::sendJoy);

    // 新建MQTT
    mqttpubsub = new MQTTPubSub(QHostAddress(mqtt_server_ip), mqtt_server_port, this);
    initMQTT();


    // 接收相机视频流处理
    connect(camimg_thread->cam_img, SIGNAL(SendImg(QImage)), this, SLOT(displayCamImg(QImage)));

    // 接收雷达视频流
    connect(lidarimg_thread->lidar_img, SIGNAL(SendImg(QImage)), this, SLOT(displayLidarImg(QImage)));

    // 手柄log
    connect(joy_thread, SIGNAL(JoyLog(QString)), this, SLOT(displayLog(QString)));
    // 相机流log
    connect(camimg_thread->cam_img, &ImgTrancefer::TransIMGLog, [=](QString log){
        ui->log_text->append("[camera]:" + log);
    });
    // 雷达流log
    connect(lidarimg_thread->lidar_img, &ImgTrancefer::TransIMGLog, [=](QString log){
        ui->log_text->append("[lidar]:" + log);
    });


    // mqtt信号
    connect(mqttpubsub, &MQTTPubSub::connected, [=](){
        qDebug() << "mqtt connected success";
        mqttpubsub->mqttSubcribe(INFO_TOPIC);
    });

    connect(mqttpubsub, &MQTTPubSub::subscribed, [=](const QString &topic){
        qDebug() << "mqtt subcriber " + topic + " success";
    });

    connect(mqttpubsub, &MQTTPubSub::error, [=](const QMQTT::ClientError error_log){

        qDebug() << "mqtt error: " + QString(error_log);
    });

    connect(mqttpubsub, &MQTTPubSub::received, [=](const QMQTT::Message& message){
        int robot_info = message.payload().toInt() ;
        qDebug() << robot_info;
        switch (robot_info) {
        case 0:
            displayLog("robot shutdown successfully!");
            break;
        case 1:
            displayLog("robot run successfully!");
            break;
        default:
            if(robot_info < 10){
                ui->battery_bar->setStyleSheet("QProgressBar::chunk{background:red}");
            }
            ui->battery_bar->setValue(robot_info);
            break;
        }

    });

    connect(ui->on_btn, &QPushButton::clicked, [=](){
        //qDebug() << "on";
        mqttpubsub->mqttPublish(ONOFF_TOPIC, "1");
    });

    connect(ui->off_btn, &QPushButton::clicked, [=](){
        //qDebug() << "off";
        mqttpubsub->mqttPublish(ONOFF_TOPIC, "0");
    });

    connect(ui->sync_btn, &QPushButton::clicked, [=](){

        mqttpubsub->mqttPublish(SYNC_TOPIC, "1");
    });

}

void MainWindow::sendJoy(QVariant c_var){
    if(sock_thread.isRunning()){
        sock_worker->sendData(c_var);
    }
}

void MainWindow::loadConfig(){
    // 读取mqtt配置
    mqtt_server_ip = config_ini->value("/MQTT/ip").toString();
    mqtt_server_port = config_ini->value("/MQTT/port").toInt();
    mqtt_client_id = config_ini->value("/MQTT/id").toString();
    mqtt_username = config_ini->value(("/MQTT/username")).toString();
    mqtt_password = config_ini->value("/MQTT/password").toString();

    // 读取socket配置
    ui->ip_text->setText(config_ini->value("/Socket/ip").toString());
    ui->port_text->setText(config_ini->value("/Socket/port").toString());

    // 读取RTMP视频流参数
    ui->camImg_text->setText(config_ini->value("/RTMP/camera").toString());
    ui->lidarImg_text->setText((config_ini->value("/RTMP/lidar").toString()));
}

void MainWindow::saveConfig(){

    config_ini->setValue("/Socket/ip", ui->ip_text->text());
    config_ini->setValue("/Socket/port", ui->port_text->text());

    config_ini->setValue("/RTMP/camera", ui->camImg_text->text());
    config_ini->setValue("/RTMP/lidar", ui->lidarImg_text->text());

}


void MainWindow::setButton(QPushButton *button, QString pix_url, QString btn_text){
    QPixmap pixmap;
    pixmap.load(pix_url);
    pixmap = pixmap.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button->setFixedSize(pixmap.width(),pixmap.height());
    button->setIcon(pixmap);
    button->setIconSize(QSize(pixmap.width(),pixmap.height()));
    button->setToolTip(btn_text);
}

void MainWindow::initMQTT(){

    mqttpubsub->setClientId(mqtt_client_id);
    mqttpubsub->setUsername(mqtt_username);
    mqttpubsub->setPassword(mqtt_password.toLatin1());
    mqttpubsub->mqttConnect();

}

void MainWindow::checkNet(const QHostInfo &host_info){
    if(host_info.error() != QHostInfo::NoError){
        displayLog("can not connecte to internet!");
    }
}

MainWindow::~MainWindow()
{

    delete config_ini;
    delete ui;

    sock_thread.quit();
    sock_thread.wait();
}

void MainWindow::displayLog(QString log){
    ui->log_text->append(log);
}

void MainWindow::displayCamImg(QImage img){

    ui->camera_img->setPixmap(QPixmap::fromImage(img));

}

void MainWindow::displayLidarImg(QImage img){

    ui->lidar_img->setPixmap(QPixmap::fromImage(img).scaled(ui->lidar_img->width(),
                                                            ui->lidar_img->height(),
                                                            Qt::KeepAspectRatio));

}

void MainWindow::on_camImg_btn_clicked(){
    if(camimg_thread->isRunning()){
        return;
    }

    QString camimg_url = ui->camImg_text->text();
    if(camimg_url == ""){
        displayLog("camImg_url is empty");
        return;
    }

    camimg_thread->cam_img->setUrl(camimg_url);

    camimg_thread->start();
    if(camimg_thread->isRunning()){
        ui->log_text->append("start recieving camera stream...");
    }else{
        ui->log_text->append("recieve camera stream faild!");
    }

}

void MainWindow::on_lidarImg_btn_clicked(){
    if(lidarimg_thread->isRunning()){
        return;
    }

    QString lidarimg_url = ui->lidarImg_text->text();
    if(lidarimg_url == ""){
        displayLog("lidarImg_url is empty");
        return;
    }

    lidarimg_thread->lidar_img->setUrl(lidarimg_url);

    lidarimg_thread->start();

    if(lidarimg_thread->isRunning()){
        ui->log_text->append("start recieving lidar stream...");
    }else{
        ui->log_text->append("recieve lidar stream faild!");
    }
}

void MainWindow::on_gps_btn_clicked()
{

    QString strMapPath="file:///" + QCoreApplication::applicationDirPath() + "/other/index.html";
    ui->map_frame->load(strMapPath);
    ui->map_frame->show();
    ui->log_text->append("start GPS location...");


}

void MainWindow::on_joy_btn_clicked()
{

    joy_thread->start();
}

void MainWindow::on_connect_btn_clicked(){

    if(!sock_thread.isRunning()){
        sock_thread.start();
    }

    sock_worker->setIP(ui->ip_text->text());
    sock_worker->setPort(ui->port_text->text().toInt());
    sock_worker->connectSocket();
}

void MainWindow::on_disconnect_btn_clicked(){
    sock_worker->disconnectSocket();
}


void MainWindow::showRobotInfo(QVariant s_var){
    r_info = s_var.value<ShowInfo>();

    if(r_info.robot_gear > 0)
        ui->gear_label->setText(QStringLiteral("挡位") + QString::number(r_info.robot_gear));
    else if(r_info.robot_gear == 0)
        ui->gear_label->setText(QStringLiteral("空挡"));
    else
        ui->gear_label->setText(QStringLiteral("倒挡"));


    switch(r_info.robot_model){
    case 0:
        ui->model_label->setText(QStringLiteral("正常模式"));
        break;

    case 1:
        ui->model_label->setText(QStringLiteral("左侧行模式"));
        break;
    case 2:
        ui->model_label->setText(QStringLiteral("右侧行模式"));
        break;
    case 3:
        ui->model_label->setText(QStringLiteral("单独轮转模式"));
        break;
    default:
        break;

    }
    if(r_info.gps_data.lat != 0.0){

        double lat = Utils::dms2d(r_info.gps_data.lat);
        double lon = Utils::dms2d(r_info.gps_data.lon);

        QString cmd = QString("doLocal(%1,%2)").arg(QString::number(lon, 'f', 8)).arg(QString::number(lat, 'f', 8));
        qDebug() << cmd;
        ui->map_frame->page()->runJavaScript(cmd);

    }

    if(m_state != r_info.robot_state){
        m_state = r_info.robot_state;
        switch (m_state) {
        case OFF_LINE_STATE:
            displayLog("robot is offline, please try to turn on it or connecte to server!");
            break;
        case NO_CONTROLLER_STATE:
            displayLog("robot is no-localcontroller state");
            break;
        case LOCAL_CONTROL_STATE:
            displayLog("robot is local-control state");
            break;
        default:
            break;

        }
    }
    ui->speed_dial->UpdateAngle(r_info.v_speed);

}

