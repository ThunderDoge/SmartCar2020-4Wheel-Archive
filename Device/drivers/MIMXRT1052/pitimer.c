#include "pitimer.h"
#include "LQ_LED.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//PIT��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/4/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#include "ANO_DT.h"
#include "LQ_UART_DMA.h"

#include "LQ_Encoder.h"
#include "speedProcess.h"
#include "adcprocess.h"
#include "LQ_PWM.h"
#include "LQ_PIT.h"
#include "MainControl.h"
//#include "uiVariables.h"
extern short Straightcnt;

volatile uint16_t pitIsrCnt2 = 0;
volatile uint16_t pitIsrCnt3 = 0;
float adcerr = 0.0;
float Madcerr = 0.0;
int adStopCnt = 0;
bool    LeftRingFlag       = false;              //Բ��ִ�б�־
bool    RightRingFlag      = false;
bool    Ramp_Flag          = false;
bool    Finish_Flag        = false;
bool    Laser_Finish_Flag  = false;
bool    Hall_Finish_Flag   = false;
bool    Out_Garage_Flag    = false;
bool    Lost_Line_Flag     = false;  
bool    Stop_Flag          = false;
extern bool    adcStopEnable;
uint16  temp1          = 60000;                 //Բ�������м�ʱ��
uint16  temp2          = 60000;                 //�µ�ʱ��
uint16  temp3          = 60000;                 //�����ʱ��
uint16  Rampcnt        = 0;                     //�µ�
uint16  Ringcnt        = 0;                     //Բ������������һ��ʱ���ָ�
uint16  Finishcnt      = 0;                     //������������ָ�
uint16_t tfMiniDistance = 0;
uint16_t tfMiniStrength = 0;

//��ʼ��PITͨ��0,PITʱ��ԴΪperclk_clk_root=75MHz.
//ldval: CH0��ʼֵ,0~0xFFFFFFFF
//�ݼ�����,��ldval==0ʱ,�����ж�.
//��ʱʱ��=ldval/PERCLK_CLK_ROOT
//void PIT_CH0_Int_Init(void)
//{
//    LQ_PIT_Init(kPIT_Chnl_1, 5000);
////    PIT_GetDefaultConfig(&pit_config);  //��ʼ��ΪĬ������
////    pit_config.enableRunInDebug=true;   //����ģʽ��PIT��������
////    PIT_Init(PIT,&pit_config);          //��ʼ��PIT��ʱ��
////    
////    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0,ldval);//���õ���ʱ��ʼֵ
////    PIT_EnableInterrupts(PIT,kPIT_Chnl_0,kPIT_TimerInterruptEnable);//ʹ���ж�
////    NVIC_SetPriority(PIT_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),4,0));	//��ռ���ȼ�6�������ȼ�0
////	  EnableIRQ(PIT_IRQn);	                //ʹ��PIT�ж�
////    PIT_StartTimer(PIT,kPIT_Chnl_0);        //��PIT
//}

Queue            queue = {0};
Threshold        threshold = {0};
Road             road;

Road judge_ramp(Queue *queue);
Road road_condition(Queue *queue,Threshold *threshold);
void deal_adc(Queue *queue);
void deal_enc(Queue *queue);
void renew_dis10T_fifo(Queue *queue);
void car_ctrl(Road road,Queue *queue,Threshold *threshold);

