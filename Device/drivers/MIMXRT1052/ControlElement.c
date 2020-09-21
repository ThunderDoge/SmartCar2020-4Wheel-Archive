#include "ControlElement.h"

#include "UIControl.h"
#include "UIVar.h"
#include "pitimer.h"
#include "adcprocess.h"
#include "LQ_12864.h"
#include "speedProcess.h"
#include "LQ_PWM.h"
#include "angleProcess.h"

bool adcStopEnable = true; //电感停车默认激活
int globalRuningCnt = 0; //全局运行计时
int gloablRuningMileage = 0; //全局运行路程
int globalBuzzerCnt = 0; //全局蜂鸣器停止响的计数(车辆运行时)
int BuzzerCnt = 0; //蜂鸣器停止响的计数(无关车辆是否运行)


//获取本文件内变量的函数
int getGlobalRuningCnt(void) { //获取本文件内的全局运行计时变量的取值
	return globalRuningCnt;
}
int getGlobalRuningMileage(void) { //获取全局运行路径
	return gloablRuningMileage;
}

void UiSaveimgAdcstopOled() {
  
	//static int adStopCnt = 0;
  
        //仅在停止状态扫描UI
	if (Ui_Stop_Flag) {
		keyScanForUI();
	}
        
        //上次运行该函数时的路程读值
//	static int preMileage = 0;
//        
//	if (!Ui_Stop_Flag) {
//	//全局运行计时自增
//	globalRuningCnt++;
//	
//	//若未初始化，则初始化上次的路程读值
//	if (preMileage == 0)
//		preMileage = (currentMileageLeft + currentMileageRight) / 2;
//	//得到这次运行的路程读值
//	int currentMileage = (currentMileageLeft + currentMileageRight) / 2;
//	//将差值加入全局运行路程，并更新上次的路程读值
//	gloablRuningMileage += (currentMileage - preMileage);
//	preMileage = currentMileage;
//	
//	//将当前信息记录到UI界面
//	//addRunInfoToUi(globalRuningCnt, gloablRuningMileage);
//	}
//        else{
//          globalRuningCnt = 0;
//          gloablRuningMileage = 0;
//          globalBuzzerCnt = 0;
//        }
        
        //如果使能了电感停车,则进行电感出界保护 //TODO
        
//        if(pitIsrCnt2 * 0.005 >= 1){
//          if (adcStopEnable && !Turn_Back_Flag) {
//		if ((leftAdcVal < AD_STOP_THRESHOLD) && (rightAdcVal < AD_STOP_THRESHOLD))
//			adStopCnt++;
//		else
//			adStopCnt = 0;
//		if (adStopCnt >= 2000){
//			Ui_Stop_Flag = true;
//                        LED_Color(violet);
//                }
//          }
//        }
        //最大运行时间(调试用)
//	if (globalRuningCnt > (Ui_Max_Runing_Time * 75)) {
//		Ui_Stop_Flag = true;
//	}
        
        	//蜂鸣器
//	if (globalRuningCnt < globalBuzzerCnt || BuzzerCnt > 0) {
//		buzzer(1);
//		//如果是因为BuzzerCnt而响蜂鸣器，则计数自减
//		if (BuzzerCnt > 0)
//			BuzzerCnt--;
//	}
//	else
//		buzzer(0);
        
        //OLED显示(计时和路程)
//	if (!Ui_Stop_Flag) {
//                LCD_CLS();
//                float temp1 = leftAdcVal;
//                float temp2 = rightAdcVal;
//                float temp3 = MidAdcVal;
//                float temp4 = Adc_Process_Result;
//                sprintf(getOledBuf(2), "AdcError %4.1f",getAdcError());
//                sprintf(getOledBuf(3), "LeftAdc %4.1f",temp1);
//                sprintf(getOledBuf(4), "RightAdc %4.1f",temp2);
//                sprintf(getOledBuf(5), "MidAdc %4.1f",temp3);
//                sprintf(getOledBuf(6), "AdcSevorError %3.1f",temp4);
//		sprintf(getOledBuf(7), "t %.1f m %.1f", pitIsrCnt2 * 0.005, gloablRuningMileage / 5900.0f);
//		for (int i = 2; i < 8; i++) {
//                  LCD_refreshOneLine(i, false);
//                }
//	}
}

void unableAdcstop(void) { //禁止电感停车
	adcStopEnable = false;
}
void enableAdcstop(void) { //使能电感停车
	adcStopEnable = true;
}

void makeBuzzerSoundFor(int cnt) {
	globalBuzzerCnt = globalRuningCnt + cnt;
}

void makeBuzzerSoundWhetherCarIsRuning(int cnt) {
	BuzzerCnt = cnt;
}

static void showWhiteAndCenterInOled(void) {
  for (uint8 k = 0; k < 4; k++) { // 0 1 2 3页,每页8行
    for (uint8 j = 0; j < IMG_RGB_W; j++) {
      uint8 oledData = 0;
      //填oledData, i k * 0 - k * 7 对应 b0 - 7
      for (uint8 i = 0 + k * 8; i < 8 + k * 8; i++) {
        if (isWhite(i, j)) {
          if (centerValid[i] && centerLine[i] == j)
                  continue;
          oledData += (0x01 << (i - k * 8));
        }
      }
      
      //oled显示
      oledPutOnePos(j, k, oledData);
    }
  }
}