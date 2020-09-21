#include "ControlElement.h"

#include "UIControl.h"
#include "UIVar.h"
#include "pitimer.h"
#include "adcprocess.h"
#include "LQ_12864.h"
#include "speedProcess.h"
#include "LQ_PWM.h"
#include "angleProcess.h"

bool adcStopEnable = true; //���ͣ��Ĭ�ϼ���
int globalRuningCnt = 0; //ȫ�����м�ʱ
int gloablRuningMileage = 0; //ȫ������·��
int globalBuzzerCnt = 0; //ȫ�ַ�����ֹͣ��ļ���(��������ʱ)
int BuzzerCnt = 0; //������ֹͣ��ļ���(�޹س����Ƿ�����)


//��ȡ���ļ��ڱ����ĺ���
int getGlobalRuningCnt(void) { //��ȡ���ļ��ڵ�ȫ�����м�ʱ������ȡֵ
	return globalRuningCnt;
}
int getGlobalRuningMileage(void) { //��ȡȫ������·��
	return gloablRuningMileage;
}

void UiSaveimgAdcstopOled() {
  
	//static int adStopCnt = 0;
  
        //����ֹͣ״̬ɨ��UI
	if (Ui_Stop_Flag) {
		keyScanForUI();
	}
        
        //�ϴ����иú���ʱ��·�̶�ֵ
//	static int preMileage = 0;
//        
//	if (!Ui_Stop_Flag) {
//	//ȫ�����м�ʱ����
//	globalRuningCnt++;
//	
//	//��δ��ʼ�������ʼ���ϴε�·�̶�ֵ
//	if (preMileage == 0)
//		preMileage = (currentMileageLeft + currentMileageRight) / 2;
//	//�õ�������е�·�̶�ֵ
//	int currentMileage = (currentMileageLeft + currentMileageRight) / 2;
//	//����ֵ����ȫ������·�̣��������ϴε�·�̶�ֵ
//	gloablRuningMileage += (currentMileage - preMileage);
//	preMileage = currentMileage;
//	
//	//����ǰ��Ϣ��¼��UI����
//	//addRunInfoToUi(globalRuningCnt, gloablRuningMileage);
//	}
//        else{
//          globalRuningCnt = 0;
//          gloablRuningMileage = 0;
//          globalBuzzerCnt = 0;
//        }
        
        //���ʹ���˵��ͣ��,����е�г��籣�� //TODO
        
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
        //�������ʱ��(������)
//	if (globalRuningCnt > (Ui_Max_Runing_Time * 75)) {
//		Ui_Stop_Flag = true;
//	}
        
        	//������
//	if (globalRuningCnt < globalBuzzerCnt || BuzzerCnt > 0) {
//		buzzer(1);
//		//�������ΪBuzzerCnt�����������������Լ�
//		if (BuzzerCnt > 0)
//			BuzzerCnt--;
//	}
//	else
//		buzzer(0);
        
        //OLED��ʾ(��ʱ��·��)
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

void unableAdcstop(void) { //��ֹ���ͣ��
	adcStopEnable = false;
}
void enableAdcstop(void) { //ʹ�ܵ��ͣ��
	adcStopEnable = true;
}

void makeBuzzerSoundFor(int cnt) {
	globalBuzzerCnt = globalRuningCnt + cnt;
}

void makeBuzzerSoundWhetherCarIsRuning(int cnt) {
	BuzzerCnt = cnt;
}

static void showWhiteAndCenterInOled(void) {
  for (uint8 k = 0; k < 4; k++) { // 0 1 2 3ҳ,ÿҳ8��
    for (uint8 j = 0; j < IMG_RGB_W; j++) {
      uint8 oledData = 0;
      //��oledData, i k * 0 - k * 7 ��Ӧ b0 - 7
      for (uint8 i = 0 + k * 8; i < 8 + k * 8; i++) {
        if (isWhite(i, j)) {
          if (centerValid[i] && centerLine[i] == j)
                  continue;
          oledData += (0x01 << (i - k * 8));
        }
      }
      
      //oled��ʾ
      oledPutOnePos(j, k, oledData);
    }
  }
}