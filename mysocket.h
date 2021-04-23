#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QTcpSocket>
#include <WinSock2.h>

// 发送报文，buf_len取0发送字符流，发送二进制数据取二进制数据块大小
bool tcpSendTo(QTcpSocket *sock, const char* buffer, const int buf_len=0);

// 接收报文，s_timeout为超时时间，取0为无限等待
bool tcpRecvFrom(QTcpSocket *sock, char* buffer);

#endif // MYSOCKET_H
