/*-----------------------------------------------------------------------------------------------------
【平    台】龙邱K60核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0，龙邱开源代码，仅供参考，后果自负
【最后更新】2019年04月02日
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【编译平台】IAR 8.2
【功    能】测试线性CCD
【注意事项】
-------------------------------------------------------------
CCD接口
              
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
【函    数】Test_CCD
【功    能】CCD测试函数
【参    数】无
【返 回 值】无
【实    例】Test_CCD();
【注意事项】
--------------------------------------------------------------------------------------------------------*/

void SendImageData(void) 
{
   uint8_t i = 0;
   UART_PutChar(LPUART1,0x00); 
   UART_PutChar(LPUART1,0xff);
   UART_PutChar(LPUART1,0x01);
   UART_PutChar(LPUART1,0x00);
   
   for(i=0; i<128; i++)                        //数据
   {
      UART_PutChar(LPUART1,ccd_data[i]>>8);   //发送高8位
      UART_PutChar(LPUART1,ccd_data[i]&0XFF); //发送高低8位
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
//		if(systime.get_time_ms() - now_time >= 50)                  //20帧
//		{
//			now_time = systime.get_time_ms();
//			
//			/* 采集ccd图像 */
//			CCD_Collect();
//			
//			/* 发送到上位机 */
//			if(count++%5 == 0)
//			{
//				UART_PutChar(LPUART3,'T');
//				for(uint8_t i=0; i<128; i++) 
//				{
//                                        uint8_t tmp = ccd_data[i];
//					UART_PutChar(LPUART3,(unsigned char)(ccd_data[i]>>8)); //发送高8位
//					UART_PutChar(LPUART3,(unsigned char)ccd_data[i]);      //发送高低8位 
//                                        printf("\r\nLQ CCD Test");
//				}
//			}
//			
//			/* OLED 显示 */
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
【函    数】CCD_Delayus
【功    能】CCD的延时函数，大概延时，
【参    数】us    ：    延时时间
【返 回 值】无
【实    例】CCD_Delayus(1);
【注意事项】
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
【函    数】CCD_Init
【功    能】初始化CCD
【参    数】无
【返 回 值】无
【实    例】CCD_Init(); 
【注意事项】
--------------------------------------------------------------------------------------------------------*/
void CCD_Init(void) 
{
  //-----端口初始化----//
  CLOCK_EnableClock(kCLOCK_Iomuxc);           // IO口时钟使能
  
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16,0U); //J11 CLK
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0U); //H12 SI
  
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16,0x10B0u);  //配置IO属性 以上为说明
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0x10B0u);
    //输出口管脚配置结构体
  gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIO为输出方向
  1,                   //高电平
  kGPIO_NoIntmode      //非中断模式
  }; 
  
  GPIO_PinInit(GPIO1, 16, &GPIO_Output_Config);      
  GPIO_PinInit(GPIO1, 28, &GPIO_Output_Config);
  
  LQ_ADC_Init(ADC1, ADC1_CH11);
  
  
}

/*------------------------------------------------------------------------------------------------------
【函    数】CCD_Collect
【功    能】CCD采集函数
【参    数】p  ：  存放采集数据的首地址
【返 回 值】无
【实    例】uint16_t ccd_buf[128];
【实    例】CCD_Collect(ccd_buf); ADC_Read_Average(ADC1_CH11,5);
【注意事项】
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



