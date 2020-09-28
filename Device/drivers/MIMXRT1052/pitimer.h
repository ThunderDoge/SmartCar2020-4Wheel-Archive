#ifndef _PITIMER_H
#define _PITIMER_H
#include "sys.h"

#define   ERR_LENGTH20            40//保存偏差的队列长度
#define   ENCODER_FIFO_LENGTH     30//定义编码器脉冲值队列长度为30
#define   ERR_10T_LENGTH          40//保存10次的偏差40个数据
//#define   DIS_ERR_MIN             -12.4//拟合曲线的最小偏差值
//#define   DIS_ERR_MAX             12.4//拟合曲线的最大偏差值

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//PIT定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern void PIT_CH0_Int_Init(u32 ldval);
extern inline void LQ_SetMotorDty(uint8_t num, short duty);
extern void Test_PIT(void);

extern volatile uint16_t pitIsrCnt2;



typedef struct Queue_
{
  /*-------------赛道偏差值相关队列---------------------*/
  float         dis_err_ori;//未限幅的偏差
  float         dis_err;//存放计算出的偏差距离
  float         dis_err_fifo[ERR_LENGTH20];//储存距离偏差，单位cm
  float         once_uni_AD[6];//储存归一化后最终的传感器电压
  float         twice_uni_AD[6];//存储对归一化后数据再1,6,3,4归一化后的数据
  float         dis10T_fifo[40];//记录400T内的偏差，每10T记录一个
  
  /*-------------ENC值相关队列--------------------------*/
  int16_t       currentEncValLeft;//当前左ENC值
  int16_t       currentEncValRight;//当前右ENC值
  int16_t       encoder_cnt[ENCODER_FIFO_LENGTH];//记录编码器的原始脉冲数
  int16_t       encoder_cnt_left[ENCODER_FIFO_LENGTH];//记录编码器左边的原始脉冲数
  int16_t       encoder_cnt_right[ENCODER_FIFO_LENGTH];//记录编码器右边的原始脉冲数
  bool          Cal_Speed_Flag;//使能电机计算TRUE/FALSE
  
}Queue;

typedef struct threshold_
{
  float            LOSE_LINE_SUM;//= 45//定义丢线时传感器3和传感器4的和小于这个值
  float            CROSS_VALUE;// 定义十字交叉时传感1,2,5,6之和大于这个值 
  float            OUT_OF_WAY_VALUE;//记录传感1256加权和小于这个值时车已经冲出跑道
  uint8_t          OUT_OF_WAY_SPEED;//冲出跑道后减到极限最小速度
  uint8_t          Ramp_Speed;//坡道速度
}Threshold;

typedef enum Road_
{
  No_road,      //严重冲出跑道且无法校正则停车
  Out_of_way,   //已经冲出跑道
  Lose_line,    //丢线，用于判断旁道的干扰
  Cross,        //十字交叉
  Straight_Devious,//直道弯道
  Need_judge,//为了程序方便设置的，表示需要更进一步的判断
  Ramp,//上坡
  LeftRing,//左圆环
  LaserFinish,//激光入库标志
  HallFinish,//霍尔入库标志
  OutGarage,//出库标志
  RightRing//右圆环
}Road;

//extern Queue queue;
//extern Threshold threshold;

#endif
