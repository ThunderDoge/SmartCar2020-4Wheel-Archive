#include "MainControl.h"
#include "UIControl.h"
#include "UIVar.h"
#include "angleProcess.h"
#include "adcprocess.h"
#include "speedProcess.h"
#include "ControlElement.h"
#include "pitimer.h"
#include "LQ_LED.h"

void finishLineControlProcess(void);

//extern Queue            queue;
//extern Threshold        threshold;
//extern Road             road;

//int finishlineflag = 0;

void mainControlProcess(void){
  static uint16_t temp1 = 0;
 
  temp1 = ADC_Read_Average(ADC1_CH14, 6)*3.3*6;
  BatV = (float)(temp1 / 4095.0f + 0.2f);
  
  UiSaveimgAdcstopOled();
  
//           if(!Ui_Stop_Flag || Ui_Test_Adc_Flag){
//             /*--------------������-----------------------*/
//              deal_adc(&queue);
//             /*--------------��������Ԫ�ؼ�����-------------*/
//              road = road_condition(&queue,&threshold);
//             /*--------------�������-----------------------*/
//              car_ctrl(road,&queue,&threshold);
//           }

}



void finishLineControlProcess(void) {
  //��ʼ���ͣ����
  //static bool checkIfPassFinishLine = false;
  //LED_Init(); 
 //if (getGlobalRuningCnt() <= (Ui_Finish_Shield_Time * 75)) { //δ������ʱ�����ͣ����
 //    checkIfPassFinishLine = false; //��֤����ʱ����ʱ��Ҫ���ȼ��ͣ�����ټ��ͨ��ͣ����
 //}
 //else { //���ͣ���ߵ���ʱ����
      if (isFinishLineCount >= 3) { //������������ߵ��д��ڵ���3�У���ȷ����⵽������
        //�����⵽ͣ���ߣ���ʼ���ͨ��ͣ����
              //checkIfPassFinishLine = true;
                LED_Color(red);
                //buzzer(1);
                //delayms(1000);
              //makeBuzzerSoundWhetherCarIsRuning(113);
      }
//      if (checkIfPassFinishLine && isNotFinishLineCount >= 3) { //���7����ͼ�����а����ߵĳ���
//        //��⵽ͨ��ͣ����
//              checkIfPassFinishLine = false;
//               LED_Color(red);
//               delayms(1000);
//             // Ui_Stop_Flag = true;
//      }
  //}
}
