#ifndef __LQ_KEY_H__
#define __LQ_KEY_H__
#include "stdint.h"
/**
  * @brief    ��ʼ������ĸ���ϵİ���
  *
  * @param    
  *
  * @return   
  *
  * @note     GPIO3_IO04 ------ �ܽ�P2   ----- >  ĸ�尴��K2
  * @note     GPIO2_IO27 ------ �ܽ�C13  ----- >  ĸ�尴��K0
  * @note     GPIO2_IO30 ------ �ܽ�C14  ----- >  ĸ�尴��k1
  *
  * @example  
  *
  * @date     2019/6/4 ���ڶ�
  */
void KEY_Init(void);

#define UP		1	//KEY1���º󷵻�ֵ
#define DOWN	        2	//KEY4���º󷵻�ֵ
#define ADD		3	//KEY2���º󷵻�ֵ
#define SUB 	        4	//KEY0���º󷵻�ֵ
#define OK		5	//KEY5���º󷵻�ֵ
#define ESC		6	//KEY3���º󷵻�ֵ

/**
  * @brief    ��ȡ����״̬
  *
  * @param    mode�� 0����֧��������;   1��֧��������;
  *
  * @return   0���ް�������  1��KEY0����  2:KEY1����  3:KEY2����
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/4 ���ڶ�
  */
uint8_t KEY_Read(uint8_t mode);


/**
  * @brief    ���԰���
  *
  * @param    
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/4 ���ڶ�
  */
void Test_KEY(void);



/*!
  * @brief    ����GPIIO�ⲿ�ж�
  *
  * @param    
  *
  * @return   
  *
  * @note     ����ĸ���ϲ�ͬ�İ��� ��ͬ��ɫ�ĵ���˸
  *
  * @example  
  *
  * @date     2019/8/19 ����һ
  */
void Test_ExtiKEY(void);



#endif 
