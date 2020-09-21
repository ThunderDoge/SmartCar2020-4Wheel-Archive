
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

//���ü�ȡ���̶��ٶ�
extern void setToFixedSpeed(short minFixedSpeed, short maxFixedSpeed);
extern void cancelSetToFixedSpeed(void);

//��ʵ��������Ϣ
extern volatile short currentSpeedLeft;
extern volatile short currentSpeedRight;
extern volatile int currentMileageLeft;
extern volatile int currentMileageRight;

#endif
