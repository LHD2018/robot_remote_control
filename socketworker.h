#ifndef SOCKETWORKER_H
#define SOCKETWORKER_H

#include "mysocket.h"
#include "joythread.h"
#include <QObject>


// 机器人状态
const int OFF_LINE_STATE = 0;       // 离线
const int NO_CONTROLLER_STATE = 1;  // 未有控制器
const int LOCAL_CONTROL_STATE = 2;  // 本地控制
const int REMOTE_CONTROL_STATE = 3; // 远程控制

// 控制机器人的参数
struct ControlParams{
    bool type_flag;     // 客户端标识符（true为控制器发送的ControlParams. false为机器人发送的StateParams）
    int camera_tag;     // 相机流标识符（-1为后置相机，0为不传输相机流，1为前置相机）
    int robot_model;    // 机器人控制模式（0为正常模式，3为单独轮转）
    float x_speed;      // x反向速度
    float y_speed;      // y方向速度
    float w_speed;      // w速度

};

// 机器人的当前状态参数
struct StatusParams{
    bool type_flag;     // 客户端标识符（true为控制器发送的ControlParams. false为机器人发送的StateParams）
    int robot_state;    // 机器人状态（0为离线，1为没有控制器，2为本地控制， 3为远程控制）
    struct {
        double lon;
        double lat;
    } gps_data;         // GPS信息（lon:经度， lat：纬度）
};

struct ShowInfo{
    int robot_state;
    int robot_model;
    int robot_gear;
    struct{
        double lon;
        double lat;
    }gps_data;

    float v_speed;
};
Q_DECLARE_METATYPE(ShowInfo);

class SocketWorker : public QObject
{
    Q_OBJECT
public:
    explicit SocketWorker(QObject *parent = nullptr);

    QTcpSocket *tcp_socket;         // socket

    struct JoyControl joy_c;        // 手柄传上来的数据

    struct ControlParams c_params;  // 控制器参数
    struct StatusParams s_params;   // 状态参数

    struct ShowInfo s_info;         // 在ui中显示的信息
    QVariant s_var;                 // 传递s_info的容器

public slots:
    // 发送数据
    void sendData(QVariant joy_var);

    // 设置ip，port
    void setIP(QString ip);
    void setPort(qint16 port);

    void connectSocket();
    // 断开连接
    void disconnectSocket();

signals:

    void SocketLog(QString log);

    void DisplayInfo(QVariant);

private:

    QString server_ip;
    qint16 server_port;


};

#endif // SOCKETWORKER_H
