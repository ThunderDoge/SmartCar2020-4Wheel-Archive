#ifndef _PITIMER_H
#define _PITIMER_H
#include "sys.h"

#define   ERR_LENGTH20            40//����ƫ��Ķ��г���
#define   ENCODER_FIFO_LENGTH     30//�������������ֵ���г���Ϊ30
#define   ERR_10T_LENGTH          40//����10�ε�ƫ��40������
//#define   DIS_ERR_MIN             -12.4//������ߵ���Сƫ��ֵ
//#define   DIS_ERR_MAX             12.4//������ߵ����ƫ��ֵ

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//PIT��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern void PIT_CH0_Int_Init(u32 ldval);
extern inline void LQ_SetMotorDty(uint8_t num, short duty);
extern void Test_PIT(void);

extern volatile uint16_t pitIsrCnt2;



typedef struct Queue_
{
  /*-------------����ƫ��ֵ��ض���---------------------*/
  float         dis_err_ori;//δ�޷���ƫ��
  float         dis_err;//��ż������ƫ�����
  float         dis_err_fifo[ERR_LENGTH20];//�������ƫ���λcm
  float         once_uni_AD[6];//�����һ�������յĴ�������ѹ
  float         twice_uni_AD[6];//�洢�Թ�һ����������1,6,3,4��һ���������
  float         dis10T_fifo[40];//��¼400T�ڵ�ƫ�ÿ10T��¼һ��
  
  /*-------------ENCֵ��ض���--------------------------*/
  int16_t       currentEncValLeft;//��ǰ��ENCֵ
  int16_t       currentEncValRight;//��ǰ��ENCֵ
  int16_t       encoder_cnt[ENCODER_FIFO_LENGTH];//��¼��������ԭʼ������
  int16_t       encoder_cnt_left[ENCODER_FIFO_LENGTH];//��¼��������ߵ�ԭʼ������
  int16_t       encoder_cnt_right[ENCODER_FIFO_LENGTH];//��¼�������ұߵ�ԭʼ������
  bool          Cal_Speed_Flag;//ʹ�ܵ������TRUE/FALSE
  
}Queue;

typedef struct threshold_
{
  float            LOSE_LINE_SUM;//= 45//���嶪��ʱ������3�ʹ�����4�ĺ�С�����ֵ
  float            CROSS_VALUE;// ����ʮ�ֽ���ʱ����1,2,5,6֮�ʹ������ֵ 
  float            OUT_OF_WAY_VALUE;//��¼����1256��Ȩ��С�����ֵʱ���Ѿ�����ܵ�
  uint8_t          OUT_OF_WAY_SPEED;//����ܵ������������С�ٶ�
  uint8_t          Ramp_Speed;//�µ��ٶ�
}Threshold;

typedef enum Road_
{
  No_road,      //���س���ܵ����޷�У����ͣ��
  Out_of_way,   //�Ѿ�����ܵ�
  Lose_line,    //���ߣ������ж��Ե��ĸ���
  Cross,        //ʮ�ֽ���
  Straight_Devious,//ֱ�����
  Need_judge,//Ϊ�˳��򷽱����õģ���ʾ��Ҫ����һ�����ж�
  Ramp,//����
  LeftRing,//��Բ��
  LaserFinish,//��������־
  HallFinish,//��������־
  OutGarage,//�����־
  RightRing//��Բ��
}Road;

//extern Queue queue;
//extern Threshold threshold;

#endif
