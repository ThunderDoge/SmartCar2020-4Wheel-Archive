
#ifndef __SPEED_PROCESS_H__
#define __SPEED_PROCESS_H__

#include "sys.h"
#include "pitimer.h"
extern volatile uint16_t purpostEncVal;
extern volatile uint16_t purpostEncValLeft;
extern volatile uint16_t purpostEncValRight;

extern void updatePurpostEncVal();
extern int16_t getLeftUk();
extern int16_t getRightUk();

//设置及取消固定速度
extern void setToFixedSpeed(short minFixedSpeed, short maxFixedSpeed);
extern void cancelSetToFixedSpeed(void);

//车实际运行信息
extern volatile short currentSpeedLeft;
extern volatile short currentSpeedRight;
extern volatile int currentMileageLeft;
extern volatile int currentMileageRight;

#endif
