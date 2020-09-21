#include "UIVar.h"
#include "LQ_KEY.h"
#include "main.h"
#include "pitimer.h"

extern Threshold threshold;

volatile short Ui_Speed_Mode = 0;

//uiCamPage
volatile short Ui_Control_Point = 0;
volatile short Ui_Fifo_Sum = 20;
volatile float Ui_Cam_D = 1.00;
volatile unsigned short Ui_Find_Range = 15;
volatile float Ui_Cam_P_More = 0;
volatile short Ui_Laser_Threshold = 1300;
volatile short Ui_CenterUnfound_Speed = 50;
volatile short Ui_Servo_Mid_Pwm = 1107;
volatile short Ui_Cross_Time = 15;//150

//uiSpeedPage
volatile float Ui_Motor_P = 13.6;
volatile float Ui_Motor_I = 1.0;
volatile float Ui_Motor_D = 0.0;
volatile short Ui_Uk_To_Pwm = 50;
volatile uint16 Ui_Max_Speed[5] = {50,70,60,67,80};
volatile uint16 Ui_Min_Speed[5] = {50,55,60,67,60};
volatile uint16 Ui_SMin_Speed[5] = {50,65,60,67,70};
volatile uint16 Ui_Cur_Speed[5] = {50,50,60,67,60};
volatile int   Ui_Inner_Max_Speed[5] = {50,50,60,67,60};
volatile short Ui_Decelerate_K = 6;
volatile float Ui_Straight_Diff_K[5] = {0.20,0.15,0.10,0.10,0.10};
volatile float Ui_Bend_Diff_K[5] = {0.25,0.20,0.25,0.20,0.10};
volatile float Ui_Motor_Threshold = 20;
volatile float Ui_MotorD_Threshold = 20;
volatile short Ui_Ring_In_Max_Speed = 45;
volatile short Ui_Ring_In_Min_Speed = 35;
volatile short Ui_Out_Garage_Max_Speed = 50;
volatile short Ui_Out_Garage_Min_Speed = 30;
volatile short Ui_Ramp_Up_Speed = 40;
volatile short Ui_Ramp_Dowm_Speed = 50;
volatile short Ui_Stop_Left_Speed[5] = {50,50,50,50,50};
volatile short Ui_Stop_Right_Speed[5] = {0,0,0,0,0};
volatile short Ui_Hall_Stop_Speed[5] = {50};


//uiAdcPage
volatile float Ui_Adc_SP[5] = {1.45,2.50,2.50,2.50,3.00};//pre2.15
volatile float Ui_Adc_SD[5] = {8,30,40,50,70};//pre1.00
volatile float Ui_Adc_BP[5] = {7.58,8.00,7.90,8.10,8.45};//pre2.15
volatile float Ui_Adc_BD[5] = {436,350,301,300,300};//pre1.00
volatile float Ui_Adc_P_Max = 8.00;//pre2.15
volatile float Ui_Adc_D_Max = 3.50;//pre1.00
volatile float Ui_MAdc_P = 2.15;
volatile float Ui_MAdc_D = 1.00;
volatile float Ui_Ramp_Up_Adc = 0.50f; //待处理
volatile int   Ui_RunT_To_Mileage[MAX_RUNT_TO_MILEAGE] = {0}; //待处理
volatile float Ui_Max_Left_ADC_VAL = 2024;
volatile float Ui_Max_Right_ADC_VAL = 1919;
volatile float Ui_Ring_In_Adc = 5.05; 
volatile float Ui_Ring_Out_Adc = 2.00; 
volatile float Ui_Turn_Adc = 1.3;
volatile float Ui_Ring_Turn_In_End_Radios = 0.7; //进环打死阶段结束的阈值
volatile float Ui_Adc_Valid_Threshold = 0.20f;
volatile float Ui_Adc_Thr = 4.00;
volatile float Ui_MAdc_Thr = 20;
volatile float Ui_Max_MLeft_ADC_VAL = 50;//320;
volatile float Ui_Max_MRight_ADC_VAL = 40;//320;
volatile float Ui_Max_LMid_ADC_VAL = 1860;//1000;
volatile float Ui_Max_RMid_ADC_VAL = 1937;//1000;