void PIT_IRQHandler(void)
{
    /* �ж�PITͨ��0�����ж�.*/
    
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_1)&kPIT_TimerFlag)==kPIT_TimerFlag)  
    {
        /* �����־λ.*/
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
        /*�û���������Ӵ���*/
        //GPIO_PinWrite(GPIO3,13,1U);
      if(!Ui_Stop_Flag || Ui_Test_Adc_Flag){
        /*--------------������-----------------------*/
        deal_adc(&queue);
        
        /*--------------��ȡENCֵ----------------------*/
        deal_enc(&queue);
        
        /*--------------��������Ԫ�ؼ�����-------------*/
        road = road_condition(&queue,&threshold);
        //renew_dis10T_fifo(&queue);
        
        //����purpostEncVal
	updatePurpostEncVal();
        
        car_ctrl(road,&queue,&threshold);
      //}
        
        //purpostEncVal = 80;
	//purpostEncValLeft = 80;
	//purpostEncValRight = 80;

        //������ҵĿ�����(encVal)
	int16_t UkLeft = getLeftUk();
	int16_t UkRight = getRightUk();
        
        int16_t leftMotorPWM = UkLeft * -Ui_Uk_To_Pwm;
	int16_t rightMotorPWM = UkRight * -Ui_Uk_To_Pwm;
        
        LQ_SetMotorDty(1,leftMotorPWM);
        LQ_SetMotorDty(2,rightMotorPWM);
        
//        if(!Ui_Stop_Flag){
//          LQ_SetMotorDty(1,-3500);
//          LQ_SetMotorDty(2,-3500);
//        }
//        else{
//          LQ_SetMotorDty(1,0);
//          LQ_SetMotorDty(2,0);
//        }
      }
        
        if(!Ui_Stop_Flag)
          pitIsrCnt2++;
      if(pitIsrCnt2 * 0.003 >= 1){
          if (adcStopEnable) {
		if ((leftAdcVal < 100) && (rightAdcVal < 100))
			adStopCnt++;
		else
			adStopCnt = 0;
		if (adStopCnt >= 200){
			Stop_Flag = true;
                        LED_Color(violet);
                }
          }
        }
      //GPIO_PinWrite(GPIO3,13,0U);
        
        //PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
    }
    
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_2)&kPIT_TimerFlag)==kPIT_TimerFlag) 
    {
        /* �����־λ.*/
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);    
        /*�û���������Ӵ���*/
        //Test_CCD();
        CCD_Collect();
        //MPos = Adc_Max_Position();
        
    }
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_3)&kPIT_TimerFlag)==kPIT_TimerFlag)  
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);   
        
        /*�û���������Ӵ���*/        
        pitIsrCnt3++;
    }
    __DSB();
}
/*-----------------------�жϵ�·״��------------------------*/
Road road_condition(Queue *queue , Threshold *threshold){
  float         sum34        = 0.0;     //��¼������34��ֵ֮��
  float         sum_err      = 0.0;     //�������ERR_SUM_LEN��ƫ�����֮��
  float         sum1256      = 0.0;     //��¼������1,2,5,6��ֵ֮��
  float         sum_1346     = 0.0;     //��¼����1,3,4,6֮�ͣ������ж϶��ߣ����ʮ�ֻ�
  float         Ramp_sum     = 0.0;     //�����ж��µ��ĵ��ֵ
  uint8         i;                      //�ۼӾ���ƫ���ʱ����
  
  static uint16  Bend_Time      = 0;    //��¼�ܵ�����ȥ��ʱ�䣬����һ��ֵ˵���Ѿ�����У������ͣ��
  static uint16  Non_Bend_Time  = 0;    //��¼��������������Ĵ���
  static uint16  cross_time     = 0;    //��¼̽�����ʮ�ֽ�����ֵĴ���
  static uint16  non_cross_time = 0;    //��¼������ʮ�������Ĵ���
  //static uint16  Ramp_Time      = 0;    //�µ��жϴ���
  Road road;
/*-----------------------------�ƶ�����ƫ�����----------------------------*/
  for(i = ERR_LENGTH20-1;i>0;i--)
    queue->dis_err_fifo[i] = queue->dis_err_fifo[i-1];
  
  sum_1346 = queue->once_uni_AD[0] + queue->once_uni_AD[5]
    + queue->once_uni_AD[2] + queue->once_uni_AD[3];
  sum34 = queue->once_uni_AD[2] + queue->once_uni_AD[3];
  Ramp_sum = queue->twice_uni_AD[2] + queue->twice_uni_AD[3] 
    - queue->twice_uni_AD[0] - queue->twice_uni_AD[5];
  /*-----------------------------����-----------------------------------*/
  if(pitIsrCnt2 * 0.003 < Ui_Out_Garage_Time)
    return OutGarage;
  else if(Out_Garage_Flag)
  {
    Out_Garage_Flag = false;
  }
  /*----------------------------Բ���ж�---------------------------------*/
  if(sum_1346 > Ui_Ring_In_Adc && Ringcnt == 0 && !Ramp_Flag)
  {
    Ringcnt++;
    temp1 = pitIsrCnt2;
    buzzer(1);
    if(Ui_Ring_Directions[Ui_Speed_Mode][Ui_Ring_N]){
      queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
      setToFixedSpeed(Ui_Ring_Max_Speed[Ui_Speed_Mode][Ui_Ring_N],Ui_Ring_Min_Speed[Ui_Speed_Mode][Ui_Ring_N]);
      return RightRing;
    }
    else{
      queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
      setToFixedSpeed(Ui_Ring_Max_Speed[Ui_Speed_Mode][Ui_Ring_N],Ui_Ring_Min_Speed[Ui_Speed_Mode][Ui_Ring_N]);
      return LeftRing;
    }
  }
  else if(sum34 < Ui_Ring_Out_Adc && (pitIsrCnt2 * 0.003 - temp1 * 0.003 > Ui_Ring_Shield_Time[Ui_Speed_Mode][Ui_Ring_N]) && (Ringcnt == 1))
  {
    Ui_Ring_N++;
    LeftRingFlag = false;
    RightRingFlag = false;
    Ramp_Flag = false;
    cancelSetToFixedSpeed();
    Ringcnt = 0;
    buzzer(0);
  }
  /*---------------------------�µ��ж�---------------------------------*/
  if(Ramp_sum > Ui_Ramp_Up_Adc && sum_1346 > threshold->LOSE_LINE_SUM && Rampcnt == 0 && !LeftRingFlag && !RightRingFlag){
    Rampcnt++;
    Straightcnt = 0;
    buzzer(1);
//    if(Rampcnt >= 3)
//    {
        temp2 = pitIsrCnt2;
        //queue->dis_err_fifo[0] = queue->dis_err;
        queue->dis_err_fifo[0] = 
                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
     
     if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
        //setToFixedSpeed(Ui_Ring_In_Max_Speed,Ui_Ring_In_Min_Speed);
        return Ramp;
    //}
//    else
//      Rampcnt = 0;
  }
  else if(tfMiniDistance <= 40 && tfMiniStrength >= Ui_Laser_Threshold && !Ramp_Flag)
  {
    Rampcnt++;
    Straightcnt = 0;
    if(Rampcnt > 10){
    buzzer(1);
    temp2 = pitIsrCnt2;
    //queue->dis_err_fifo[0] = queue->dis_err;
    queue->dis_err_fifo[0] = 
                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
     
     if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
    return Ramp;
    }
  }
  else
  {
    Rampcnt = 0;
  }
  if((pitIsrCnt2 * 0.003 - temp2 * 0.003) > Ui_Ramp_Time && Ramp_Flag)
  {
    buzzer(0);
    //temp2 = 60000;
    //cancelSetToFixedSpeed();
    Rampcnt = 0;
    Ramp_Flag = false;
  }
//  if(Ramp_Flag)
//  {
//    queue->dis_err_fifo[0] = 
//                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
//                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
//     
//     if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
//       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
//     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
//       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
//    //return Ramp;
//  }
  /*-----------------------------------�����ж�------------------------------*/
//  sum_1346 = queue->once_uni_AD[0] + queue->once_uni_AD[5]
//    + queue->once_uni_AD[2] + queue->once_uni_AD[3];
  if(sum_1346 < threshold->LOSE_LINE_SUM && !Ramp_Flag)
  {
    Bend_Time++;
//    if(!Ramp_Flag || !LeftRingFlag || !RightRingFlag)
//    buzzer(0);
    //LED_Color(cyan);
    sum_err = 0.0;//����ƫ���
    //Bend_Diff_Flag = true;
    Straightcnt = 0;
    if(Bend_Time > 15)
    Lost_Line_Flag = true;
    for(i = 1; i < Ui_Fifo_Sum;i ++)
    {
      sum_err += queue->dis_err_fifo[i];
    }
    sum_err += queue->dis_err;
    if(sum_err < 0.0 || RightRingFlag)
      queue->dis_err_fifo[0] = Ui_Dis_Err_Min;//���ת�ǵ��������
    else if(sum_err > 0.0 || LeftRingFlag)
      queue->dis_err_fifo[0] = Ui_Dis_Err_Max;//���ת�ǵ����
            return Lose_line;//����
  }
  else
  {
    Bend_Time = 0;
    Lost_Line_Flag = false;
  }
  /*-----------------------------ʮ���ж�------------------------------------*/
  if(queue->once_uni_AD[1] * queue->once_uni_AD[4] > threshold->CROSS_VALUE && !LeftRingFlag && !RightRingFlag)
  {
    Straightcnt = 0;
    cross_time ++;
    if(cross_time > 8)//����5�ζ���ʮ�ֽ������ж�Ϊʮ�ֽ���
    {
     buzzer(1);
     LED_Color(violet);
     queue->dis_err_fifo[0] = 
                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
     
     if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
       
      return Cross;
    }
  }
  //�˳�ʮ�ֹ���
  else if(cross_time > 15)//˵��֮ǰȷʵ��ʮ�֣����ж���εĲ������ǲ��Ǹ���
  {
    non_cross_time ++;
    if(non_cross_time > Ui_Cross_Time)//����10�ζ�����ʮ�֣�˵��ȷʵ�˳���ʮ��
    {
      buzzer(0);
      non_cross_time = 0;
      cross_time = 0;//����쳣��ֵ
    }
    else//��Ȼ��Ϊ����������֤�˳�ʮ�֣�������ʮ�ִ���
    {
      buzzer(1);
      queue->dis_err_fifo[0] = 
                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
      
      if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
      
      return Cross;
    }
  }
  else 
  { 
    if(cross_time >= 8 && cross_time <= 15)
      buzzer(0);
    
    cross_time = 0;//˵�����ǽ�ʮ��Ҳ�����˳�ʮ�֣���һֱ������ʮ��
    non_cross_time = 0;
  }
  /*-----------------------------�������ж�----------------------------------*/
  if(!Fixed_Finish_Flag)
  {
  if(((GPIO_PinRead(GPIO1,03)) || (GPIO_PinRead(GPIO1,22))) && !Ramp_Flag && Finishcnt == 0 && (pitIsrCnt2 * 0.003 > Ui_Finish_Shield_Time[Ui_Speed_Mode])) //&& (Straightcnt >= 100))
  {
    Finishcnt++;
    temp3 = pitIsrCnt2;
    return LaserFinish;
  }
//  else if(GPIO_PinRead(GPIO1,20) && Finishcnt == 0)
//  {
//    Finishcnt++;
//    //temp3 = pitIsrCnt2;
//    return HallFinish;
//  }
  }
  else
  {
    if(pitIsrCnt2 * 0.003 > Ui_Finish_Shield_Time[Ui_Speed_Mode] && Finishcnt == 0)
    {
      Finishcnt++;
      temp3 = pitIsrCnt2;
      return LaserFinish;
    }
  }
 /*---------------------------����ͨ������·��������PIDУ��------------------*/
  queue->dis_err_fifo[0] = queue->dis_err;//����ƫ�����
  if(Ramp_Flag)
  {
    queue->dis_err_fifo[0] = 
                        Ui_fit_LM * queue->once_uni_AD[2]/sum34
                      + Ui_fit_RM * queue->once_uni_AD[3]/sum34;
     
     if(queue->dis_err_fifo[0] < Ui_Dis_Err_Min)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Min;
     else if(queue->dis_err_fifo[0] > Ui_Dis_Err_Max)
       queue->dis_err_fifo[0] = Ui_Dis_Err_Max;
    //return Ramp;
  }
  return Straight_Devious;
}

