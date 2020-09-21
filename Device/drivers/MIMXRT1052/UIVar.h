#ifndef __UI_VAR_H__
#define __UI_VAR_H__
#include "stdbool.h"
#include "stdint.h"

#define MAX_RUNT_TO_MILEAGE (200)

extern volatile short Ui_Speed_Mode;

//uiCamPage
extern volatile short Ui_Control_Point;
extern volatile short Ui_Fifo_Sum;
extern volatile float Ui_Cam_D;
extern volatile unsigned short Ui_Find_Range;
extern volatile float Ui_Cam_P_More;
extern volatile short Ui_Laser_Threshold;
extern volatile short Ui_CenterUnfound_Speed;
extern volatile short Ui_Servo_Mid_Pwm;

//uiSpeedPage
extern volatile float Ui_Motor_P;
extern volatile float Ui_Motor_I;
extern volatile float Ui_Motor_D;
extern volatile short Ui_Uk_To_Pwm;
extern volatile uint16_t Ui_Max_Speed[5];
extern volatile uint16_t Ui_Min_Speed[5];
extern volatile uint16_t Ui_SMin_Speed[5];
extern volatile uint16_t Ui_Cur_Speed[5];
extern volatile int Ui_Inner_Max_Speed[5];
extern volatile short Ui_Decelerate_K;
extern volatile float Ui_Straight_Diff_K[5];
extern volatile float Ui_Bend_Diff_K[5];
extern volatile float Ui_Motor_Threshold;
extern volatile float Ui_MotorD_Threshold;
extern volatile short Ui_Ring_In_Max_Speed;
extern volatile short Ui_Ring_In_Min_Speed;
extern volatile short Ui_Out_Garage_Max_Speed;
extern volatile short Ui_Out_Garage_Min_Speed;
extern volatile short Ui_Ramp_Up_Speed;
extern volatile short Ui_Ramp_Dowm_Speed;
extern volatile short Ui_Stop_Left_Speed[5];
extern volatile short Ui_Stop_Right_Speed[5];
extern volatile short Ui_Hall_Stop_Speed[5];

extern volatile bool Ui_Stop_Flag;
extern volatile bool Turn_Back_Flag;
extern volatile bool Bend_Diff_Flag;
extern volatile bool Ui_Test_Adc_Flag;
extern volatile bool Ui_Save_Adc_Flag;
extern volatile bool Fixed_Finish_Flag;
extern volatile float BatV;
extern volatile uint8_t KeyMode;
//extern volatile uint16_t temp2;

//uiAdcPage
extern volatile float Ui_Adc_SP[5];
extern volatile float Ui_Adc_SD[5];
extern volatile float Ui_Adc_BP[5];
extern volatile float Ui_Adc_BD[5];
extern volatile float Ui_Adc_P_Max;
extern volatile float Ui_Adc_D_Max;
extern volatile float Ui_MAdc_P;
extern volatile float Ui_MAdc_D;
extern volatile float Ui_Ramp_Up_Adc;
extern volatile int Ui_RunT_To_Mileage[MAX_RUNT_TO_MILEAGE]; //������
extern volatile float Ui_Max_Left_ADC_VAL;
extern volatile float Ui_Max_Right_ADC_VAL;
extern volatile float Ui_Ring_In_Adc;
extern volatile float Ui_Ring_Out_Adc;
extern volatile float Ui_Turn_Adc;
extern volatile float Ui_Ring_Turn_In_End_Radios; //���������׶ν�������ֵ
extern volatile int Ui_Before_Enter_Ring_Cnts[6]; //������ʱ
extern volatile int Ui_Ring_Turn_In_Pwms[6]; //���������׶εĴ��
extern volatile int Ui_Ring_Inside_Min_Pwms[6]; //����+�����׶ε���С���
extern volatile float Ui_Adc_Thr;
extern volatile float Ui_MAdc_Thr;
extern volatile float Ui_Adc_Valid_Threshold;
extern volatile float Ui_Max_MLeft_ADC_VAL;
extern volatile float Ui_Max_MRight_ADC_VAL;
extern volatile float Ui_Max_LMid_ADC_VAL;
extern volatile float Ui_Max_RMid_ADC_VAL;

//uiMainConPage
extern volatile float Ui_Ring_Shield_Time[5][5];
extern volatile float Ui_Ring_In_Servo_Time[5][5];
extern volatile short Ui_Ring_In_Servo_Duty[5][5];//Բ�����
extern volatile bool Ui_Ring_Directions[5][5]; //Բ������
extern volatile float Ui_Ring_In_Delay_Time[5][5];
extern volatile float Ui_Out_Garage_Time;
extern volatile int Ui_Out_Garage_Servo_Duty;
extern volatile float Ui_Finish_Shield_Time[5];
extern volatile float Ui_Ramp_Time;
extern volatile float Ui_Ramp_Up_Time;
extern volatile short Ui_Finish_Servo_Duty;
extern volatile float Ui_Stop_Time[5];
extern volatile float Ui_Back_Time[5];
extern volatile short Ui_Cross_Time;
extern volatile int Ui_Ring_Max_Speed[5][5];
extern volatile int Ui_Ring_Min_Speed[5][5];
extern volatile float Ui_Finish_Back_Time;
extern volatile int Ui_Ring_N; //Բ������
extern volatile int Ui_Ring_Radii[10]; //Բ���뾶
extern volatile int Ui_Ring_Set_Speeds[6]; //Բ��Ԥ���ٶ�
extern volatile int Ui_Ring_Shielding_Cnts[6]; //������ͬ�����׶εļ�ʱ
extern volatile float Ui_Ring_CamPs[6];
extern volatile float Ui_Stop_Delay_Time[5];

extern volatile float Ui_fit_LM;
extern volatile float Ui_fit_RM;
extern volatile float Ui_fit_LR;
extern volatile float Ui_fit_MLR;
extern volatile float Ui_Dis_Err_Min;//������Ͼ�����Сƫ��
extern volatile float Ui_Dis_Err_Max;//������Ͼ������ƫ��


extern volatile int Ui_Record_Current_Img_Index;

//ֱ��д��,���ڷ���ʱд��
extern void writeUiVarsToFlash(void);
//oledѯ���Ƿ�д��,���³����һ������ʱд��
extern void tryWirteUiVarsToFlash(void);
//��������������
extern void tryReadUiVarsFromFlash(void);
//ControlElement��,���ڼ�¼��ʻ����ÿ��ʱ�̶�Ӧ��·����Ϣ
extern void addRunInfoToUi(int globalRuningCnt, int globalRuningMileage);
extern int getMileageByUiRunT(void);

#endif