//uiMainConPage
volatile float Ui_Ring_Shield_Time[5][5] = {  {1.75 , 0.00 , 0.00 , 0.00 , 0.00},
                                              {1.75 , 0.00 , 0.00 , 0.00 , 0.00},
                                              {1.71 , 0.00 , 0.00 , 0.00 , 0.00},
                                              {1.75 , 0.00 , 0.00 , 0.00 , 0.00},
                                              {1.71 , 0.00 , 0.00 , 0.00 , 0.00},
                                                                                  };
volatile float Ui_Ring_In_Servo_Time[5][5] = {  {0.28 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.32 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.30 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.20 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.20 , 0.00 , 0.00 , 0.00 , 0.00},
                                                                                  };
volatile short Ui_Ring_In_Servo_Duty[5][5] = {  {35 , 0 , 0 , 0 , 0},
                                                {48 , 0 , 0 , 0 , 0},
                                                {45 , 0 , 0 , 0 , 0} ,
                                                {53 , 0 , 0 , 0 , 0} ,
                                                {85 , 0 , 0 , 0 , 0} ,
                                                                                  };
volatile float Ui_Ring_In_Delay_Time[5][5] = {  {0.12 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.12 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.10 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.08 , 0.00 , 0.00 , 0.00 , 0.00},
                                                {0.09 , 0.00 , 0.00 , 0.00 , 0.00},
                                                                                  };
volatile float Ui_Out_Garage_Time = 0.35;
volatile int Ui_Out_Garage_Servo_Duty = 120;
volatile float Ui_Finish_Shield_Time[5] = {10.80 , 10.00 , 9.75 , 9.80 , 9.80};
volatile float Ui_Ramp_Time = 0.75;
volatile float Ui_Ramp_Up_Time = 0.40;
volatile short Ui_Finish_Servo_Duty = 90;
volatile float Ui_Stop_Time[5] = {0.45 , 0.43 , 0.32 , 0.34 , 0.30};
volatile float Ui_Back_Time[5] = {0.50};
volatile float Ui_Finish_Back_Time = 2.00;
volatile int Ui_Ring_N = 0; //圆环序数
volatile int Ui_Ring_Max_Speed[5][5] = {        {57 , 0 , 0 , 0 , 0},
                                                {60 , 0 , 0 , 0 , 0},
                                                {64 , 0 , 0 , 0 , 0} ,
                                                {60 , 0 , 0 , 0 , 0} ,
                                                {66 , 0 , 0 , 0 , 0} ,
                                                                                  };
volatile int Ui_Ring_Min_Speed[5][5] = {        {52 , 0 , 0 , 0 , 0},
                                                {56 , 0 , 0 , 0 , 0},
                                                {60 , 0 , 0 , 0 , 0} ,
                                                {57 , 0 , 0 , 0 , 0} ,
                                                {62 , 0 , 0 , 0 , 0} ,
                                                                                  };
volatile int Ui_Ring_Radii[10] = {0, 5, 0, 5, 0, 0, 0, 0, 0, 0}; //圆环半径
volatile int Ui_Ring_Set_Speeds[6] = {70, 72, 74, 76, 78, 80}; //圆环预设速度
volatile int Ui_Ring_Shielding_Cnts[6] = {20, 24, 28, 32, 36, 40}; //屏蔽相同环岛阶段的计时
volatile float Ui_Ring_CamPs[6] = {2.15,2.18,2.21,2.24,2.27,2.3};
volatile int Ui_Before_Enter_Ring_Cnts[6] = {2, 2, 2, 3, 3, 3}; //进环延时
volatile bool Ui_Ring_Directions[5][5] = {true, true, false, true, false, false, false, false, false, false}; //圆环方向
volatile int Ui_Ring_Turn_In_Pwms[6] = {89, 79, 69, 59, 49, 42}; //进环打死阶段的打角
volatile int Ui_Ring_Inside_Min_Pwms[6] = {70, 64, 58, 52, 46, 38}; //环中+出环阶段的最小打角
volatile float Ui_Stop_Delay_Time[5] = {0.12 , 0.13 , 0.02 , 0.06 , 0.05};


