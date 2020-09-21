#include "adcprocess.h"
#include "LQ_ADC.h"
#include "pitimer.h"

volatile uint16_t leftAdcVal;
volatile uint16_t rightAdcVal;
volatile uint16_t MidleftAdcVal;
volatile uint16_t MidrightAdcVal;
volatile uint16_t LMidAdcVal;
volatile uint16_t RMidAdcVal;
volatile float preMAdcError = 0;
volatile float preAdcError = 0;
volatile int bendcntL = 0; 
volatile int bendcntR = 0;
volatile short Adc_Process_Result = 0;
volatile short MAdc_Process_Result = 0;
volatile short Straightcnt = 0;
extern Queue queue;
//float Bias = 0;

void updateLeftAdcVal(void) {
	leftAdcVal = ADC_Read_Average(ADC1_CH13, 6);//H13 ADC3 
}

void updateRightAdcVal(void) {
	rightAdcVal = ADC_Read_Average(ADC1_CH4, 6);//L10 ADC2
}

void updateMidLeftAdcVal(void) {
	MidleftAdcVal = ADC_Read_Average(ADC1_CH9, 6);//L12 ADC3
}

void updateMidRightAdcVal(void) {
	MidrightAdcVal = ADC_Read_Average(ADC1_CH15, 6);//L13 ADC2
}

void updateLMidAdcVal(void) {
	LMidAdcVal = ADC_Read_Average(ADC1_CH6, 6);//K11 ADC1
}

void updateRMidAdcVal(void) {
	RMidAdcVal = ADC_Read_Average(ADC1_CH8, 6);//M12 ADC1
}

uint8_t Adc_Max_Position(void){
  float Adc_Value[5] = {leftAdcVal / Ui_Max_Left_ADC_VAL,MidleftAdcVal / Ui_Max_MLeft_ADC_VAL,LMidAdcVal / Ui_Max_LMid_ADC_VAL,MidrightAdcVal / Ui_Max_MRight_ADC_VAL,rightAdcVal / Ui_Max_Right_ADC_VAL};
  float temp = Adc_Value[0];
  int MaxPosition = 0;
  int i;
  for(i=1;i<5;i++){
    if(temp<Adc_Value[i]){
      temp = Adc_Value[i];
      MaxPosition = i;
    }
  }
  
  if(MaxPosition == 0)
    return Left;
  else if(MaxPosition == 1)
    return MidLeft;
  else if(MaxPosition == 2)
    return Mid;
  else if(MaxPosition == 3)
    return MidRight;
  else if(MaxPosition == 4)
    return Right;
  
  return 0;
}



float getAdcError(void) {
	
	//一次归一化
//	float leftAdcPercentage = leftAdcVal / Ui_Max_Left_ADC_VAL;
//	float rightAdcPercentage = rightAdcVal / Ui_Max_Right_ADC_VAL;
	
        //比值法求偏差
//        float a = (float)(sqrt(queue.once_uni_AD[0]) - sqrt(queue.once_uni_AD[5]));
//        float b = (float)(queue.once_uni_AD[0] + queue.once_uni_AD[5]);
//        float c = queue.once_uni_AD[0];
        //float Bias = 100.0f * a / b;
         float Bias = ((float)(sqrt(queue.once_uni_AD[0])) - (float)(sqrt(queue.once_uni_AD[5]))) / (queue.once_uni_AD[0] + queue.once_uni_AD[5]);
//        if(Bias>0)
//          Bias = Bias;
//        else
//          Bias = -Bias;
	return 100.0f * Bias;
}

float getMAdcError(void) {
	
	//一次归一化
//	float MleftAdcPercentage = MidleftAdcVal / Ui_Max_MLeft_ADC_VAL;
//        float MrightAdcPercentage = MidrightAdcVal / Ui_Max_MRight_ADC_VAL;
	
        //比值法求偏差
        float Bias = ((float)(sqrt(queue.once_uni_AD[2])) - (float)(sqrt(queue.once_uni_AD[3]))) / (queue.once_uni_AD[2] + queue.once_uni_AD[3]);
//        if(Bias>0)
//          Bias = Bias;
//        else
//          Bias = -Bias;

	return 100.0f * Bias;
}

void MAdcProcessAndGetValue(void){
  
          float NowMAdcError = getAdcError();
          //float NowMAdcError = queue.dis_err_fifo[Ui_Control_Point];
          //      preMAdcError = queue.dis_err_fifo[9];
        MAdc_Process_Result = -((short)(Ui_Adc_P_Max * NowMAdcError + Ui_Adc_D_Max * (NowMAdcError - preMAdcError)));
        preMAdcError = NowMAdcError;
        LED_Color(yellow);
}

void adcProcessAndGetValue(void)
{ 
	float nowAdcError = queue.dis_err_fifo[Ui_Control_Point];
              preAdcError = queue.dis_err_fifo[9];
          //float nowAdcError = getAdcError(&queue);
          if (-Ui_Adc_Thr <= nowAdcError && nowAdcError <= Ui_Adc_Thr){
                  Adc_Process_Result = -((short)(Ui_Adc_SP[Ui_Speed_Mode] * nowAdcError + Ui_Adc_SD[Ui_Speed_Mode] * (nowAdcError - preAdcError) / 10));
                  LED_Color(blue);
                  
                  //if(pitIsrCnt2 * 0.003 > 40)
                  Straightcnt++;
                  
                  //Bend_Diff_Flag = false;
                  //buzzer(0);
          }
          else{
		Adc_Process_Result = -((short)(Ui_Adc_BP[Ui_Speed_Mode] * nowAdcError + Ui_Adc_BD[Ui_Speed_Mode] * (nowAdcError - preAdcError) / 10));
                LED_Color(green);
                Straightcnt = 0;
                //Bend_Diff_Flag = true;
                //buzzer(0);
          } 
          //preAdcError = nowAdcError;
}