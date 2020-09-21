#include "pitimer.h"
#include "LQ_LED.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//PIT定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/4/25
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
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
bool    LeftRingFlag       = false;              //圆环执行标志
bool    RightRingFlag      = false;
bool    Ramp_Flag          = false;
bool    Finish_Flag        = false;
bool    Laser_Finish_Flag  = false;
bool    Hall_Finish_Flag   = false;
bool    Out_Garage_Flag    = false;
bool    Lost_Line_Flag     = false;  
bool    Stop_Flag          = false;
extern bool    adcStopEnable;
uint16  temp1          = 60000;                 //圆环屏蔽中间时间
uint16  temp2          = 60000;                 //坡道时间
uint16  temp3          = 60000;                 //入库打角时间
uint16  Rampcnt        = 0;                     //坡道
uint16  Ringcnt        = 0;                     //圆环自锁，出环一段时间后恢复
uint16  Finishcnt      = 0;                     //入库自锁，不恢复
uint16_t tfMiniDistance = 0;
uint16_t tfMiniStrength = 0;

//初始化PIT通道0,PIT时钟源为perclk_clk_root=75MHz.
//ldval: CH0初始值,0~0xFFFFFFFF
//递减计数,当ldval==0时,产生中断.
//定时时间=ldval/PERCLK_CLK_ROOT
//void PIT_CH0_Int_Init(void)
//{
//    LQ_PIT_Init(kPIT_Chnl_1, 5000);
////    PIT_GetDefaultConfig(&pit_config);  //初始化为默认配置
////    pit_config.enableRunInDebug=true;   //调试模式下PIT继续运行
////    PIT_Init(PIT,&pit_config);          //初始化PIT定时器
////    
////    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0,ldval);//设置倒计时初始值
////    PIT_EnableInterrupts(PIT,kPIT_Chnl_0,kPIT_TimerInterruptEnable);//使能中断
////    NVIC_SetPriority(PIT_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),4,0));	//抢占优先级6，子优先级0
////	  EnableIRQ(PIT_IRQn);	                //使能PIT中断
////    PIT_StartTimer(PIT,kPIT_Chnl_0);        //打开PIT
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
    /* 判断PIT通道0发生中断.*/
    
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_1)&kPIT_TimerFlag)==kPIT_TimerFlag)  
    {
        /* 清除标志位.*/
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
        /*用户可自行添加代码*/
        //GPIO_PinWrite(GPIO3,13,1U);
      if(!Ui_Stop_Flag || Ui_Test_Adc_Flag){
        /*--------------处理电感-----------------------*/
        deal_adc(&queue);
        
        /*--------------获取ENC值----------------------*/
        deal_enc(&queue);
        
        /*--------------处理赛道元素及控制-------------*/
        road = road_condition(&queue,&threshold);
        //renew_dis10T_fifo(&queue);
        
        //更新purpostEncVal
	updatePurpostEncVal();
        
        car_ctrl(road,&queue,&threshold);
      //}
        
        //purpostEncVal = 80;
	//purpostEncValLeft = 80;
	//purpostEncValRight = 80;

        //获得左右的控制量(encVal)
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
        /* 清除标志位.*/
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);    
        /*用户可自行添加代码*/
        //Test_CCD();
        CCD_Collect();
        //MPos = Adc_Max_Position();
        
    }
    if((PIT_GetStatusFlags(PIT,kPIT_Chnl_3)&kPIT_TimerFlag)==kPIT_TimerFlag)  
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);   
        
        /*用户可自行添加代码*/        
        pitIsrCnt3++;
    }
    __DSB();
}
/*-----------------------判断道路状况------------------------*/
Road road_condition(Queue *queue , Threshold *threshold){
  float         sum34        = 0.0;     //记录传感器34的值之和
  float         sum_err      = 0.0;     //保存最近ERR_SUM_LEN个偏差距离之和
  float         sum1256      = 0.0;     //记录传感器1,2,5,6的值之和
  float         sum_1346     = 0.0;     //记录传感1,3,4,6之和，用于判断丢线，针对十字环
  float         Ramp_sum     = 0.0;     //用于判断坡道的电感值
  uint8         i;                      //累加距离偏差的时候用
  
  static uint16  Bend_Time      = 0;    //记录跑道外面去的时间，超过一定值说明已经不能校正，就停车
  static uint16  Non_Bend_Time  = 0;    //记录不满足弯道条件的次数
  static uint16  cross_time     = 0;    //记录探测出的十字交叉出现的次数
  static uint16  non_cross_time = 0;    //记录不满足十字条件的次数
  //static uint16  Ramp_Time      = 0;    //坡道判断次数
  Road road;
/*-----------------------------移动更新偏差队列----------------------------*/
  for(i = ERR_LENGTH20-1;i>0;i--)
    queue->dis_err_fifo[i] = queue->dis_err_fifo[i-1];
  
  sum_1346 = queue->once_uni_AD[0] + queue->once_uni_AD[5]
    + queue->once_uni_AD[2] + queue->once_uni_AD[3];
  sum34 = queue->once_uni_AD[2] + queue->once_uni_AD[3];
  Ramp_sum = queue->twice_uni_AD[2] + queue->twice_uni_AD[3] 
    - queue->twice_uni_AD[0] - queue->twice_uni_AD[5];
  /*-----------------------------出库-----------------------------------*/
  if(pitIsrCnt2 * 0.003 < Ui_Out_Garage_Time)
    return OutGarage;
  else if(Out_Garage_Flag)
  {
    Out_Garage_Flag = false;
  }
  /*----------------------------圆环判断---------------------------------*/
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
  /*---------------------------坡道判断---------------------------------*/
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
  /*-----------------------------------丢线判断------------------------------*/
//  sum_1346 = queue->once_uni_AD[0] + queue->once_uni_AD[5]
//    + queue->once_uni_AD[2] + queue->once_uni_AD[3];
  if(sum_1346 < threshold->LOSE_LINE_SUM && !Ramp_Flag)
  {
    Bend_Time++;
//    if(!Ramp_Flag || !LeftRingFlag || !RightRingFlag)
//    buzzer(0);
    //LED_Color(cyan);
    sum_err = 0.0;//清零偏差和
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
      queue->dis_err_fifo[0] = Ui_Dis_Err_Min;//舵机转角到反向最大
    else if(sum_err > 0.0 || LeftRingFlag)
      queue->dis_err_fifo[0] = Ui_Dis_Err_Max;//舵机转角到最大
            return Lose_line;//丢线
  }
  else
  {
    Bend_Time = 0;
    Lost_Line_Flag = false;
  }
  /*-----------------------------十字判断------------------------------------*/
  if(queue->once_uni_AD[1] * queue->once_uni_AD[4] > threshold->CROSS_VALUE && !LeftRingFlag && !RightRingFlag)
  {
    Straightcnt = 0;
    cross_time ++;
    if(cross_time > 8)//连续5次都是十字交叉则判定为十字交叉
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
  //退出十字过程
  else if(cross_time > 15)//说明之前确实是十字，则判断这次的不满足是不是干扰
  {
    non_cross_time ++;
    if(non_cross_time > Ui_Cross_Time)//连续10次都不是十字，说明确实退出了十字
    {
      buzzer(0);
      non_cross_time = 0;
      cross_time = 0;//清除异常的值
    }
    else//依然认为现在是在验证退出十字，还当成十字处理
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
    
    cross_time = 0;//说明不是进十字也不是退出十字，是一直都不是十字
    non_cross_time = 0;
  }
  /*-----------------------------斑马线判断----------------------------------*/
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
 /*---------------------------是普通非特殊路况，正常PID校正------------------*/
  queue->dis_err_fifo[0] = queue->dis_err;//更新偏差队列
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
    /*------------------------------圆环处理-----------------------------------*/
    case LeftRing:
      //buzzer(1);
      LeftRingFlag = true;
      break;
    case RightRing:
      //buzzer(1);
      RightRingFlag = true;
      break;
    /*------------------------------坡道处理-----------------------------------*/
    case Ramp:
      //buzzer(1);
      Ramp_Flag = true;
      //queue->Cal_Speed_Flag = false;
      //purpostEncVal = threshold->Ramp_Speed;
      break;
    /*------------------------------入库处理-----------------------------------*/
    case LaserFinish:
      LED_Color(green);
      Laser_Finish_Flag = true;
      break;
    case HallFinish:
      LED_Color(green);
      Hall_Finish_Flag = true;
      break;
    /*------------------------------异常处理-----------------------------------*/ 
//    case Out_of_way:
//      queue->Cal_Speed_Flag = false;
//      purpostEncVal = threshold->OUT_OF_WAY_SPEED;
//      break;
    /*----------------------极端异常，冲出跑道很久且不可校正，则停车-----------*/
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
        queue->once_uni_AD[1] = MidleftAdcVal/Ui_Max_MLeft_ADC_VAL;//竖直电感
        queue->once_uni_AD[2] = LMidAdcVal/Ui_Max_LMid_ADC_VAL;
        queue->once_uni_AD[3] = RMidAdcVal/Ui_Max_RMid_ADC_VAL;
        queue->once_uni_AD[4] = MidrightAdcVal/Ui_Max_MRight_ADC_VAL;//竖直电感
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
//        if(txOnGoing == false)//没在传输
//        {
//            ANO_DT_send_int16( (short)(queue->dis_err * 100), 0, 0, 0, 0, 0, 0, 0);
//        }
}

void Test_PIT(void)
{ 
    LQ_PIT_Init(kPIT_Chnl_2, 1000000);    //1000 000us 一次中断
    LQ_PIT_Init(kPIT_Chnl_3, 2000000);    //2000 000us 一次中断
//    uint32_t temp1;
//    uint32_t temp2;
//    temp1 = PIT->CHANNEL[kPIT_Chnl_2].CVAL;
//    temp2 = PIT->CHANNEL[kPIT_Chnl_3].CVAL;
    while (true)
    {
        /* Check whether occur interupt and toggle LED */
         if (pitIsrCnt2)
        {
            //printf("\r\n PIT通道2中断已发生!");
            //LED_Color(blue);   //绿灯 
          PT2_22 = !PT2_22;
            pitIsrCnt2=0;
        }
         if (pitIsrCnt3)
        {
            //printf("\r\n PIT通道3中断已发生!");
            //LED_Color(red);   //蓝灯 
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
	if((LPUART4->STAT)&kLPUART_RxDataRegFullFlag) //接收中断
	{
		//读取数据到队列尾部,同时将队头出队
		buffer[queueHead] = LPUART_ReadByte(LPUART4);
		queueHead = (queueHead + 1) % 9;
		//检查队头是否是帧头
		if (buffer[queueHead] == 0x59 && buffer[(queueHead + 1) % 9] == 0x59) {
			//如果是帧头,则读出数据
			//distance = (uint16_t)(buffer[(queueHead + 2) % 9] | buffer[(queueHead + 3) % 9] << 8);		
				tfMiniDistance = buffer[(queueHead + 2) % 9] + (buffer[(queueHead + 3) % 9] << 8);
				tfMiniStrength = buffer[(queueHead + 4) % 9] + (buffer[(queueHead + 5) % 9] << 8);
		}
        __DSB();                                                        //数据同步屏蔽指令
	}				                              
}