#ifndef __CONTROL_ELEMENT_H__
#define __CONTROL_ELEMENT_H__

#define AD_STOP_THRESHOLD (10)

extern int getGlobalRuningCnt(void); //获取全局运行计时变量的取值
extern int getGlobalRuningMileage(void); //获取全局运行路径


extern void UiSaveimgAdcstopOled(void); //运行循环的公共事务
extern void unableAdcstop(void); //禁止电感停车
extern void enableAdcstop(void); //使能电感停车
extern void makeBuzzerSoundFor(int cnt); //让蜂鸣器响一段时间(车运行时)
extern void makeBuzzerSoundWhetherCarIsRuning(int cnt); 

#endif
