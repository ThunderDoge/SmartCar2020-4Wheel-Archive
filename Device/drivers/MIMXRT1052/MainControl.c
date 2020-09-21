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
//             /*--------------处理电感-----------------------*/
//              deal_adc(&queue);
//             /*--------------处理赛道元素及控制-------------*/
//              road = road_condition(&queue,&threshold);
//             /*--------------舵机控制-----------------------*/
//              car_ctrl(road,&queue,&threshold);
//           }

}



void finishLineControlProcess(void) {
  //开始检测停车线
  //static bool checkIfPassFinishLine = false;
  //LED_Init(); 
 //if (getGlobalRuningCnt() <= (Ui_Finish_Shield_Time * 75)) { //未到倒计时不检测停车线
 //    checkIfPassFinishLine = false; //保证倒计时结束时，要首先检测停车线再检测通过停车线
 //}
 //else { //检测停车线倒计时结束
      if (isFinishLineCount >= 3) { //如果检测的起跑线的行大于等于3行，则确定检测到起跑线
        //如果检测到停车线，开始检测通过停车线
              //checkIfPassFinishLine = true;
                LED_Color(red);
                //buzzer(1);
                //delayms(1000);
              //makeBuzzerSoundWhetherCarIsRuning(113);
      }
//      if (checkIfPassFinishLine && isNotFinishLineCount >= 3) { //这个7代表图像处理中斑马线的长度
//        //检测到通过停车线
//              checkIfPassFinishLine = false;
//               LED_Color(red);
//               delayms(1000);
//             // Ui_Stop_Flag = true;
//      }
  //}
}