//Road judge_ramp(Queue *queue){
//  return Ramp_up;
//}

void renew_dis10T_fifo(Queue *queue){
  uint8 i;
  uint8 cnt = 0;
  cnt++;
  if(cnt>2)
  {
    cnt=0;
    for(i = ERR_10T_LENGTH;i>0;i--)
      queue->dis10T_fifo[i] = queue->dis10T_fifo[i-1];
    
    if(fabsf(queue->dis_err_fifo[0]) > fabsf(queue->dis_err_ori))
      queue->dis10T_fifo[0] = queue->dis_err_fifo[0];
    else
      queue->dis10T_fifo[0] = queue->dis_err_ori;
  }
}

void car_ctrl(Road road,Queue *queue,Threshold *threshold){
  if(!Ui_Stop_Flag)
  {
    switch(road)
    {
    case OutGarage:
      Out_Garage_Flag = true;
      break;
    case Cross:
      //buzzer(1);
      break;
    case Straight_Devious:
     if(!(queue->Cal_Speed_Flag) && !(Ramp_Flag))
      queue->Cal_Speed_Flag = true;
     
      //buzzer(0);
      break;
    case Lose_line:
      //buzzer(1);
      if((Lost_Line_Flag || LeftRingFlag || RightRingFlag) && !Laser_Finish_Flag && !Hall_Finish_Flag)
      {
      if(queue->dis_err_fifo[0] < 0 && !Finish_Flag)
      {
        if(LeftRingFlag)
          LQ_SetServoDty(-120 + SERVO_MID_PWM_VAL);
        else
          LQ_SetServoDty(120 + SERVO_MID_PWM_VAL);
      }
      else if(queue->dis_err_fifo[0] >= 0 && !Finish_Flag)
      {
        if(RightRingFlag)
          LQ_SetServoDty(120 + SERVO_MID_PWM_VAL);
        else
          LQ_SetServoDty(-120 + SERVO_MID_PWM_VAL);
      }
      return;
      }
      break;
    /*------------------------------Բ������-----------------------------------*/
    case LeftRing:
      //buzzer(1);
      LeftRingFlag = true;
      break;
    case RightRing:
      //buzzer(1);
      RightRingFlag = true;
      break;
    /*------------------------------�µ�����-----------------------------------*/
    case Ramp:
      //buzzer(1);
      Ramp_Flag = true;
      //queue->Cal_Speed_Flag = false;
      //purpostEncVal = threshold->Ramp_Speed;
      break;
    /*------------------------------��⴦��-----------------------------------*/
    case LaserFinish:
      LED_Color(green);
      Laser_Finish_Flag = true;
      break;
    case HallFinish:
      LED_Color(green);
      Hall_Finish_Flag = true;
      break;
    /*------------------------------�쳣����-----------------------------------*/ 
//    case Out_of_way:
//      queue->Cal_Speed_Flag = false;
//      purpostEncVal = threshold->OUT_OF_WAY_SPEED;
//      break;
    /*----------------------�����쳣������ܵ��ܾ��Ҳ���У������ͣ��-----------*/
//    case No_road:
//      queue->Cal_Speed_Flag = false;
//      Ui_Stop_Flag = true;
//      break;
    default:
      break;
    }
  }
  if(pitIsrCnt2 * 0.003 < Ui_Out_Garage_Time && Out_Garage_Flag){
    LQ_SetServoDty(Ui_Out_Garage_Servo_Duty + SERVO_MID_PWM_VAL);
  }
  else if(LeftRingFlag && (pitIsrCnt2 * 0.003 - temp1 * 0.003) < Ui_Ring_In_Servo_Time[Ui_Speed_Mode][Ui_Ring_N] && (pitIsrCnt2 * 0.003 - temp1 * 0.003) > Ui_Ring_In_Delay_Time[Ui_Speed_Mode][Ui_Ring_N]){
    LQ_SetServoDty(-Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][Ui_Ring_N] + SERVO_MID_PWM_VAL);
    Straightcnt = 0;
    //buzzer(1);
  }
  else if(RightRingFlag && (pitIsrCnt2 * 0.003 - temp1 * 0.003) < Ui_Ring_In_Servo_Time[Ui_Speed_Mode][Ui_Ring_N] && (pitIsrCnt2 * 0.003 - temp1 * 0.003) > Ui_Ring_In_Delay_Time[Ui_Speed_Mode][Ui_Ring_N]){
    LQ_SetServoDty(Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][Ui_Ring_N] + SERVO_MID_PWM_VAL);
    Straightcnt = 0;
    //buzzer(1);
  }
  else if(Laser_Finish_Flag && (pitIsrCnt2 * 0.003 - temp3 * 0.003) > Ui_Stop_Delay_Time[Ui_Speed_Mode]){
    if((pitIsrCnt2 * 0.003 - temp3 * 0.003) < Ui_Stop_Time[Ui_Speed_Mode]){
      LQ_SetServoDty(Ui_Out_Garage_Servo_Duty + SERVO_MID_PWM_VAL);
      //Finish_Stop_Flag = true;
    }
    else
      LQ_SetServoDty(SERVO_MID_PWM_VAL);
  }
  else{
  adcProcessAndGetValue();
  LQ_SetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL);
  //Lost_Line_Flag = false;
  }
}