//volatile float Ui_fit_LM = 23.9670;//十字串道拟合曲线系数22.9197(old_car) 
//volatile float Ui_fit_RM = -26.6637;//十字串道拟合曲线系数-24.4731(old_car) 
//volatile float Ui_fit_LR = 34.6021;//直道拟合曲线系数28.9108(old_car)
//volatile float Ui_fit_MLR = -3.1405;//直道拟合曲线系数-9.2245(old_car)
volatile float Ui_fit_LM = 18.3671;//十字串道拟合曲线系数new_car
volatile float Ui_fit_RM = -17.3211;//十字串道拟合曲线系数new_car
volatile float Ui_fit_LR = 19.5908;//直道拟合曲线系数new_car
volatile float Ui_fit_MLR = 16.5920;//直道拟合曲线系数new_car
//volatile float Ui_fit_LM = 18.0010;//十字串道拟合曲线系数（40cm前瞻）
//volatile float Ui_fit_RM = -19.4450;//十字串道拟合曲线系数（40cm前瞻）
//volatile float Ui_fit_LR = 12.1485;//直道拟合曲线系数（40cm前瞻）
//volatile float Ui_fit_MLR = 19.8490;//直道拟合曲线系数（40cm前瞻）

volatile float Ui_Dis_Err_Min = -10;//曲线拟合距离最小偏差
volatile float Ui_Dis_Err_Max = 10;//曲线拟合距离最大偏差



volatile bool Ui_Stop_Flag = true;
volatile bool Turn_Back_Flag = false;
volatile bool Bend_Diff_Flag = false;
volatile bool Ui_Test_Adc_Flag = false;
volatile bool Ui_Save_Adc_Flag = false;
volatile bool Fixed_Finish_Flag = false;
volatile float BatV = 0;
volatile uint8_t KeyMode = 0;
//volatile uint16_t temp2 = 60000;






volatile int Ui_Record_Current_Img_Index = 0;

//UiSetToAdcPage
volatile int Ui_SetToAdc_N = 0;
volatile int Ui_SetToAdc_StartMileage[10] = {0,0,0,0,0,0,0,0,0,0};
volatile int Ui_SetToAdc_EndMileage[10] = {0,0,0,0,0,0,0,0,0,0};


void tryWirteUiVarsToFlash(void) {
	LCD_CLS();
	sprintf(getOledBuf(1), "Not Write to Flash?"); LCD_refreshOneLine(1, true);
	sprintf(getOledBuf(2), "ESC:1.NewDownload"); LCD_refreshOneLine(2, false);
	sprintf(getOledBuf(3), "    2.KeepOriSet"); LCD_refreshOneLine(3, false);
	sprintf(getOledBuf(4), "OK :  KeepPreSet"); LCD_refreshOneLine(4, false);
	sprintf(getOledBuf(5), "    OK  /  ESC"); LCD_refreshOneLine(5, true);
	while (true) {
		uint8 keyRes = KEY_Read(0);
		
		//按下取消键,跳出循环开始写入
		if (keyRes == ESC) {
			LCD_CLS();
                        sprintf(getOledBuf(4), " Are You Sure ?"); LCD_refreshOneLine(4, true);
                        while(1)
                        {
                          keyRes = KEY_Read(0);
                          if(keyRes == OK)
                            break;
                          if(keyRes == ESC)
                            return;
                        }
			break;
		}
		//按下确认键,直接退出函数
		if (keyRes == OK) {
			delayms(500);
			return;
		}
	}
	
	//开始写入
	writeUiVarsToFlash();
	
	//写入完成
        LCD_CLS();
	sprintf(getOledBuf(4), " writing finished !"); LCD_refreshOneLine(4, true);
	delayms(1000);
}

#define uiVariablesBufLen (1000)
//uint32_t uiVariablesBuf[uiVariablesBufLen];
int uiVariablesBuf[uiVariablesBufLen];

