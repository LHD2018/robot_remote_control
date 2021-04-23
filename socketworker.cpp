#include "socketworker.h"

SocketWorker::SocketWorker(QObject *parent) : QObject(parent)
{
    server_ip = nullptr;
    server_port = 0;

}

void SocketWorker::connectSocket(){

    // 创建socket
    tcp_socket = new QTcpSocket();
    tcp_socket->connectToHost(server_ip, server_port);
    if(tcp_socket->waitForConnected(3000)){
        qDebug() << "connect server success";
        emit SocketLog("connect server success");
    }else{
        qDebug() << "connect server faild";
        emit SocketLog("connect server faild");
        return;
    }

    // 初始化控制参数与状态参数
    memset(&c_params, 0, sizeof(struct ControlParams));
    memset(&s_params, 0, sizeof(struct StatusParams));
    memset(&s_info, 0, sizeof(struct ShowInfo));

    // socket接收数据处理
    connect(tcp_socket, &QTcpSocket::readyRead, [=](){
        char recv_buffer[1024];
        if(tcpRecvFrom(tcp_socket, recv_buffer) == false){
            qDebug() << "recieve data failed!" << endl;
        }

        memcpy(&s_params, recv_buffer, sizeof(struct StatusParams)) ;
        s_info.robot_state = s_params.robot_state;
        s_info.gps_data.lon = s_params.gps_data.lon;
        s_info.gps_data.lat = s_params.gps_data.lat;

        s_var.setValue(s_info);
        emit DisplayInfo(s_var);

    });

    // 断开socket连接处理
    connect(tcp_socket, &QTcpSocket::disconnected, [=](){
        emit SocketLog("disconnect from server");
    });

}

void SocketWorker::sendData(QVariant joy_var){
    c_params = joy_var.value<ControlParams>();

    s_info.robot_gear = c_params.robot_gear;
    s_info.robot_model = c_params.robot_model;
    s_info.v_speed = sqrt(c_params.x_speed*c_params.x_speed + c_params.y_speed * c_params.y_speed);

    if(tcpSendTo(tcp_socket, (char *)&c_params, sizeof(ControlParams)) == false){
        qDebug() << "send data failed!" << endl;
    }

}




void SocketWorker::setIP(QString ip){
    server_ip = ip;
}

void SocketWorker::setPort(qint16 port){
    server_port = port;
}

void SocketWorker::disconnectSocket(){

    tcp_socket->disconnectFromHost();
    tcp_socket->close();
}
