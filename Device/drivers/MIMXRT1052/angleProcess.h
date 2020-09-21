#include "szx_image.h"
#include "myUtil.h"

#ifndef __ANGLE_PROCESS_H__
#define __ANGLE_PROCESS_H__

//打角模式
#define TURN_USE_CAM (1)
#define TURN_USE_ADC_CAM_NOT_FOUND (2)
#define TURN_USE_ADC (3)
#define TURN_USE_ADC_BROKEN (4)
#define TURN_USE_CAM_BROKEN (5)
#define TURN_BEFORE_RING (6)
#define TURN_RING_TURN_IN (7)
#define TURN_RING_OUT (8)
#define TURN_RING_SHIELDING (9)
#define TURN_RING_ADVANCE_DECELERATE (10)
#define TURN_OBSTACLE_DECELERATE (11)
#define TURN_OBSTACLE_TURNOUT (12)
#define TURN_OBSTACLE_TURNBACK (13)
#define TURN_OBSTACLE_RETURN (14)
#define TURN_RAMP (15)
#define TURN_OBSTACLE_GOBACK (16)
#define TURN_RAMP_ADVANCE_DECELERATE (17)
#define TURN_OBSTACLE_ADVANCE_DECELERATE (18)

extern volatile short MaxCenterError;
//注意：这里的几个打角函数必须一起改！！！！！！！！！！！！！！！！

//本函数分别更新图像中线和电感值1次
//根据camFirst有先后顺序的区别
//fixedSpeed在camTurn时有效
//务必确保本函数每次执行是分别更新图像中线和电感值1次,并根据其中一种调整速度和打角
extern void updateCamAndAdcThenTurn(bool camFirst, bool fixedSpeed, int minFixedSpeed, int maxFixedSpeed, int turnMode);
//圆环用打角(摄像头优先)
extern void updateCamAndAdcThenTurnWithFixed(int fixedPwm, bool fixedSpeed, int setSpeed, int turnMode);
extern void updateCamAndAdcThenTurnWithMin(int minPwm, bool fixedSpeed, int setSpeed, int turnMode);

#endif