void writeUiVarsToFlash(void) {
    uiVariablesBuf[0] = Ui_Motor_P * 1000;
    uiVariablesBuf[1] = Ui_Motor_I * 1000;
    uiVariablesBuf[2] = Ui_Motor_D * 1000;
    uiVariablesBuf[3] = Ui_Fifo_Sum * 1000;
    uiVariablesBuf[4] = Ui_Cam_D * 1000;
    uiVariablesBuf[5] = Ui_Find_Range * 1000;
    uiVariablesBuf[6] = Ui_Cam_P_More * 1000;
    uiVariablesBuf[7] = Ui_Uk_To_Pwm * 1000;
    uiVariablesBuf[8] = Fixed_Finish_Flag * 1000;
    uiVariablesBuf[9] = Ui_CenterUnfound_Speed * 1000;
    uiVariablesBuf[10] = Ui_Speed_Mode * 1000;
    uiVariablesBuf[11] = threshold.CROSS_VALUE * 1000;
    //uiVariablesBuf[12] = Ui_Finish_Shield_Time * 1000;
    uiVariablesBuf[13] = Ui_Servo_Mid_Pwm * 1000;
    uiVariablesBuf[14] = Ui_Decelerate_K * 1000;
    uiVariablesBuf[15] = Ui_Cross_Time * 1000;
    uiVariablesBuf[16] = Ui_Laser_Threshold * 1000;
    uiVariablesBuf[17] = Ui_Motor_Threshold * 1000;
//    uiVariablesBuf[18] = Ui_Adc_SP * 1000;
//    uiVariablesBuf[19] = Ui_Adc_SD * 1000;
//    uiVariablesBuf[20] = Ui_Adc_BP * 1000;
//    uiVariablesBuf[21] = Ui_Adc_BD * 1000;
    uiVariablesBuf[22] = Ui_Adc_Thr * 1000;
    uiVariablesBuf[23] = Ui_Max_Left_ADC_VAL * 1000;
    uiVariablesBuf[24] = Ui_Max_Right_ADC_VAL * 1000;
    uiVariablesBuf[25] = Ui_Max_MLeft_ADC_VAL * 1000;
    uiVariablesBuf[26] = Ui_Max_MRight_ADC_VAL * 1000;
    uiVariablesBuf[27] = Ui_Max_LMid_ADC_VAL * 1000;
    uiVariablesBuf[28] = Ui_Max_RMid_ADC_VAL * 1000;
    uiVariablesBuf[29] = threshold.LOSE_LINE_SUM * 1000;
    uiVariablesBuf[30] = Ui_Control_Point * 1000;
    uiVariablesBuf[31] = Ui_Ring_In_Adc * 1000;
    uiVariablesBuf[32] = Ui_Out_Garage_Time * 1000;
    uiVariablesBuf[33] = Ui_Ring_In_Max_Speed * 1000;
    uiVariablesBuf[34] = Ui_Ring_In_Min_Speed * 1000;
//    for(int i = 35,j = 0; i < 55; i += 4,j++)
//    {
//      uiVariablesBuf[i] = Ui_Ring_Directions[j] * 1000;
//      uiVariablesBuf[i + 1] = Ui_Ring_In_Servo_Duty[j] * 1000;
//      uiVariablesBuf[i + 2] = Ui_Ring_Shield_Time[j] * 1000;
//      uiVariablesBuf[i + 3] = Ui_Ring_In_Servo_Time[j] * 1000;
//    }
    uiVariablesBuf[75] = Ui_Ramp_Time * 1000;
    uiVariablesBuf[76] = Ui_Ramp_Up_Time * 1000;
    uiVariablesBuf[77] = Ui_Ramp_Up_Speed * 1000;
    uiVariablesBuf[78] = Ui_Ramp_Dowm_Speed * 1000;
    uiVariablesBuf[79] = Ui_Ramp_Up_Adc * 1000;
    uiVariablesBuf[80] = Ui_Out_Garage_Servo_Duty * 1000;
    uiVariablesBuf[81] = Ui_MotorD_Threshold * 1000;
//    uiVariablesBuf[82] = Ui_SMin_Speed * 1000;
//    uiVariablesBuf[83] = Ui_Cur_Speed * 1000;
    //uiVariablesBuf[84] = Ui_Stop_Time * 1000;
    //uiVariablesBuf[85] = Ui_Stop_Left_Speed * 1000;
    //uiVariablesBuf[86] = Ui_Stop_Right_Speed * 1000;
    uiVariablesBuf[87] = Ui_Ring_Out_Adc * 1000;
    
//    for(int i = 88,j = 0; i< 98; i += 2,j++)
//    {
//      uiVariablesBuf[i] = Ui_Ring_Max_Speed[j] * 1000;
//      uiVariablesBuf[i + 1] = Ui_Ring_Min_Speed[j] * 1000;
//    }
    
    for(int i = 108,j = 0; i < 173; i += 13,j++)
    {
      uiVariablesBuf[i] = Ui_Max_Speed[j] * 1000;
      uiVariablesBuf[i + 1] = Ui_Min_Speed[j] * 1000;
      uiVariablesBuf[i + 2] = Ui_Inner_Max_Speed[j] * 1000;
      uiVariablesBuf[i + 3] = Ui_Straight_Diff_K[j] * 1000;
      uiVariablesBuf[i + 4] = Ui_Bend_Diff_K[j] * 1000;
      uiVariablesBuf[i + 5] = Ui_Adc_SP[j] * 1000;
      uiVariablesBuf[i + 6] = Ui_Adc_SD[j] * 1000;
      uiVariablesBuf[i + 7] = Ui_Adc_BP[j] * 1000;
      uiVariablesBuf[i + 8] = Ui_Adc_BD[j] * 1000;
      uiVariablesBuf[i + 9] = Ui_SMin_Speed[j] * 1000;
      uiVariablesBuf[i + 10] = Ui_Cur_Speed[j] * 1000;
      uiVariablesBuf[i + 11] = Ui_Stop_Left_Speed[j] * 1000;
      uiVariablesBuf[i + 12] = Ui_Stop_Right_Speed[j] * 1000;
    }
    
    for(int j = 0; j < 5; j++)
    {
      for(int i = 174 + j * 30,k = 0; i < 204 + j * 30; i += 6,k++)
      {
         uiVariablesBuf[i] = Ui_Ring_Directions[j][k] * 1000;
         uiVariablesBuf[i + 1] = Ui_Ring_In_Servo_Duty[j][k] * 1000;
         uiVariablesBuf[i + 2] = Ui_Ring_Shield_Time[j][k] * 1000;
         uiVariablesBuf[i + 3] = Ui_Ring_In_Servo_Time[j][k] * 1000;
         uiVariablesBuf[i + 4] = Ui_Ring_Max_Speed[j][k] * 1000;
         uiVariablesBuf[i + 5] = Ui_Ring_Min_Speed[j][k] * 1000;
      }
    }
    
    for(int i = 325,j = 0; i < 340; i += 3,j++)
    {
      uiVariablesBuf[i] = Ui_Stop_Time[j] * 1000;
      uiVariablesBuf[i + 1] = Ui_Back_Time[j] * 1000;
      uiVariablesBuf[i + 2] = Ui_Hall_Stop_Speed[j] * 1000;
    }
    
    for(int j = 0; j < 5; j++)
    {
      for(int i = 340 + j * 5,k = 0; i < 345 + j * 5; i += 1,k++)
      {
        uiVariablesBuf[i] = Ui_Ring_In_Delay_Time[j][k] * 1000;
      }
    }
    
    for(int i = 365,j = 0; i < 375; i += 2,j++)
    {
      uiVariablesBuf[i] = Ui_Finish_Shield_Time[j] * 1000;
      uiVariablesBuf[i + 1] = Ui_Stop_Delay_Time[j] * 1000;
    }
    
    W25QXX_Write((u8 *)uiVariablesBuf, 8*1024*1024, uiVariablesBufLen * 4);  
}

