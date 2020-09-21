#ifndef __INCLUDE_H
#define __INCLUDE_H

#define EnableInterrupts  __asm(" CPSIE i");//�����ж�
#define DisableInterrupts __asm(" CPSID i");//�����ж�

//#define LQMT9V034    //ѡ��ʹ����������ͷģ��
#define LQOV7725   //����ѡ��һ

#ifdef LQMT9V034 //LQMT9V034ģ��
#define CAMERA_FPS  0
#define APP_CAMERA_WIDTH  (IMAGEW)
#define APP_CAMERA_HEIGHT (IMAGEH/2)  //һ��uint16_t����װ���������أ� һ��IMAGEW ������ʵװ�����е����ص㣬���Ը߶�/2
#define APP_BPP 2 //���ظ�ʽ��
#define  Use_ROWS  60    //ʹ��ͼ��߶�
#define  Use_Line  94    //ʹ��ͼ����
#else            //LQOV7725ģ��
//#define LQOV7725RGB   //ʹ��7725RGB   
#define LQOV7725YUV     //ʹ��7725�Ҷ�
#define CAMERA_FPS  100
#define APP_CAMERA_WIDTH  320
#define APP_CAMERA_HEIGHT 240

#define APP_BPP 2 //���ظ�ʽ
#define  Use_ROWS  30    //ʹ��ͼ��߶�
#define  Use_Line  40    //ʹ��ͼ����
#endif

/* ������Ҫ���һЩȫ�ֺ궨�� */

/* ��ʾ��Ļ */
#define OLED
//#define TFT1_8


/* MPU6050   ֻ��ʹ�� Ӳ��IIC �����IIC */
/* ICM20602  ��ʹ�� Ӳ��IIC �����IIC  Ӳ��SPI �����SPI */
/* �������  ֻ��ʹ�� Ӳ��IIC �����IIC */
/* �궨�� ʹ�ò�ͬ�Ľ��߷�ʽ */
#define USE_HAL_SPI  0
#define USE_SOFT_SPI 1
#define USE_SOFT_IIC 2
#define USE_HAL_IIC  3

/* ѡ��ʹ��Ӳ��IIC */
//#define IMU_USE_SELECT  USE_HAL_IIC



/* ����������λ��ʹ�� ���� DMA���䷽ʽ */
//#define ANO_DT_USE_DMA  1

#endif
