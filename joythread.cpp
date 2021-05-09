#include "joythread.h"


JoyThread::JoyThread(QObject *parent) :
    QThread(parent)
{
}

//JoyThread的线程主体
void JoyThread::run()
{
    memset(&joy_c, 0, sizeof(JoyControl));
    int joy_id = openJoy();

    if(joy_id == 0){
        emit JoyLog(QString("can not find controller!"));
        return;
    }else if(joy_id < 20){
        emit JoyLog(QString("joystick connected success!"));
    }else{
        emit JoyLog(QString("G29 connected success!"));
    }

    while(1) {
        msleep(JOY_READ_PEROID);
        state_row = joyRead_row();

        if(joy_id < 20){
            joystickProcess();
        }else{
            G29Process();
        }

        joy_var.setValue(joy_c);
        emit JoySignal_row(joy_var);

    }
}

void JoyThread::joystickProcess(){

    memset(&joykey_info, 0, sizeof (Joykey_Info));

    joykey_info.x_ax = state_row.dwXpos - 32767;
    joykey_info.y_ax = 32511 - state_row.dwYpos;
    joykey_info.u_ax = state_row.dwZpos - 32767;
    joykey_info.v_ax = 32511 - state_row.dwRpos;

    if(abs(joykey_info.x_ax) > 500 || abs(joykey_info.y_ax) > 500){
        joy_c.x_speed = V_MAX_SPEED * (joykey_info.x_ax / 32767.0);
        joy_c.y_speed = V_MAX_SPEED * (joykey_info.y_ax / 32511.0);

        if(joy_c.y_speed > 0){
            joy_c.camera_tag = 1;
        } else{
            joy_c.camera_tag = -1;
        }
    }else{
        joy_c.x_speed = 0.0;
        joy_c.y_speed = 0.0;
        joy_c.camera_tag = 0;
    }

    if(abs(joykey_info.u_ax) > 500){
        joy_c.w_speed = joykey_info.u_ax / 32767.0;
    }else{
        joy_c.w_speed = 0.0;
    }

    //qDebug() << "button" << state_row.dwButtons<< endl;

    switch (state_row.dwButtons) {

    case 1:
        qDebug() << "button_A";
        joy_c.robot_model = 0;
        break;
    case 4:
        qDebug() << "button_X";
        joy_c.robot_model = 3;
        break;
    case 16:
        qDebug() << "button_LB";
        joy_c.camera_tag = 1;
        break;
    case 32:
        qDebug() << "button_RB";
        joy_c.camera_tag = -1;
        break;
    case 256:
        qDebug() << "button_Back";
        joy_c.camera_tag = 0;
        break;
    default:
        break;
    }

    if(joy_c.robot_model == 0){
        joy_c.w_speed *= W_MAX_SPEED;
    }else{
        joy_c.y_speed = 0;
    }

}

void JoyThread::G29Process(){

    qDebug()<<"state_row.dwXpos:"<<state_row.dwXpos;
    //方向盘是X轴
    qDebug()<<"state_row.dwYpos:"<<state_row.dwYpos;
    //左一 离合
    qDebug()<<"state_row.dwRpos:"<<state_row.dwRpos;
    //左二 刹车
    qDebug()<<"state_row.dwUpos:"<<state_row.dwUpos;
    //空白 无数据
    qDebug()<<"state_row.dwZpos:"<<state_row.dwZpos;
    //左三 油门

    qDebug() << "button" << state_row.dwButtons<< endl;

    memset(&joykey_info, 0, sizeof(joykey_info));

    joykey_info.x_ax = 65535 - state_row.dwZpos;
    joykey_info.y_ax = 65535 - state_row.dwRpos;
    joykey_info.u_ax = 32767 - state_row.dwXpos;
    joykey_info.v_ax = 65535 - state_row.dwYpos;

    if(abs(joykey_info.u_ax) > 1000){
        joy_c.w_speed = joykey_info.u_ax / 32767.0;
    }else{
        joy_c.w_speed = 0.0;
    }

    if(abs(joykey_info.x_ax) < 1000){
        joykey_info.x_ax = 0;
    }

    switch (joy_c.robot_model) {

    case 0:
        if(joy_c.robot_gear >=0){
            joy_c.x_speed = 0.1 * joy_c.robot_gear * (joykey_info.x_ax / 65535.0);

        }else{
            joy_c.x_speed = -0.3 * (joykey_info.x_ax / 65535.0);
        }
        joy_c.y_speed = 0.0;
        joy_c.w_speed *= W_MAX_SPEED;
        break;
    case 1:
        joy_c.y_speed = 0.3 *(joykey_info.x_ax / 65535.0);
        joy_c.x_speed = 0.0;
        joy_c.w_speed = 0.0;
        break;
    case 2:

        joy_c.y_speed = -0.3 *(joykey_info.x_ax / 65535.0);
        joy_c.x_speed = 0.0;
        joy_c.w_speed = 0.0;
        break;
    case 3:
        // 轮独转
        if(joy_c.robot_gear >=0){
            joy_c.x_speed = 0.1 * joy_c.robot_gear * (joykey_info.x_ax / 65535.0);

        }else{
            joy_c.x_speed = -0.3 * (joykey_info.x_ax / 65535.0);
        }
        joy_c.y_speed = 0.0;
        joy_c.w_speed *= M_PI_2;
        break;
    default:
        break;

    }


    switch (state_row.dwButtons) {

    case 1:
        // X按钮
        joy_c.robot_model = 3;
        break;
    case 2:
        // 正方形按钮
        joy_c.robot_model = 1;
        break;
    case 4:
        // 圆形按钮
        joy_c.robot_model = 0;
        break;
    case 8:
        // 三角形按钮
        joy_c.robot_model = 2;
        break;

    case 4096:
        joy_c.robot_gear = 1;
        break;
    case 8192:
        joy_c.robot_gear = 2;
        break;
    case 16384:
        joy_c.robot_gear = 3;
        break;
    case 32768:
        joy_c.robot_gear = 4;
        break;
    case 65536:
        joy_c.robot_gear = 5;
        break;
    case 131072:
        joy_c.robot_gear = 6;
        break;
    case 262144:
        joy_c.robot_gear = -1;
        break;
    case 0:
        joy_c.robot_gear = 0;
        break;
    default:
        break;
    }

    if(joykey_info.y_ax > 20000){
        joy_c.x_speed = 0.0;
        joy_c.y_speed = 0.0;
        joy_c.w_speed = 0.0;
    }

}

