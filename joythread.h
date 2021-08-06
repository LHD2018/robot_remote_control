#ifndef JOYTHREAD_H
#define JOYTHREAD_H

/*
 * 控制器线程，包含手柄和G29
 */

#include "joystick.h"
#include <QtMath>
#include <QThread>
#include <QMetaType>
#include <QVariant>


#define JOY_READ_PEROID  100 //读取手柄周期,单位ms

const float V_MAX_SPEED = 0.6;  // xy最大速度
const float W_MAX_SPEED = 1.93;    // w最大速度

struct Joykey_Info{
    int x_ax;   // 左摇杆上下（Vx）或油门
    int y_ax;   // 左遥杆左右（Vy）或刹车
    int u_ax;   // 右遥杆左右（W）或方向盘
    int v_ax;   // 右摇杆上下或离合器
};

// 控制器控制参数
struct JoyControl{
    int camera_tag;     // 相机流标识符（-1为后置相机，0为不传输相机流，1为前置相机）
    int robot_model;    // 机器人控制模式（0为正常模式(通用)，1为左侧行（G29），2为右侧行，3单独轮转向(通用)）
    int robot_gear;     // 机器人控制挡位（-1，0， 1， 2，3，4，5，6）
    float x_speed;      // x反向速度
    float y_speed;      // y方向速度
    float w_speed;      // w速度

};
Q_DECLARE_METATYPE(JoyControl);

class JoyThread : public QThread
{
    Q_OBJECT
public:
    QVariant joy_var;
    struct Joykey_Info joykey_info;
    struct JoyControl joy_c;
    joyinfoex_tag state_row;

    explicit JoyThread(QObject *parent = 0);

    // 处理蓝牙手柄数据
    void joystickProcess();
    // 处理G29数据
    void G29Process();

signals:
    void JoySignal_row(QVariant joy_var);
    void JoyLog(QString log);

protected:
    void run();

};

#endif // JOYTHREAD_H