void deal_enc(Queue *queue){
  
  uint8 i;
  queue->currentEncValLeft = getEncoderValueLeft();
  queue->currentEncValRight = getEncoderValueRight();
  
//  for(i = ENCODER_FIFO_LENGTH - 1;i > 0;i --)
//  queue->encoder_cnt[i] = queue->encoder_cnt[i-1];
//  queue->encoder_cnt[0] = (int16_t)((queue->currentEncValLeft + queue->currentEncValRight) / 2);
}

void deal_adc(Queue *queue){
        updateLeftAdcVal();
        updateRightAdcVal();
        updateLMidAdcVal();
        updateRMidAdcVal();
        updateMidLeftAdcVal();
        updateMidRightAdcVal();
        
        queue->once_uni_AD[0] = leftAdcVal/Ui_Max_Left_ADC_VAL;
        queue->once_uni_AD[1] = MidleftAdcVal/Ui_Max_MLeft_ADC_VAL;//��ֱ���
        queue->once_uni_AD[2] = LMidAdcVal/Ui_Max_LMid_ADC_VAL;
        queue->once_uni_AD[3] = RMidAdcVal/Ui_Max_RMid_ADC_VAL;
        queue->once_uni_AD[4] = MidrightAdcVal/Ui_Max_MRight_ADC_VAL;//��ֱ���
        queue->once_uni_AD[5] = rightAdcVal/Ui_Max_Right_ADC_VAL;
        
        float sum16_34 = queue->once_uni_AD[0] + queue->once_uni_AD[2] + queue->once_uni_AD[3] + queue->once_uni_AD[5];
        queue->twice_uni_AD[0] = queue->once_uni_AD[0] / sum16_34;
        queue->twice_uni_AD[2] = queue->once_uni_AD[2] / sum16_34;
        queue->twice_uni_AD[3] = queue->once_uni_AD[3] / sum16_34;
        queue->twice_uni_AD[5] = queue->once_uni_AD[5] / sum16_34;
        
        
        queue->dis_err_ori = Ui_fit_LR * (queue->twice_uni_AD[0] - queue->twice_uni_AD[5]) + Ui_fit_MLR * (queue->twice_uni_AD[2] - queue->twice_uni_AD[3]);
        
        //queue->dis_err_ori = Ui_fit_LR * (queue->twice_uni_AD[0] - queue->twice_uni_AD[5]) + Ui_fit_MLR * (queue->twice_uni_AD[2] - queue->twice_uni_AD[3]) + 1.0f;//old_car
        
        queue->dis_err = queue->dis_err_ori;
        
        if(queue->dis_err < Ui_Dis_Err_Min)
        queue->dis_err = Ui_Dis_Err_Min;
        else if(queue->dis_err > Ui_Dis_Err_Max)
        queue->dis_err = Ui_Dis_Err_Max;
//        if(txOnGoing == false)//û�ڴ���
//        {
//            ANO_DT_send_int16( (short)(queue->dis_err * 100), 0, 0, 0, 0, 0, 0, 0);
//        }
}

