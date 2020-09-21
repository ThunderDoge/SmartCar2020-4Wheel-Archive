/*-----------------------------------------------------------------------------------------------------
��ƽ    ̨������K60���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0������Դ���룬�����ο�������Ը�
�������¡�2019��04��02��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
������ƽ̨��IAR 8.2
����    �ܡ���������CCD
��ע�����
-------------------------------------------------------------
CCD�ӿ�
              
GND            GND 
VDD            5V
AO             ADC0_SE16 
CLK            PTE2        
SI             PTE3               
-------------------------------------------------------------
-------------------------------------------------------------------------------------------------------*/
#include "include.h"
#include "LQ_CCD.h"
#include "stdint.h"
#include "LQ_LED.h"
#include "LQ_ADC.h"
#include "LQ_UART.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "LQ_12864.h"
#include "LQ_GPIO_Cfg.h"
#include "LQ_SYSTICK.h"

//#define CCD_A0   ADC1_CH11
//#define CCD_CLK  PTE2
//#define CCD_SI   PTE3

#define TSL_CLK  PT1_16   
#define TSL_SI   PT1_28   

uint16_t ccd_data[128];
/*------------------------------------------------------------------------------------------------------
����    ����Test_CCD
����    �ܡ�CCD���Ժ���
����    ������
���� �� ֵ����
��ʵ    ����Test_CCD();
��ע�����
--------------------------------------------------------------------------------------------------------*/

void SendImageData(void) 
{
   uint8_t i = 0;
   UART_PutChar(LPUART1,0x00); 
   UART_PutChar(LPUART1,0xff);
   UART_PutChar(LPUART1,0x01);
   UART_PutChar(LPUART1,0x00);
   
   for(i=0; i<128; i++)                        //����
   {
      UART_PutChar(LPUART1,ccd_data[i]>>8);   //���͸�8λ
      UART_PutChar(LPUART1,ccd_data[i]&0XFF); //���͸ߵ�8λ
   }
}

void Test_CCD(void)
{
	//LED_Init();
    //LQ_UART_Init(LPUART3, 115200);
	//LCD_Init();
    //LCD_CLS();
	//CCD_Init();
	//systime.init();
	
	//LCD_P8x16Str(15,0,"LQ CCD Test"); 
//    printf("\r\nLQ CCD Test");
//	
//	uint32_t now_time = systime.get_time_ms();
//	//static uint16_t ccd_buf[128] ={0};
//	static uint8_t count = 0;
//	while(1)
//	{
//		if(systime.get_time_ms() - now_time >= 50)                  //20֡
//		{
//			now_time = systime.get_time_ms();
//			
//			/* �ɼ�ccdͼ�� */
//			CCD_Collect();
//			
//			/* ���͵���λ�� */
//			if(count++%5 == 0)
//			{
//				UART_PutChar(LPUART3,'T');
//				for(uint8_t i=0; i<128; i++) 
//				{
//                                        uint8_t tmp = ccd_data[i];
//					UART_PutChar(LPUART3,(unsigned char)(ccd_data[i]>>8)); //���͸�8λ
//					UART_PutChar(LPUART3,(unsigned char)ccd_data[i]);      //���͸ߵ�8λ 
//                                        printf("\r\nLQ CCD Test");
//				}
//			}
//			
//			/* OLED ��ʾ */
//			if(count % 3 == 0)
//			{
//				for(uint8_t i=0; i<128; i++)
//				{
//					LCD_Set_Pos(i , 7); 
//					if(ccd_data[i] > 790)	      			
//						LCD_WrDat(0xFF);
//					else
//						LCD_WrDat(0x00);	
//				}
//			
//			}	
//			
//		}
//	}
  for(;;)
  {
    SendImageData();
  }
}


/*------------------------------------------------------------------------------------------------------
����    ����CCD_Delayus
����    �ܡ�CCD����ʱ�����������ʱ��
����    ����us    ��    ��ʱʱ��
���� �� ֵ����
��ʵ    ����CCD_Delayus(1);
��ע�����
--------------------------------------------------------------------------------------------------------*/
void CCD_Delayus(uint8_t us)
{
  while(us--)
  {
    for(uint16_t i=0;i<50;i++)
	{
		 asm("nop");  
	}  
  }
}

/*------------------------------------------------------------------------------------------------------
����    ����CCD_Init
����    �ܡ���ʼ��CCD
����    ������
���� �� ֵ����
��ʵ    ����CCD_Init(); 
��ע�����
--------------------------------------------------------------------------------------------------------*/
void CCD_Init(void) 
{
  //-----�˿ڳ�ʼ��----//
  CLOCK_EnableClock(kCLOCK_Iomuxc);           // IO��ʱ��ʹ��
  
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16,0U); //J11 CLK
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0U); //H12 SI
  
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16,0x10B0u);  //����IO���� ����Ϊ˵��
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0x10B0u);
    //����ڹܽ����ýṹ��
  gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIOΪ�������
  1,                   //�ߵ�ƽ
  kGPIO_NoIntmode      //���ж�ģʽ
  }; 
  
  GPIO_PinInit(GPIO1, 16, &GPIO_Output_Config);      
  GPIO_PinInit(GPIO1, 28, &GPIO_Output_Config);
  
  LQ_ADC_Init(ADC1, ADC1_CH11);
  
  
}

/*------------------------------------------------------------------------------------------------------
����    ����CCD_Collect
����    �ܡ�CCD�ɼ�����
����    ����p  ��  ��Ųɼ����ݵ��׵�ַ
���� �� ֵ����
��ʵ    ����uint16_t ccd_buf[128];
��ʵ    ����CCD_Collect(ccd_buf); ADC_Read_Average(ADC1_CH11,5);
��ע�����
--------------------------------------------------------------------------------------------------------*/
void CCD_Collect(void) 
{
	unsigned char i;

//	CCD_CLK(0);
//        CCD_Delayus(1);
//        CCD_SI(1);
//        CCD_Delayus(1);
//        CCD_CLK(1);
//        CCD_Delayus(1);
//        CCD_SI(0);
         CCD_CLK(1);
         CCD_SI(0);
         
     
         CCD_SI(1);
         CCD_CLK(0);
         
     
         CCD_CLK(1);
         CCD_SI(0);
        //CCD_CLK(1);
        //CCD_SI(0);

	for(i=0; i<128; i++) 
	{
		CCD_CLK(0);       // CLK = 1 
//                CCD_Delayus(5);
		ccd_data[i]= ADC_Read(ADC1, ADC1_CH11);   
//                CCD_Delayus(5);
		CCD_CLK(1);       // CLK = 0 
	}
}



