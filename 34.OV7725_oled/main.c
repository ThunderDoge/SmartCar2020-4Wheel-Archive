/*------------------------------------------------------
��ƽ    ̨������i.MX RT1064���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2019��3��12��
��dev.env.��IAR8.30.1�����ϰ汾
��Target �� i.MX RT1064
��Crystal�� 24.000Mhz
��ARM PLL�� 1200MHz
��SYS PLL�� 600MHz
��USB PLL�� 480MHz
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
---------------------------------------------------------
LED��

GPIO2_IO23  -------->  �ܽ�B12   ----- >  ���İ�G��
GPIO3_IO26  -------->  �ܽ�A7    ----- >  ���İ�R��
GPIO3_IO27  -------->  �ܽ�C7    ----- >  ���İ�B��

GPIO2_IO22  -------->  �ܽ�C12   ----- >  ĸ��D1
GPIO2_IO25  -------->  �ܽ�A13   ----- >  ĸ��D0
---------------------------------------------------------
����

GPIO3_IO04 ------ �ܽ�P2   ----- >  ĸ�尴��K2
GPIO2_IO27 ------ �ܽ�C13  ----- >  ĸ�尴��K0
GPIO2_IO30 ------ �ܽ�C14  ----- >  ĸ�尴��k1
GPIO5_I000 ------ �ܽ�L6   ----- >  ���İ尴��WUP
---------------------------------------------------------
����
LPUART1_TX ------ �ܽ�K14
LPUART1_RX ------ �ܽ�L14
---------------------------------------------------------
OLED
LCD_DC	   ------ �ܽ�E11
LCD_RST    ------ �ܽ�E12
LCD_SDA    ------ �ܽ�D12
LCD_SCL    ------ �ܽ�D11
----------------------------------------------------------
TFT1.8����
TFTSPI_CS  ------ �ܽ�D11
TFTSPI_SCK ------ �ܽ�D12
TFTSPI_SDI ------ �ܽ�E12
TFTSPI_DC  ------ �ܽ�E11
TFTSPI_RST ------ �ܽ�B13
---------------------------------------------------------
ADC
ADC1_CH10  ------ �ܽ�K12
ADC1_CH4   ------ �ܽ�L10
ADC1_CH7   ------ �ܽ�L11
ADC1_CH8   ------ �ܽ�M12
ADC1_CH3   ------ �ܽ�H14
ADC1_CH1   ------ �ܽ�K14 
ADC1_CH2   ------ �ܽ�L14 
---------------------------------------------------------
PWM
���
PWM1, kPWM_Module_3, kPWM_PwmA   L5     ���1�ӿ�
PWM1, kPWM_Module_3, kPWM_PwmB   M5     ���1�ӿ�
PWM2, kPWM_Module_0, kPWM_PwmA   A8     ���2�ӿ�
PWM2, kPWM_Module_0, kPWM_PwmB   A9     ���2�ӿ�
PWM2, kPWM_Module_1, kPWM_PwmA   B9     ���3�ӿ�
PWM2, kPWM_Module_1, kPWM_PwmB   C9     ���3�ӿ�
PWM2, kPWM_Module_2, kPWM_PwmB   A10    ���4�ӿ�
PWM1, kPWM_Module_1, kPWM_PwmA   J1     ���4�ӿ�
���
PWM2, kPWM_Module_3, kPWM_PwmA   M3     ����ӿ�
PWM2, kPWM_Module_3, kPWM_PwmB   M4     ����ӿ�
----------------------------------------------------------
��������ENC��
G13--A�����LSB
F14--B�����DIR

H2--A�����LSB
J2--B�����DIR

J3--A�����LSB
K1--B�����DIR

C11--A�����LSB
B11--B�����DIR
----------------------------------------------------------
IIC
SCL  ---  J11
SDA  ---  K11
----------------------------------------------------------
�ⲿ16MB flash
QSPI_DQS --  N3
QSPI_SS0 --  L3
QSPI_CLK --  L4
QSPI_D0  --  P3
QSPI_D1  --  N4
QSPI_D2  --  P4
QSPI_D3  --  P5
----------------------------------------------------------
SD���ӿڣ�enc ��pwm�ܽŸ��ã�
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

/* �ж����ȼ��� */
#define NVIC_Group0   0x07
#define NVIC_Group1   0x06
#define NVIC_Group2   0x05
#define NVIC_Group3   0x04
#define NVIC_Group4   0x03

