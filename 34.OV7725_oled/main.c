/*------------------------------------------------------
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月12日
【dev.env.】IAR8.30.1及以上版本
【Target 】 i.MX RT1064
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 600MHz
【USB PLL】 480MHz
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
---------------------------------------------------------
LED灯

GPIO2_IO23  -------->  管脚B12   ----- >  核心板G灯
GPIO3_IO26  -------->  管脚A7    ----- >  核心板R灯
GPIO3_IO27  -------->  管脚C7    ----- >  核心板B灯

GPIO2_IO22  -------->  管脚C12   ----- >  母板D1
GPIO2_IO25  -------->  管脚A13   ----- >  母板D0
---------------------------------------------------------
按键

GPIO3_IO04 ------ 管脚P2   ----- >  母板按键K2
GPIO2_IO27 ------ 管脚C13  ----- >  母板按键K0
GPIO2_IO30 ------ 管脚C14  ----- >  母板按键k1
GPIO5_I000 ------ 管脚L6   ----- >  核心板按键WUP
---------------------------------------------------------
串口
LPUART1_TX ------ 管脚K14
LPUART1_RX ------ 管脚L14
---------------------------------------------------------
OLED
LCD_DC	   ------ 管脚E11
LCD_RST    ------ 管脚E12
LCD_SDA    ------ 管脚D12
LCD_SCL    ------ 管脚D11
----------------------------------------------------------
TFT1.8寸屏
TFTSPI_CS  ------ 管脚D11
TFTSPI_SCK ------ 管脚D12
TFTSPI_SDI ------ 管脚E12
TFTSPI_DC  ------ 管脚E11
TFTSPI_RST ------ 管脚B13
---------------------------------------------------------
ADC
ADC1_CH10  ------ 管脚K12
ADC1_CH4   ------ 管脚L10
ADC1_CH7   ------ 管脚L11
ADC1_CH8   ------ 管脚M12
ADC1_CH3   ------ 管脚H14
ADC1_CH1   ------ 管脚K14 
ADC1_CH2   ------ 管脚L14 
---------------------------------------------------------
PWM
电机
PWM1, kPWM_Module_3, kPWM_PwmA   L5     电机1接口
PWM1, kPWM_Module_3, kPWM_PwmB   M5     电机1接口
PWM2, kPWM_Module_0, kPWM_PwmA   A8     电机2接口
PWM2, kPWM_Module_0, kPWM_PwmB   A9     电机2接口
PWM2, kPWM_Module_1, kPWM_PwmA   B9     电机3接口
PWM2, kPWM_Module_1, kPWM_PwmB   C9     电机3接口
PWM2, kPWM_Module_2, kPWM_PwmB   A10    电机4接口
PWM1, kPWM_Module_1, kPWM_PwmA   J1     电机4接口
舵机
PWM2, kPWM_Module_3, kPWM_PwmA   M3     舵机接口
PWM2, kPWM_Module_3, kPWM_PwmB   M4     舵机接口
----------------------------------------------------------
编码器（ENC）
G13--A相或者LSB
F14--B相或者DIR

H2--A相或者LSB
J2--B相或者DIR

J3--A相或者LSB
K1--B相或者DIR

C11--A相或者LSB
B11--B相或者DIR
----------------------------------------------------------
IIC
SCL  ---  J11
SDA  ---  K11
----------------------------------------------------------
外部16MB flash
QSPI_DQS --  N3
QSPI_SS0 --  L3
QSPI_CLK --  L4
QSPI_D0  --  P3
QSPI_D1  --  N4
QSPI_D2  --  P4
QSPI_D3  --  P5
----------------------------------------------------------
SD卡接口（enc 、pwm管脚复用）
SD_CMD   --  J4
SD_CLK   --  J3
SD_D0    --  J1
SD_D1    --  K1
SD_D2    --  H2
SD_D3    --  J2
SD_Detect--  D13
--------------------------------------------------------*/

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "clock_config.h"

#include "LQ_Camera.h"
#include "LQ_PWM.h"
#include "pitimer.h"
#include "LQ_Encoder.h"
#include "LQ_KEY.h"
#include "LQ_ADC.h"
#include "LQ_PIT.h"
#include "LQ_12864.h"
#include "LQ_FLASH.h"
#include "UIControl.h"
#include "MainControl.h"
#include "LQ_W25Q128.h"
#include "LQ_CCD.h"

/* 中断优先级组 */
#define NVIC_Group0   0x07
#define NVIC_Group1   0x06
#define NVIC_Group2   0x05
#define NVIC_Group3   0x04
#define NVIC_Group4   0x03

extern Threshold threshold;
/**
  * @brief    不精确延时
  *
  * @param    无
  *
  * @return   无
  *
  * @note     无
  *
  * @example  delayms(100);
  *
  * @date     2019/4/22 星期一
*/
void delayms(uint16_t ms)
{
	volatile uint32_t i = 0;
	while(ms--)
	{
		for (i = 0; i < 30000; ++i)
		{
			__asm("NOP"); /* delay */
		}
	}	
}

