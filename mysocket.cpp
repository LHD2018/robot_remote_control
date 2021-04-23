#include "mysocket.h"

// 发送报文，buf_len取0发送字符流，发送二进制数据取二进制数据块大小
    bool tcpSendTo(QTcpSocket *sock, const char* buffer, const int buf_len) {

        int post_len = 0;
        if (buf_len == 0) post_len = strlen(buffer);
        else post_len = buf_len;

        int i_post_len = htonl(post_len);

        char* send_buffer = new char[post_len + 4];
        memset(send_buffer, 0, post_len + 4);
        memcpy(send_buffer, &i_post_len, 4);
        memcpy(send_buffer + 4, buffer, post_len);

        int left_len, writen_len, index_write;
        left_len = post_len + 4;
        index_write = 0;

        while (left_len > 0) {
            if ((writen_len = sock->write(send_buffer + index_write, left_len)) <= 0) return false;

            left_len -= writen_len;
            index_write += writen_len;
        }
        return true;
    }

    // 接收报文，s_timeout为超时时间，取0为无限等待
    bool tcpRecvFrom(QTcpSocket *sock, char* buffer) {


        int buf_len = 0;
        if (sock->read((char*)&buf_len, 4) <= 0) return false;
        buf_len = ntohl(buf_len);

        int left_len, read_len, index_read;
        left_len = buf_len;
        index_read = 0;

        while (left_len > 0) {
            if ((read_len = sock->read(buffer + index_read, left_len)) <= 0) return false;

            left_len -= read_len;
            index_read += read_len;
        }
        return true;
    }
