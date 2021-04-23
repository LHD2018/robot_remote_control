#ifndef SOCKETWORKER_H
#define SOCKETWORKER_H

#include "mysocket.h"
#include "joythread.h"
#include <QObject>


// 机器人状态
const int OFF_LINE_STATE = 0;       // 离线
const int NO_CONTROLLER_STATE = 1;  // 未有控制器
const int LOCAL_CONTROL_STATE = 2;  // 本地控制

// 机器人的当前状态参数
struct StatusParams{
    bool type_flag;
    int robot_state;
    struct {
        double lon;
        double lat;
    } gps_data;
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
