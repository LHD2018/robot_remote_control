#include "joythread.h"


JoyThread::JoyThread(QObject *parent) :
    QThread(parent)
{
}

//JoyThread的线程主体
void JoyThread::run()
{
    memset(&c_params, 0, sizeof(ControlParams));
    int joy_id = openJoy();

    if(joy_id == 0){
        emit JoyLog(QString("can not find controller!"));
        return;
    }else if(joy_id > 0 && joy_id < 20){
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
        c_params.type_flag = true;
        joy_var.setValue(c_params);
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
        c_params.x_speed = joykey_info.x_ax / 32767.0;
        c_params.y_speed = joykey_info.y_ax / 32511.0;

        if(c_params.y_speed > 0){
            c_params.camera_tag = 1;
        } else{
            c_params.camera_tag = -1;
        }
    }else{
        c_params.x_speed = 0.0;
        c_params.y_speed = 0.0;
        c_params.camera_tag = 0;
    }

    if(abs(joykey_info.u_ax) > 500){
        c_params.w_speed = joykey_info.u_ax / 32767.0;
    }else{
        c_params.w_speed = 0.0;
    }

    //qDebug() << "button" << state_row.dwButtons<< endl;

    if(state_row.dwButtons == 16) {
        // LB按钮
        c_params.camera_tag = 1;
    }else if(state_row.dwButtons == 32){
        // RB按钮
        c_params.camera_tag = -1;
    }else if(state_row.dwButtons == 256){
        // BACK按钮
        c_params.camera_tag = 0;
    }else if(state_row.dwButtons == 1){
        c_params.robot_model = 0;
    }else if(state_row.dwButtons == 2){
        c_params.robot_model = 1;
    }else if(state_row.dwButtons == 4){
        c_params.robot_model = 2;
    }else if(state_row.dwButtons == 8){
        c_params.robot_model = 3;
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

}

void JoyThread::copy_value_to_joykey_info()
{
 //*******************************************************//
    /*
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
    */
/***************************************************
                按键  轴数据  控制
 **************************************************/
/*
    //方向盘控制位置
        if(state_row.dwXpos>32832)
             joykey_info.X_Axis=(state_row.dwXpos*0.03051);
        else if(state_row.dwXpos<32700)
             joykey_info.X_Axis=(state_row.dwXpos*0.03051+1);
        else
             joykey_info.X_Axis=1000;
       //qDebug()<<joykey_info.X_Axis;

    //三个脚刹控制速度
        //正向速度
        if(state_row.dwRpos>state_row.dwZpos)
           joykey_info.Y_Axis=0.4218*(state_row.dwRpos-state_row.dwZpos);
        else
            joykey_info.Y_Axis=0;

        //反向速度
        if(state_row.dwRpos>state_row.dwYpos)
           joykey_info.Z_Axis=65535-0.4218*(state_row.dwRpos-state_row.dwYpos);
        else
            joykey_info.Z_Axis=65535;
        qDebug()<<"YYYY"<<joykey_info.Y_Axis;
        qDebug()<<"ZZZZ"<<joykey_info.Z_Axis;

     //档位选择
            if(state_row.dwButtons & 0x01 << 12) {
                joykey_info.U_Axis=1;
                //qDebug()<<"挡位1";
            }
            else if(state_row.dwButtons & 0x01 << 13) {
                joykey_info.U_Axis=2;
                //qDebug()<<"挡位2";
            }
            else if(state_row.dwButtons & 0x01 << 14) {
                joykey_info.U_Axis=3;
                //qDebug()<<"挡位3";
            }
            else if(state_row.dwButtons & 0x01 << 15) {
                joykey_info.U_Axis=4;
                //qDebug()<<"挡位4";
            }
            else if(state_row.dwButtons & 0x01 << 16) {
                joykey_info.U_Axis=5;
                //qDebug()<<"挡位5";
            }
            else if(state_row.dwButtons & 0x01 << 17) {
                joykey_info.U_Axis=6;
                //qDebug()<<"挡位6";
            }
            else if(state_row.dwButtons & 0x01 << 18) {
                joykey_info.U_Axis=7;
                //qDebug()<<"倒挡";
            }
            else{joykey_info.U_Axis=0;}

*/

//************************************************************//

//    //xbox 手柄写

//        joykey_info.X_Axis = state_row.dwXpos - 32767;
//        joykey_info.Y_Axis = 32511 - state_row.dwYpos;

//        joykey_info.U_Axis = state_row.dwZpos - 32767;
//        joykey_info.V_Axis = 32511 - state_row.dwRpos;

//        qDebug() << "button" << state_row.dwButtons<< endl;
//        if(state_row.dwButtons == 2) {

//            joykey_info.A_button=1;
//        }
//        else{joykey_info.A_button=0;}

//        if(state_row.dwButtons == 4) {
//         joykey_info.B_button=1;
//        }else{  joykey_info.B_button=0;}

//        if(state_row.dwButtons == 1) {
//            joykey_info.X_button=1;
//        }else{joykey_info.X_button=0;}

//        if(state_row.dwButtons == 8) {
//            joykey_info.Y_button=1;
//        }else{
//            joykey_info.Y_button=0;
//        }

//        if(state_row.dwButtons == 512) {
//               joykey_info.START_button=1;
//        }else{ joykey_info.START_button=0;}

//        if(state_row.dwButtons == 256) {
//            joykey_info.BACK_button=1;
//        }else{
//             joykey_info.BACK_button=0;
//        }



//********************未分配*********




//            if(state_row.dwButtons & 0x01 << 6) {
//                joykey_info.BACK_button=1;
//            }else{
//                 joykey_info.BACK_button=0;
//            }



//            if(state_row.dwPOV == 0) {
//               joykey_info.UP_button=1;

//            } else if(state_row.dwPOV == 9000) {

//                 joykey_info.RIGHT_button=1;

//            } else if(state_row.dwPOV == 18000) {
//               joykey_info.DOWN_button=1;

//            } else if(state_row.dwPOV == 27000) {

//                 joykey_info.LEFT_button=1;

//            } else if(state_row.dwPOV == 4500) {

//                joykey_info.UP_button=1;
//                joykey_info.RIGHT_button=1;

//            } else if(state_row.dwPOV == 31500) {

//                joykey_info.UP_button=1;
//                joykey_info.LEFT_button=1;

//            } else if(state_row.dwPOV == 13500) {
//             joykey_info.RIGHT_button=1;
//                joykey_info.DOWN_button=1;

//            } else if(state_row.dwPOV == 22500) {
//                joykey_info.LEFT_button=1;
//                joykey_info.DOWN_button=1;
//            }else{
//                joykey_info.LEFT_button=0;
//                joykey_info.DOWN_button=0;
//                joykey_info.UP_button=0;
//                joykey_info.RIGHT_button=0;
//            }



}