void Test_PIT(void)
{ 
    LQ_PIT_Init(kPIT_Chnl_2, 1000000);    //1000 000us һ���ж�
    LQ_PIT_Init(kPIT_Chnl_3, 2000000);    //2000 000us һ���ж�
//    uint32_t temp1;
//    uint32_t temp2;
//    temp1 = PIT->CHANNEL[kPIT_Chnl_2].CVAL;
//    temp2 = PIT->CHANNEL[kPIT_Chnl_3].CVAL;
    while (true)
    {
        /* Check whether occur interupt and toggle LED */
         if (pitIsrCnt2)
        {
            //printf("\r\n PITͨ��2�ж��ѷ���!");
            //LED_Color(blue);   //�̵� 
          PT2_22 = !PT2_22;
            pitIsrCnt2=0;
        }
         if (pitIsrCnt3)
        {
            //printf("\r\n PITͨ��3�ж��ѷ���!");
            //LED_Color(red);   //���� 
          PT2_25 = !PT2_25;
            pitIsrCnt3=0;
        }
    }
}


inline void LQ_SetMotorDty(uint8_t num, short duty)  
{
    assert(num < 5);
    
    switch(num)
    {
        case 1:
            if(duty >= 0)  
            {
                LQ_PWMA_B_SetDuty(PWM1, kPWM_Module_3, 0, duty);
            }
            else
            {
                LQ_PWMA_B_SetDuty(PWM1, kPWM_Module_3, -duty, 0);
            }
            break;
            
        case 2:
            if(duty >= 0)  
            {
                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_3, duty, 0);
//                LQ_PWM_SetDuty(PWM2,kPWM_Module_1,kPWM_PwmA,duty);
//                LQ_PWM_SetDuty(PWM2,kPWM_Module_0,kPWM_PwmA,0);
            }
            else
            {
                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_3, 0, -duty);
//                  LQ_PWM_SetDuty(PWM2,kPWM_Module_0,kPWM_PwmA,-duty);
//                  LQ_PWM_SetDuty(PWM2,kPWM_Module_1,kPWM_PwmA,0);
            }
            break;
            
        case 3:
            if(duty >= 0)  
            {
                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_1, 0, duty);
            }
            else
            {
                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_1, -duty, 0);
            }
            break;
            
        case 4:
            if(duty >= 0)  
            {
                LQ_PWM_SetDuty(PWM2,    kPWM_Module_2, kPWM_PwmB , 0);
                LQ_PWM_SetDuty(PWM1,    kPWM_Module_1, kPWM_PwmA , duty);
            }
            else
            {
                LQ_PWM_SetDuty(PWM2,    kPWM_Module_2, kPWM_PwmB , -duty);
                LQ_PWM_SetDuty(PWM1,    kPWM_Module_1, kPWM_PwmA , 0);
            }
            break;
    }
  
}

void LPUART4_IRQHandler(void)
{
	static uint8_t queueHead=0;
	static uint8_t buffer[9];
	if((LPUART4->STAT)&kLPUART_RxDataRegFullFlag) //�����ж�
	{
		//��ȡ���ݵ�����β��,ͬʱ����ͷ����
		buffer[queueHead] = LPUART_ReadByte(LPUART4);
		queueHead = (queueHead + 1) % 9;
		//����ͷ�Ƿ���֡ͷ
		if (buffer[queueHead] == 0x59 && buffer[(queueHead + 1) % 9] == 0x59) {
			//�����֡ͷ,���������
			//distance = (uint16_t)(buffer[(queueHead + 2) % 9] | buffer[(queueHead + 3) % 9] << 8);		
				tfMiniDistance = buffer[(queueHead + 2) % 9] + (buffer[(queueHead + 3) % 9] << 8);
				tfMiniStrength = buffer[(queueHead + 4) % 9] + (buffer[(queueHead + 5) % 9] << 8);
		}
        __DSB();                                                        //����ͬ������ָ��
	}				                              
}