void delayus(uint32_t us) //延时1us 实测为1.18us   延时10us 实测为10.02us   延时100us 实测为98.4us
{
    uint32_t i,j;
    
    for (i = 0; i < us; i++);
    {
        for (j = 0; j < 290; j++)//528M--1us
        __NOP();//asm("NOP");   
    }
    return ;
} 

/*!
 * @brief Main function
 */


void Init_threshold(Threshold * threshold);

int main(void) 
{
    InitAll();
    while (1) {
	mainControlProcess();
        //Test_ENCoder();
        //Test_ADC();
        //Test_PIT();
        //Test_OLED_Camera();//玄学问题
        //Test_W25Q128();
        //Test_CCD();
        //Test_MOTOR();
        //Test_SERVO();//按键版本
        //Test_Servo();//自动版本
        //Test_BUZZER();
        //LQ_SetServoDty(SERVO_MID_PWM_VAL);
    }
}
void InitAll()
{
	BOARD_ConfigMPU();                      //初始化MPU
        BOARD_InitPins();                       //串口管脚初始化
        //BOARD_InitSDRAMPins();                //SDRAM初始化
	BOARD_BootClockRUN();	                //配置系统时钟
        BOARD_InitDebugConsole();               //初始化串口
        NVIC_SetPriorityGrouping(NVIC_Group2);  //中断优先级分组4
	LED_Init();		                //初始化LED  
	KEY_Init();                             //初始化按键
	MotoInit();				//初始化电机
	encoderInit();				//初始化编码器
	LCD_Init();				//初始化OLED
        
        LQ_UART_Init(LPUART4, 115200);
        LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);
        LPUART_DisableInterrupts(LPUART4, kLPUART_IdleLineInterruptEnable);
        NVIC_SetPriority(LPUART4_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
        EnableIRQ(LPUART4_IRQn);	                            //使能LPUART1中断

        
//        //DMA&串口
//	#if (ANO_DMA==1)	
//	LQ_UART_DMA_Init(LPUART3, 115200, 1, 2);
//	#endif
//	//使能接收中断
//	LPUART_EnableInterrupts(LPUART3, kLPUART_RxDataRegFullInterruptEnable);
//	//关闭空闲中断
//	LPUART_DisableInterrupts(LPUART3, kLPUART_IdleLineInterruptEnable);
//	NVIC_SetPriority(LPUART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
//	EnableIRQ(LPUART3_IRQn);//使能LPUART3 的空闲中断
       
        
        //CCD_Init();                             //初始化线性CCD
        Init_threshold(&threshold);             //初始化阈值结构体
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_05_GPIO3_IO17,0U);//J2
        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_GPIO3_IO13,0U);//J3
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_05_GPIO3_IO17,0x10B0u);
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_GPIO3_IO13,0x10B0u);
        gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIO为输出方向
                                                1,                   //高电平
                                                kGPIO_NoIntmode      //非中断模式
                                                };
        GPIO_PinInit(GPIO3,17, &GPIO_Output_Config);
        GPIO_PinInit(GPIO3,13, &GPIO_Output_Config);
	W25QXX_Init();      //初始化W25QXX
//	while (W25QXX_ReadID() != W25Q256) {
//		LCD_CLS();
//		sprintf(getOledBuf(4), "W25Q256 Check Failed!"); LCD_refreshOneLine(4, false);
//		delayms(500);
//        }
        
        //oled询问是否写入,当新程序第一次运行时写入
	tryWirteUiVarsToFlash();
        //默认直接从Flash里读参数，需要保持程序内参数则在前面选择写入，否则读Flash里的参数
	tryReadUiVarsFromFlash();
        
        ServoInit();				//初始化舵机
	//UI初始化
	UI_initialize();
	
	//摄像头初始化放在读取UI参数后，以设置白平衡
	//LQ_Camera_Init();
	LQ_ADC_Init(ADC1, ADC1_CH13);  //电感初始化
        LQ_ADC_Init(ADC1, ADC1_CH6);
        LQ_ADC_Init(ADC1, ADC1_CH9);
        LQ_ADC_Init(ADC1, ADC1_CH15);
        LQ_ADC_Init(ADC1, ADC1_CH8);
        LQ_ADC_Init(ADC1, ADC1_CH4);
        LQ_ADC_Init(ADC1, ADC1_CH14);
	LQ_PIT_Init(kPIT_Chnl_1, 3000);//定时器5ms中断
        //LQ_PIT_Init(kPIT_Chnl_2, 600);//全局运行时间使用    375000
	EncoderWork();			//编码器工作，得到速度
	//MPU6050_Init(); //陀螺仪初始化
}

void Init_threshold(Threshold * threshold)
{
  threshold->CROSS_VALUE = 0.40;
  threshold->LOSE_LINE_SUM = 1.15;
  threshold->OUT_OF_WAY_SPEED = 30;
  threshold->OUT_OF_WAY_VALUE = 0.8;
}