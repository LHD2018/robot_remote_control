#ifndef JOYSTICK_H
#define JOYSTICK_H

/*
 * 手柄的基本的操作
 */

#include "windows.h"
#include "winuser.h"
#include "mmsystem.h"
#include <QDebug>
#include <QLibrary>
#include <QString>
#include <QMessageBox>

#define  JSX             JOYSTICKID1 //手柄ID号

//dll库函数声明
typedef int (*MyPrototype) (HWND, UINT, UINT, BOOL);
extern MyPrototype qJoySetCapture;

typedef void (*MyPrototype1) (UINT);
extern MyPrototype1 qJoyReleaseCapture ;

typedef void (*MyPrototype2) (UINT, LPJOYINFOEX);
extern MyPrototype2 qJoyGetPosEx;

typedef void (*MyPrototype0) (UINT, LPJOYINFO);
extern MyPrototype0 qJoyGetPos;

typedef void (*MyPrototype3) (UINT, LPUINT);
extern MyPrototype3 qJoyGetThreshold ;

typedef void (*MyPrototype4) (UINT, LPJOYCAPS, UINT);
extern MyPrototype4 qJoyGetDevCaps;

typedef UINT (*MyPrototype5) (void);
extern MyPrototype5 qJoyGetNumDevs;

//joystick外部接口
int openJoy();
struct joyinfoex_tag joyRead_row();


#endif // JOYSTICK_H
