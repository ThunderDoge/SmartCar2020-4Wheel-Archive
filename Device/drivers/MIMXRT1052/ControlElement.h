#ifndef __CONTROL_ELEMENT_H__
#define __CONTROL_ELEMENT_H__

#define AD_STOP_THRESHOLD (10)

extern int getGlobalRuningCnt(void); //��ȡȫ�����м�ʱ������ȡֵ
extern int getGlobalRuningMileage(void); //��ȡȫ������·��


extern void UiSaveimgAdcstopOled(void); //����ѭ���Ĺ�������
extern void unableAdcstop(void); //��ֹ���ͣ��
extern void enableAdcstop(void); //ʹ�ܵ��ͣ��
extern void makeBuzzerSoundFor(int cnt); //�÷�������һ��ʱ��(������ʱ)
extern void makeBuzzerSoundWhetherCarIsRuning(int cnt); 

#endif