extern Threshold threshold;
/**
  * @brief    ����ȷ��ʱ
  *
  * @param    ��
  *
  * @return   ��
  *
  * @note     ��
  *
  * @example  delayms(100);
  *
  * @date     2019/4/22 ����һ
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

void delayus(uint32_t us) //��ʱ1us ʵ��Ϊ1.18us   ��ʱ10us ʵ��Ϊ10.02us   ��ʱ100us ʵ��Ϊ98.4us
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
        //Test_OLED_Camera();//��ѧ����
        //Test_W25Q128();
        //Test_CCD();
        //Test_MOTOR();
        //Test_SERVO();//�����汾
        //Test_Servo();//�Զ��汾
        //Test_BUZZER();
        //LQ_SetServoDty(SERVO_MID_PWM_VAL);
    }
}
void InitAll()
{
	BOARD_ConfigMPU();                      //��ʼ��MPU
        BOARD_InitPins();                       //���ڹܽų�ʼ��
        //BOARD_InitSDRAMPins();                //SDRAM��ʼ��
	BOARD_BootClockRUN();	                //����ϵͳʱ��
        BOARD_InitDebugConsole();               //��ʼ������
        NVIC_SetPriorityGrouping(NVIC_Group2);  //�ж����ȼ�����4
	LED_Init();		                //��ʼ��LED  
	KEY_Init();                             //��ʼ������
	MotoInit();				//��ʼ�����
	encoderInit();				//��ʼ��������
	LCD_Init();				//��ʼ��OLED
        
        LQ_UART_Init(LPUART4, 115200);
        LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);
        LPUART_DisableInterrupts(LPUART4, kLPUART_IdleLineInterruptEnable);
        NVIC_SetPriority(LPUART4_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
        EnableIRQ(LPUART4_IRQn);	                            //ʹ��LPUART1�ж�

        
//        //DMA&����
//	#if (ANO_DMA==1)	
//	LQ_UART_DMA_Init(LPUART3, 115200, 1, 2);
//	#endif
//	//ʹ�ܽ����ж�
//	LPUART_EnableInterrupts(LPUART3, kLPUART_RxDataRegFullInterruptEnable);
//	//�رտ����ж�
//	LPUART_DisableInterrupts(LPUART3, kLPUART_IdleLineInterruptEnable);
//	NVIC_SetPriority(LPUART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
//	EnableIRQ(LPUART3_IRQn);//ʹ��LPUART3 �Ŀ����ж�
       
        
        //CCD_Init();                             //��ʼ������CCD
        Init_threshold(&threshold);             //��ʼ����ֵ�ṹ��
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_05_GPIO3_IO17,0U);//J2
        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_GPIO3_IO13,0U);//J3
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_05_GPIO3_IO17,0x10B0u);
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_GPIO3_IO13,0x10B0u);
        gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIOΪ�������
                                                1,                   //�ߵ�ƽ
                                                kGPIO_NoIntmode      //���ж�ģʽ
                                                };
        GPIO_PinInit(GPIO3,17, &GPIO_Output_Config);
        GPIO_PinInit(GPIO3,13, &GPIO_Output_Config);
	W25QXX_Init();      //��ʼ��W25QXX
//	while (W25QXX_ReadID() != W25Q256) {
//		LCD_CLS();
//		sprintf(getOledBuf(4), "W25Q256 Check Failed!"); LCD_refreshOneLine(4, false);
//		delayms(500);
//        }
        
        //oledѯ���Ƿ�д��,���³����һ������ʱд��
	tryWirteUiVarsToFlash();
        //Ĭ��ֱ�Ӵ�Flash�����������Ҫ���ֳ����ڲ�������ǰ��ѡ��д�룬�����Flash��Ĳ���
	tryReadUiVarsFromFlash();
        
        ServoInit();				//��ʼ�����
	//UI��ʼ��
	UI_initialize();
	
	//����ͷ��ʼ�����ڶ�ȡUI�����������ð�ƽ��
	//LQ_Camera_Init();
	LQ_ADC_Init(ADC1, ADC1_CH13);  //��г�ʼ��
        LQ_ADC_Init(ADC1, ADC1_CH6);
        LQ_ADC_Init(ADC1, ADC1_CH9);
        LQ_ADC_Init(ADC1, ADC1_CH15);
        LQ_ADC_Init(ADC1, ADC1_CH8);
        LQ_ADC_Init(ADC1, ADC1_CH4);
        LQ_ADC_Init(ADC1, ADC1_CH14);
	LQ_PIT_Init(kPIT_Chnl_1, 3000);//��ʱ��5ms�ж�
        //LQ_PIT_Init(kPIT_Chnl_2, 600);//ȫ������ʱ��ʹ��    375000
	EncoderWork();			//�������������õ��ٶ�
	//MPU6050_Init(); //�����ǳ�ʼ��
}

void Init_threshold(Threshold * threshold)
{
  threshold->CROSS_VALUE = 0.40;
  threshold->LOSE_LINE_SUM = 1.15;
  threshold->OUT_OF_WAY_SPEED = 30;
  threshold->OUT_OF_WAY_VALUE = 0.8;
}