static void readUiVarsFromFlash(void) {
	W25QXX_Read((u8 *)uiVariablesBuf, 8*1024*1024, uiVariablesBufLen * 4);

    Ui_Motor_P = uiVariablesBuf[0] / 1000.0f;
    Ui_Motor_I = uiVariablesBuf[1] / 1000.0f;
    Ui_Motor_D = uiVariablesBuf[2] / 1000.0f;
    Ui_Fifo_Sum = uiVariablesBuf[3] / 1000.0f;
    Ui_Cam_D = uiVariablesBuf[4] / 1000.0f;
    Ui_Find_Range = uiVariablesBuf[5] / 1000.0f;
    Ui_Cam_P_More = uiVariablesBuf[6] / 1000.0f;
    Ui_Uk_To_Pwm = uiVariablesBuf[7] / 1000.0f;
    Fixed_Finish_Flag = uiVariablesBuf[8] / 1000.0f;
    Ui_CenterUnfound_Speed = uiVariablesBuf[9] / 1000.0f;
    Ui_Speed_Mode = uiVariablesBuf[10] / 1000.0f;
    threshold.CROSS_VALUE = uiVariablesBuf[11] / 1000.0f;
    //Ui_Finish_Shield_Time = uiVariablesBuf[12] / 1000.0f;
    Ui_Servo_Mid_Pwm =  uiVariablesBuf[13] / 1000.0f;
    Ui_Decelerate_K = uiVariablesBuf[14] / 1000.0f;
    Ui_Cross_Time = uiVariablesBuf[15] / 1000.0f;
    Ui_Laser_Threshold = uiVariablesBuf[16] / 1000.0f;
    Ui_Motor_Threshold = uiVariablesBuf[17] / 1000.0f;
//    Ui_Adc_SP = uiVariablesBuf[18] / 1000.0f;
//    Ui_Adc_SD = uiVariablesBuf[19] / 1000.0f;
//    Ui_Adc_BP = uiVariablesBuf[20] / 1000.0f;
//    Ui_Adc_BD = uiVariablesBuf[21] / 1000.0f;
    Ui_Adc_Thr = uiVariablesBuf[22] / 1000.0f;
    Ui_Max_Left_ADC_VAL = uiVariablesBuf[23] / 1000.0f;
    Ui_Max_Right_ADC_VAL = uiVariablesBuf[24] / 1000.0f;
    Ui_Max_MLeft_ADC_VAL = uiVariablesBuf[25] / 1000.0f;
    Ui_Max_MRight_ADC_VAL = uiVariablesBuf[26] / 1000.0f;
    Ui_Max_LMid_ADC_VAL = uiVariablesBuf[27] / 1000.0f;
    Ui_Max_RMid_ADC_VAL = uiVariablesBuf[28] / 1000.0f;
    threshold.LOSE_LINE_SUM = uiVariablesBuf[29] / 1000.0f;
    Ui_Control_Point = uiVariablesBuf[30] / 1000.0f;
    Ui_Ring_In_Adc = uiVariablesBuf[31] / 1000.0f;
    Ui_Out_Garage_Time = uiVariablesBuf[32] / 1000.0f;
    Ui_Ring_In_Max_Speed = uiVariablesBuf[33] / 1000.0f;
    Ui_Ring_In_Min_Speed = uiVariablesBuf[34] / 1000.0f;
//    for(int i =35,j=0; i < 55 ; i += 4, j++)
//    {
//      Ui_Ring_Directions[j] = uiVariablesBuf[i] / 1000.0f;
//      Ui_Ring_In_Servo_Duty[j] = uiVariablesBuf[i + 1] / 1000.0f;
//      Ui_Ring_Shield_Time[j] = uiVariablesBuf[i + 2] / 1000.0f;
//      Ui_Ring_In_Servo_Time[j] = uiVariablesBuf[i + 3] / 1000.0f;
//    }
    Ui_Ramp_Time = uiVariablesBuf[75] / 1000.0f;
    Ui_Ramp_Up_Time = uiVariablesBuf[76] / 1000.0f;
    Ui_Ramp_Up_Speed = uiVariablesBuf[77] / 1000.0f;
    Ui_Ramp_Dowm_Speed = uiVariablesBuf[78] / 1000.0f;
    Ui_Ramp_Up_Adc = uiVariablesBuf[79] / 1000.0f;
    Ui_Out_Garage_Servo_Duty = uiVariablesBuf[80] / 1000.0f;
    Ui_MotorD_Threshold = uiVariablesBuf[81] / 1000.0f;
//    Ui_SMin_Speed = uiVariablesBuf[82] / 1000.0f;
//    Ui_Cur_Speed = uiVariablesBuf[83] / 1000.0f;
    //Ui_Stop_Time = uiVariablesBuf[84] / 1000.0f;
    //Ui_Stop_Left_Speed = uiVariablesBuf[85] / 1000.0f;
    //Ui_Stop_Right_Speed = uiVariablesBuf[86] / 1000.0f;
    Ui_Ring_Out_Adc = uiVariablesBuf[87] / 1000.0f;
//    for(int i = 88,j = 0; i < 98 ; i += 2,j++)
//    {
//      Ui_Ring_Max_Speed[j] = uiVariablesBuf[i] / 1000.0f;
//      Ui_Ring_Min_Speed[j] = uiVariablesBuf[i + 1] / 1000.0f;
//    }
    
    for(int i = 108,j = 0; i < 173; i += 13,j++)
    {
      Ui_Max_Speed[j] = uiVariablesBuf[i] / 1000.0f;
      Ui_Min_Speed[j] = uiVariablesBuf[i + 1] / 1000.0f;
      Ui_Inner_Max_Speed[j] = uiVariablesBuf[i + 2] / 1000.0f;
      Ui_Straight_Diff_K[j] = uiVariablesBuf[i + 3] / 1000.0f;
      Ui_Bend_Diff_K[j] = uiVariablesBuf[i + 4] / 1000.0f;
      Ui_Adc_SP[j] = uiVariablesBuf[i + 5] / 1000.0f;
      Ui_Adc_SD[j] = uiVariablesBuf[i + 6] / 1000.0f;
      Ui_Adc_BP[j] = uiVariablesBuf[i + 7] / 1000.0f;
      Ui_Adc_BD[j] = uiVariablesBuf[i + 8] / 1000.0f;
      Ui_SMin_Speed[j] = uiVariablesBuf[i + 9] / 1000.0f;
      Ui_Cur_Speed[j] = uiVariablesBuf[i + 10] / 1000.0f;
      Ui_Stop_Left_Speed[j] = uiVariablesBuf[i + 11] / 1000.0f;
      Ui_Stop_Right_Speed[j] = uiVariablesBuf[i + 12] / 1000.0f;
    }
    for(int j = 0; j < 5; j++)
    {
      for(int i = 174 + j * 30,k = 0; i < 204 + j * 30; i += 6,k++)
      {
        Ui_Ring_Directions[j][k] = uiVariablesBuf[i] / 1000.0f;
        Ui_Ring_In_Servo_Duty[j][k] = uiVariablesBuf[i + 1] / 1000.0f;
        Ui_Ring_Shield_Time[j][k] = uiVariablesBuf[i + 2] / 1000.0f;
        Ui_Ring_In_Servo_Time[j][k] = uiVariablesBuf[i + 3] / 1000.0f;
        Ui_Ring_Max_Speed[j][k] = uiVariablesBuf[i + 4] / 1000.0f;
        Ui_Ring_Min_Speed[j][k] = uiVariablesBuf[i + 5] / 1000.0f;
      }
    }
    
    for(int i = 325,j = 0; i < 340; i += 3,j++)
    {
      Ui_Stop_Time[j] = uiVariablesBuf[i] / 1000.0f;
      Ui_Back_Time[j] = uiVariablesBuf[i + 1] / 1000.0f;
      Ui_Hall_Stop_Speed[j] = uiVariablesBuf[i + 2] / 1000.0f;
    }
    
    for(int j = 0; j < 5; j++)
    {
      for(int i = 340 + j * 5,k = 0; i < 345 + j * 5; i += 1,k++)
      {
        Ui_Ring_In_Delay_Time[j][k] = uiVariablesBuf[i] / 1000.0f;
      }
    }
    
    for(int i = 365,j = 0; i < 375; i += 2,j++)
    {
      Ui_Finish_Shield_Time[j] = uiVariablesBuf[i] / 1000.0f;
      Ui_Stop_Delay_Time[j] = uiVariablesBuf[i + 1] / 1000.0f;
    }
    
}

void tryReadUiVarsFromFlash(void) {
	LCD_CLS();
	
	//开始读出
	readUiVarsFromFlash();
}

void addRunInfoToUi(int globalRuningCnt, int globalRuningMileage) {
          //根据PIT提供的5ms计时
          uint16_t Ui_Current_RunT = globalRuningCnt*pitIsrCnt2*0.005;
          //存入路程
          Ui_RunT_To_Mileage[Ui_Current_RunT] = globalRuningMileage;
}

//UI界面显示用,用于根据运行时间查询对应的路程信息
//int getMileageByUiRunT(void) {
//	int index = Ui_Current_RunT;
//	//不能越界
//	if (index >= 0 && index < MAX_RUNT_TO_MILEAGE) {
//		return Ui_RunT_To_Mileage[index];
//	}
//	else
//		return 0;
//}