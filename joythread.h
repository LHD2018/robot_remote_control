#ifndef JOYTHREAD_H
#define JOYTHREAD_H

/*
 * 控制器线程，包含手柄和G29
 */

#include "joystick.h"
#include <QThread>
#include <QMetaType>
#include <QVariant>


#define JOY_READ_PEROID  100 //读取手柄周期,单位ms

struct Joykey_Info{
    int x_ax;
    int y_ax;
    int u_ax;
    int v_ax;
};

struct ControlParams{
    bool type_flag;
    int camera_tag;
    int robot_model;
    int robot_gear;
    float x_speed;
    float y_speed;
    float w_speed;
};
Q_DECLARE_METATYPE(ControlParams);

class JoyThread : public QThread
{
    Q_OBJECT
public:
    QVariant joy_var;
    struct Joykey_Info joykey_info;
    struct ControlParams c_params;
    joyinfoex_tag state_row;

    explicit JoyThread(QObject *parent = 0);
    void copy_value_to_joykey_info();

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
