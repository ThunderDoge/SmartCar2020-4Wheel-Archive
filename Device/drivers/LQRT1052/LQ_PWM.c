/*-----------------------------------------------------------------
��ƽ    ̨������i.MX RT1064���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
�������汾��V1.0
�������¡�2019��3��12��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
��dev.env.��IAR8.30.1�����ϰ汾
PWM1, kPWM_Module_3, kPWM_PwmA   L5     ���1�ӿ�
PWM1, kPWM_Module_3, kPWM_PwmB   M5     ���1�ӿ�
PWM2, kPWM_Module_0, kPWM_PwmA   A8     ���2�ӿ�
PWM2, kPWM_Module_0, kPWM_PwmB   A9     ���2�ӿ�
PWM2, kPWM_Module_1, kPWM_PwmA   B9     ���3�ӿ�
PWM2, kPWM_Module_1, kPWM_PwmB   C9     ���3�ӿ�
PWM2, kPWM_Module_2, kPWM_PwmB   A10    ���4�ӿ�
PWM1, kPWM_Module_1, kPWM_PwmA   J1     ���4�ӿ�
PWM2, kPWM_Module_3, kPWM_PwmA   M3     ����ӿ�
PWM2, kPWM_Module_3, kPWM_PwmB   M4     ����ӿ�
--------------------------------------------------------------------*/
#include "include.h"
#include "LQ_PWM.h" 
#include "fsl_pwm.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "LQ_LED.h"
#include "LQ_12864.h"
#include "LQ_KEY.h"
#include "LQ_SGP18T.h"
#include "stdio.h"
#include "fsl_gpio.h"
#include "szx_image.h"
#include "adcprocess.h"
   
/* �����ⲿ��ʱ���� */
//extern void delayms(uint16_t ms);

void 
Test_Servo(void){
while(1) {
//          unsigned char keyRes = KEY_Read(0);
//          u8 i = SERVO_MID_PWM_VAL;
//          if(keyRes == ADD && i <= SERVO_RIGHT_PWM_VAL){
//            i = i+10;
//            LQ_SetServoDty(i);
//          }
//          else if(keyRes == SUB && i >= SERVO_LEFT_PWM_VAL){
//            i = i-10;
//            LQ_SetServoDty(i);
//          }
		for (int i = SERVO_LEFT_PWM_VAL; i < SERVO_RIGHT_PWM_VAL; i+=10) {
			LQ_SetServoDty(i);
			delayms(500);
		}
		for (int i = SERVO_RIGHT_PWM_VAL; i > SERVO_LEFT_PWM_VAL; i-=10) {
			LQ_SetServoDty(i);
			delayms(500);
		}
	}
}
/**
  * @brief    PWM���ܳ�ʼ��
  *
  * @param    base          ��PWMģ�� PWM1~PWM4
  * @param    subModule     ��PWMģ���µ���ģ�� kPWM_Module_0 ~ kPWM_Module_3 
  * @param    pwm_channels  ��PWMģ���µ���ģ�� ��ͨ�� �� kPWM_PwmA �� kPWM_PwmB  �� kPWM_PwmA_B 
  * @param    Frequency     ��PWMģ���µ���ģ���Ƶ��
  *
  * @return   
  *
  * @note     PWM ʹ�� IPGʱ��  150M
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void LQ_PWM_Init(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t Frequency)
{
  
       LQ_PWM_InitPins(base, subModule, pwm_channels); 
      //�ܽŸ���ѡ��
      LQ_PWM_InitPins(base, subModule, pwm_channels); 
      
      uint32_t pwmSourceClockInHz;        //PWMʱ��Դ
      pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);
      
      pwm_config_t pwmConfig;// PWM��ʼ���ṹ��
      /*pwmConfigĬ�ϲ���
      * pwmConfig.enableDebugMode = false;
      * pwmConfig.enableWait = false;
      * pwmConfig.reloadSelect = kPWM_LocalReload;
      * pwmConfig.faultFilterCount = 0;
      * pwmConfig.faultFilterPeriod = 0;
      * pwmConfig.clockSource = kPWM_BusClock;
      * pwmConfig.prescale = kPWM_Prescale_Divide_1;
      * pwmConfig.initializationControl = kPWM_Initialize_LocalSync;
      * pwmConfig.forceTrigger = kPWM_Force_Local;
      * pwmConfig.reloadFrequency = kPWM_LoadEveryOportunity;
      * pwmConfig.reloadLogic = kPWM_ReloadImmediate;
      * pwmConfig.pairOperation = kPWM_Independent;        //PWMA��PWMB�������
      */
      PWM_GetDefaultConfig(&pwmConfig);                    //�õ�Ĭ�ϵ�PWM��ʼ���ṹ��
      
      pwmConfig.reloadLogic   = kPWM_ReloadPwmFullCycle;   //ѭ�����
      
      pwmConfig.enableDebugMode = true;    
      
      /* ������ѷ�Ƶϵ�� */
      uint32_t temp = pwmSourceClockInHz / Frequency;
      uint8_t div = 0;
      while(temp > 0xFFFF)
      {
          div++;
          temp = temp >> 1;
      }
      assert(div < 8);
      
      pwmConfig.prescale = (pwm_clock_prescale_t)div;   
  
      if (PWM_Init(base, subModule, &pwmConfig) == kStatus_Fail)
      {
          PRINTF("PWM subModule %d initialization failed\n",subModule);
          return ;
      }  
      base->SM[subModule].DISMAP[0]=0;      //���ι��ϼ�⹦��
      
      pwm_signal_param_t pwmSignal[2];      //PWM��ģ���ʼ���ṹ��
      uint16_t deadTimeVal = 0;
      /* Set deadtime count, we set this to about 650ns */
      //  deadTimeVal = ((uint64_t)pwmSourceClockInHz * 650) / 1000000000;//�����趨 ��������ˢ���PWMA��PWMB�������ʱʹ�� 
      
      pwmSignal[0].pwmChannel = kPWM_PwmA; //Ĭ��ʹ��ͨ��A
      if(pwm_channels == kPWM_PwmB)        //���ʹ��ͨ��B
      {
          pwmSignal[0].pwmChannel = kPWM_PwmB;  // ʹ��PWMB
      }
      pwmSignal[0].level = kPWM_HighTrue;   //�����ƽΪ�ߵ�ƽ
      pwmSignal[0].dutyCyclePercent = 0;    //��ʼռ�ձ� 0%
      pwmSignal[0].deadtimeValue = deadTimeVal;//����ʱ��
      
      /*��AB��ͨ��ͬʱʹ�� ��������*/
      pwmSignal[1].pwmChannel = kPWM_PwmB;  // ʹ��PWMB
      pwmSignal[1].level = kPWM_HighTrue;   //�����ƽΪ�ߵ�ƽ
      pwmSignal[1].dutyCyclePercent = 0;    //��ʼռ�ձ� 0%
      pwmSignal[1].deadtimeValue = deadTimeVal;//����ʱ��
      
      if(pwm_channels == kPWM_PwmA_B)  //A B��ͨ��ͬʱʹ��
      {
          /*����pwm��ʱ�� = pwmSourceClockInHz��Ƶ�� = Frequency ���뷽ʽ = kPWM_SignedCenterAligned��*/
          PWM_SetupPwm(base, subModule, pwmSignal, 2, kPWM_SignedCenterAligned, Frequency,pwmSourceClockInHz); 
      }
      else  //��ʹ��A B�е�һ��ʱ
      {
          /*����pwm��ʱ�� = pwmSourceClockInHz��Ƶ�� = Frequency ���뷽ʽ = kPWM_SignedCenterAligned��*/
          PWM_SetupPwm(base, subModule, pwmSignal, 1, kPWM_SignedCenterAligned, Frequency,pwmSourceClockInHz); 
      }   
      
      PWM_SetPwmLdok(base, 1u<<subModule, true);//����pwm�� load okλ
      
      PWM_StartTimer(base, 1u<<subModule); //������ʱ��
      
}


/**
  * @brief    ����ռ�ձ�
  *
  * @param    base          ��PWMģ�� PWM1~PWM4
  * @param    subModule     ��PWMģ���µ���ģ�� kPWM_Module_0 ~ kPWM_Module_3 
  * @param    pwm_channels  ��PWMģ���µ���ģ�� ��ͨ�� �� kPWM_PwmA �� kPWM_PwmB  �� kPWM_PwmA_B 
  * @param    duty          ��ռ�ձ�*DUTY_MAX  
  *
  * @return   
  *
  * @note     Ĭ�϶��뷽ʽkPWM_SignedCenterAligned�� ���������޸�  
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */



inline void LQ_PWM_SetDuty(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t duty)
{
    assert(duty <= DUTY_MAX);
    base->MCTRL |= PWM_MCTRL_CLDOK(1u<<subModule);
    PWM_UpdateDuty(base, subModule, pwm_channels, kPWM_SignedCenterAligned, duty); 
    base->MCTRL |= PWM_MCTRL_LDOK(1u<<subModule);
}

/**
  * @brief    ͬʱ���� һ����ģ���A �� B��  ���������
  *
  * @param    base          ��PWMģ�� PWM1~PWM4
  * @param    subModule     ��PWMģ���µ���ģ�� kPWM_Module_0 ~ kPWM_Module_3 
  * @param    pwm_channels  ��PWMģ���µ���ģ�� ��ͨ�� �� kPWM_PwmA �� kPWM_PwmB  �� kPWM_PwmA_B 
  * @param    duty          ��ռ�ձ�*DUTY_MAX  
  *
  * @return   
  *
  * @note     ͬʱ���� һ����ģ���A �� B�� ���� ���������
  * @note     Ĭ�϶��뷽ʽkPWM_SignedCenterAligned�� ���������޸�  
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
inline void LQ_PWMA_B_SetDuty(PWM_Type *base, pwm_submodule_t subModule, uint16_t dutyA, uint16_t dutyB)
{
    assert( dutyA <= DUTY_MAX);
    assert( dutyB <= DUTY_MAX);
    base->MCTRL |= PWM_MCTRL_CLDOK(1u<<subModule);
    PWM_UpdateDuty(base, subModule, kPWM_PwmA, kPWM_SignedCenterAligned, dutyA); 
    PWM_UpdateDuty(base, subModule, kPWM_PwmB, kPWM_SignedCenterAligned, dutyB); 
     base->MCTRL |= PWM_MCTRL_LDOK(1u<<subModule);
    //PWM_SetPwmLdok(base, 1u<<subModule, true);
}


/**
  * @brief    ���ö��ռ�ձ�
  *
  * @param    duty          ��ռ�ձ�*DUTY_MAX   
  *
  * @return   
  *
  * @note      �����ֵ 1.5ms�ߵ�ƽ��PWM��    �޷��� 1.3 - 1.7ms֮��
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
volatile uint16_t currentServoDty = 0;
//volatile uint8_t currentTurnMode = 0;
inline void LQ_SetServoDty(uint16_t duty)    
{
	if(duty > SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL;
	if(duty < SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL;  
        
	currentServoDty = duty;
        
        LQ_PWM_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmB, duty); //A9   
}


/**
  * @brief    ���õ��ռ�ձ�
  *
  * @param    num�� 1�����1�� ʹ�� L5  M5  
                    2�����2�� ʹ�� A8  A9  
                    3�����3�� ʹ�� B9  C9  
                    4�����4�� ʹ�� A10 J1
  *
  * @return   
  *
  * @note     �������ת���Ե��ڵ�����ߵ�����Ҳ���Կ���PWM����  ����������ת�ͳ����ж���Ĳ�һ�������������޸�
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
//inline void LQ_SetMotorDty(uint8_t num, short duty)  
//{
//    assert(num < 5);
//    
//    switch(num)
//    {
//        case 1:
//            if(duty >= 0)  
//            {
//                LQ_PWMA_B_SetDuty(PWM1, kPWM_Module_3, 0, duty);
//            }
//            else
//            {
//                LQ_PWMA_B_SetDuty(PWM1, kPWM_Module_3, -duty, 0);
//            }
//            break;
//            
//        case 2:
//            if(duty >= 0)  
//            {
//                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_0, 0, duty);
//            }
//            else
//            {
//                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_0, -duty, 0);
//            }
//            break;
//            
//        case 3:
//            if(duty >= 0)  
//            {
//                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_1, 0, duty);
//            }
//            else
//            {
//                LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_1, -duty, 0);
//            }
//            break;
//            
//        case 4:
//            if(duty >= 0)  
//            {
//                LQ_PWM_SetDuty(PWM2,    kPWM_Module_2, kPWM_PwmB , 0);
//                LQ_PWM_SetDuty(PWM1,    kPWM_Module_1, kPWM_PwmA , duty);
//            }
//            else
//            {
//                LQ_PWM_SetDuty(PWM2,    kPWM_Module_2, kPWM_PwmB , -duty);
//                LQ_PWM_SetDuty(PWM1,    kPWM_Module_1, kPWM_PwmA , 0);
//            }
//            break;
//    }
//  
//}

/**
  * @brief    �������
  *
  * @param    
  *
  * @return   
  *
  * @note     ����ʹ�ô˺���ʱ�������Ҫװ���̣���ֹ���������������ֵ����װ������������������ֵ
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */

/**
  * @brief    �������
  *
  * @param    
  *
  * @return   
  *
  * @note     ����ʹ�ô˺���ʱ�������Ҫװ���̣���ֹ���������������ֵ����װ������������������ֵ
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void ServoInit(void)
{
    LQ_PWM_Init(PWM2, kPWM_Module_0, kPWM_PwmB,  75);//PWM�����Ƶ�� = 6250 000 / VAL1  = 96Hz  A9
    LQ_SetServoDty(SERVO_MID_PWM_VAL);//LPF��Ī������Ҫ��Ȼ����ɾ��
}



void Test_SERVO(void){
  char txt [16];
  int temp = SERVO_MID_PWM_VAL;
//  LCD_Init();
//  KEY_Init();
//  LED_Init();
  LCD_CLS();              //LCD����  
  //ServoInit();
  LQ_SetServoDty(temp);//��ֵ
  while(1){
    unsigned char keyRes = KEY_Read(0);
    if(keyRes == ADD){
      temp = temp + 10;
      LQ_SetServoDty(temp);
    }
    else if(keyRes == OK){
      LQ_SetServoDty(SERVO_MID_PWM_VAL);
      temp = SERVO_MID_PWM_VAL;
    }
    else if(keyRes == SUB){
      temp = temp - 10;
      LQ_SetServoDty(temp);
    }
    
    sprintf(txt,"servopwm:%d",temp);
    LCD_P6x8Str(2,3,txt);
    
    LED_Color(violet); 
    delayms(50);
}
}

/**
  * @brief    �������
  *
  * @param    
  *
  * @return   
  *
  * @note     ���1�� ʹ�� L5  M5   ���2��ʹ��A8  A9  ���3��ʹ�� B9 C9  ���4��A10 J1
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void MotoInit(void)
{    
    LQ_PWM_Init(PWM1, kPWM_Module_3, kPWM_PwmA_B, 12000);//PWM�����Ƶ�� = 6250 000 / VAL1  = 96Hz     L5 M5 ���1��ʼ��
    LQ_PWM_Init(PWM2, kPWM_Module_3, kPWM_PwmA_B, 12000);//PWM�����Ƶ�� = 6250 000 / VAL1  = 96Hz     M3 M4 ���2��ʼ��
//    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_GPIO2_IO10, 0U);//D9
//    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_GPIO2_IO10,0x10B0u);
//    gpio_pin_config_t GPIO_Output_Config = {kGPIO_DigitalOutput, //GPIOΪ�������
//                                            1,                   //�ߵ�ƽ
//                                            kGPIO_NoIntmode      //���ж�ģʽ
//                                            };
//    GPIO_PinInit(GPIO2,10, &GPIO_Output_Config);
    //LQ_SetMotorDty(1,0);
    //LQ_SetMotorDty(2,0);
    //LQ_PWM_Init(PWM2,kPWM_Module_0,kPWM_PwmA,12000);//A8
    //LQ_PWM_Init(PWM2,kPWM_Module_1,kPWM_PwmA,12000);//B9
}

/**
  * @brief    ����PWM
  *
  * @param    
  *
  * @return   
  *
  * @note     ��������ĸ���ϵ�PWM�ӿ��Ƿ���PWM���������ʾ��������
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void Test_PWM_nCH(void)
{    
    uint32_t pwmVal = 4;  
    LED_Init();
     /*��ʼ�����PWM�ӿ�  */
    LQ_PWM_Init(PWM2, kPWM_Module_0, kPWM_PwmA_B, 100);  // A8 A9
    LQ_PWM_Init(PWM2, kPWM_Module_1, kPWM_PwmA_B, 1000); // B9 C9
    LQ_PWM_Init(PWM2, kPWM_Module_2, kPWM_PwmB, 10000);  // A10
    LQ_PWM_Init(PWM1, kPWM_Module_3, kPWM_PwmA_B, 20000);// L5 M5
    LQ_PWM_Init(PWM1, kPWM_Module_1, kPWM_PwmA,   12000);// J1
    LQ_PWM_Init(PWM2, kPWM_Module_3, kPWM_PwmA_B,  50);  // M3 M4

    while (1U)
    {
        delayms(100);        //��ʱ100����
        pwmVal = pwmVal + 4;
        
        /* Reset the duty cycle percentage */
        if (pwmVal > 100)
        {
            pwmVal =2;
        }
        LQ_PWM_SetDuty(PWM1,    kPWM_Module_1, kPWM_PwmA, pwmVal*50);
        LQ_PWM_SetDuty(PWM1,    kPWM_Module_3, kPWM_PwmA, pwmVal*50);
        LQ_PWM_SetDuty(PWM1,    kPWM_Module_3, kPWM_PwmB, pwmVal*100); 
        LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_0, pwmVal*100, pwmVal*50);
        LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_1, pwmVal*100, pwmVal*50);
        LQ_PWM_SetDuty(PWM2,    kPWM_Module_2, kPWM_PwmB , pwmVal*100);
        LQ_PWMA_B_SetDuty(PWM2, kPWM_Module_3, pwmVal*100, pwmVal*50);  
        
        LED_ColorReverse(red);     //���   
        delayms(500);
    }
}


void Test_MOTOR(void){
  LCD_CLS();
  while(1){
    unsigned char txt[16];
    unsigned char keyRes = KEY_Read(KeyMode);
    static short LMotorDuty = 0;
    static short RMotorDuty = 0;
    if(keyRes == ADD)
    {
      LMotorDuty = LMotorDuty + 500;
      LQ_SetMotorDty(1, LMotorDuty);
    }
    else if(keyRes == SUB)
    {
      LMotorDuty = LMotorDuty - 500;
      LQ_SetMotorDty(1, LMotorDuty);
    }
    else if(keyRes == UP)
    {
      RMotorDuty = RMotorDuty - 500;
      LQ_SetMotorDty(2, RMotorDuty);
    }
    else if(keyRes == DOWN)
    {
      RMotorDuty = RMotorDuty - 500;
      LQ_SetMotorDty(2, RMotorDuty);
    }
    sprintf(txt,"LMotorDuty:%d ",LMotorDuty);
    LCD_P6x8Str(4,0,txt);
    sprintf(txt,"RMotorDuty:%d ",RMotorDuty);
    LCD_P6x8Str(4,1,txt);
//    LQ_SetMotorDty(1, 2000);
//    LQ_SetMotorDty(2, 2000);
  }
}




/**
  * @brief    �ı�ռ�ձ�
  *
  * @param    base       ��PWMģ�� PWM1~PWM4
  * @param    subModule  ��PWMģ���µ���ģ�� kPWM_Module_0 ~ kPWM_Module_3 
  * @param    pwmSignal  ��kPWM_PwmA/kPWM_PwmB
  * @param    currPwmMode�����뷽ʽ
  * @param    duty       ��ռ�ձ�*DUTY_MAX
  *
  * @return   ��
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void PWM_UpdateDuty(PWM_Type *base,
                            pwm_submodule_t subModule,
                            pwm_channels_t pwmSignal,
                            pwm_mode_t currPwmMode,
                            uint16_t duty)
{    
    assert(pwmSignal < 2);
    uint16_t pulseCnt = 0, pwmHighPulse = 0;
    int16_t modulo = 0;

    switch (currPwmMode)
    {
        case kPWM_SignedCenterAligned:
            modulo = base->SM[subModule].VAL1;
            pulseCnt = modulo * 2;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * duty) / DUTY_MAX;   //ռ�ձ� =  duty / DUTY_MAX ���޸�

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
                base->SM[subModule].VAL3 = (pwmHighPulse / 2);
            }
            else
            {
                base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
                base->SM[subModule].VAL5 = (pwmHighPulse / 2);
            }
            break;
        case kPWM_CenterAligned:
            pulseCnt = base->SM[subModule].VAL1;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * duty) / DUTY_MAX;   //ռ�ձ� =  duty / DUTY_MAX ���޸�

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
                base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
            }
            else
            {
                base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
                base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
            }
            break;
        case kPWM_SignedEdgeAligned:
            modulo = base->SM[subModule].VAL1;
            pulseCnt = modulo * 2;
            /* Calculate pulse width */
            pwmHighPulse =  (pulseCnt * duty) / DUTY_MAX;   //ռ�ձ� =  duty / DUTY_MAX ���޸�

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = (-modulo);
                base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
            }
            else
            {
                base->SM[subModule].VAL4 = (-modulo);
                base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
            }
            break;
        case kPWM_EdgeAligned:
            pulseCnt = base->SM[subModule].VAL1;
            /* Calculate pulse width */
            pwmHighPulse =  (pulseCnt * duty) / DUTY_MAX;   //ռ�ձ� =  duty / DUTY_MAX ���޸�

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = 0;
                base->SM[subModule].VAL3 = pwmHighPulse;
            }
            else
            {
                base->SM[subModule].VAL4 = 0;
                base->SM[subModule].VAL5 = pwmHighPulse;
            }
            break;
        default:
            break;
    }
}

/**
  * @brief    PWM���ų�ʼ��
  *
  * @param    base          ��PWMģ�� PWM1~PWM4
  * @param    subModule     ��PWMģ���µ���ģ�� kPWM_Module_0 ~ kPWM_Module_3 
  * @param    pwm_channels  ��PWMģ���µ���ģ�� ��ͨ�� �� kPWM_PwmA �� kPWM_PwmB  �� kPWM_PwmA_B 
  *
  * @return   
  *
  * @note     �ڲ����õģ����������ô��ڹܽŸ��õ�  
  *
  * @example  
  *
  * @date     2019/5/23 ������
  */
void LQ_PWM_InitPins(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels)
{
    CLOCK_EnableClock(kCLOCK_Iomuxc);          /* ��ioʱ�� */
    
    if(base == PWM1)
    {
        switch(subModule)      //ѡ��PWM��ģ��
        {
          case 0:        //ѡ��PWM1ģ���subModule0��ģ��
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B) 
                {
                    if(PWM1_MODULE0_PWMA == J4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_00_FLEXPWM1_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_00_FLEXPWM1_PWMA00, 0x10B0u);
                    }
                    else if(PWM1_MODULE0_PWMA == G2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_23_FLEXPWM1_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_23_FLEXPWM1_PWMA00, 0x10B0u);
                    }
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)  
                {
                    if(PWM1_MODULE0_PWMB == D3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_24_FLEXPWM1_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_24_FLEXPWM1_PWMB00, 0x10B0u);
                    }
                    else if(PWM1_MODULE0_PWMB == J3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_FLEXPWM1_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_FLEXPWM1_PWMB00, 0x10B0u);
                    } 
                }
                break;
            }
          case 1:       //ѡ��PWM1ģ���subModule1��ģ��
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE1_PWMA == D2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_25_FLEXPWM1_PWMA01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_25_FLEXPWM1_PWMA01, 0x10B0u);
                    }
                    else if(PWM1_MODULE1_PWMA == J1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_02_FLEXPWM1_PWMA01, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_02_FLEXPWM1_PWMA01, 0x10B0u);
                    }
                    
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE1_PWMB == B3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_26_FLEXPWM1_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_26_FLEXPWM1_PWMB01, 0x10B0u);
                    }
                    else if(PWM1_MODULE1_PWMB == K1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_03_FLEXPWM1_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_03_FLEXPWM1_PWMB01, 0x10B0u);  
                    }  
                }                                                         
                break;
            }
          case 2:        //ѡ��PWM1ģ���subModule2��ģ��
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE2_PWMA == A2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_27_FLEXPWM1_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_27_FLEXPWM1_PWMA02, 0x10B0u);
                    }
                    else if(PWM1_MODULE2_PWMA == H2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_04_FLEXPWM1_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_04_FLEXPWM1_PWMA02, 0x10B0u); 
                    }
                     
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE2_PWMB == D1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_28_FLEXPWM1_PWMB02, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_28_FLEXPWM1_PWMB02, 0x10B0u);
                    }
                    else if(PWM1_MODULE2_PWMB == J2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_05_FLEXPWM1_PWMB02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_05_FLEXPWM1_PWMB02, 0x10B0u);
                    }
                    
                }
                break;
            }
          case 3:        //ѡ��PWM1ģ���subModule3��ģ��
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE3_PWMA == A11)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_00_FLEXPWM1_PWMA03, 0U);   
                    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_00_FLEXPWM1_PWMA03, 0x10B0u); 
                    }
                    else if(PWM1_MODULE3_PWMA == D6)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_38_FLEXPWM1_PWMA03, 0U);   
                    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_38_FLEXPWM1_PWMA03, 0x10B0u); 
                    }
                    else if(PWM1_MODULE3_PWMA == G13)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_10_FLEXPWM1_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_10_FLEXPWM1_PWMA03, 0x10B0u); 
                    }
                    else if(PWM1_MODULE3_PWMA == H1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_12_FLEXPWM1_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_12_FLEXPWM1_PWMA03, 0x10B0u); 
                    }
                    else if(PWM1_MODULE3_PWMA == L5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_FLEXPWM1_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_FLEXPWM1_PWMA03, 0x10B0u); 
                    }   
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM1_MODULE3_PWMB == A6)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_13_FLEXPWM1_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_13_FLEXPWM1_PWMB03, 0x10B0u);
                    }
                    else if(PWM1_MODULE3_PWMB == B7)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_39_FLEXPWM1_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_39_FLEXPWM1_PWMB03, 0x10B0u);
                    }
                    else if(PWM1_MODULE3_PWMB == B11)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_FLEXPWM1_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_FLEXPWM1_PWMB03, 0x10B0u);
                    }
                    else if(PWM1_MODULE3_PWMB == G10)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_FLEXPWM1_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_FLEXPWM1_PWMB03, 0x10B0u);
                    }
                    else if(PWM1_MODULE3_PWMB == M5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_01_FLEXPWM1_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_FLEXPWM1_PWMB03, 0x10B0u);
                    }                   
                }
                
                break;
            }
          default:
#ifdef DEBUG
            PRINTF("\nPWM ��ʼ��ʧ�ܣ�\n ��ѡ��kPWM_Module_0 - kPWM_Module_3 \n");
#endif
            break;
        }
        
    }
    else if(base == PWM2)
    {
        switch(subModule)      //ѡ��PWM��ģ��
        {
          case 0:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B) 
                {
                    if(PWM2_MODULE0_PWMA == A8)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_FLEXPWM2_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_FLEXPWM2_PWMA00, 0x10B0u);
                    }
                    else if(PWM2_MODULE0_PWMA == H5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_06_FLEXPWM2_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_06_FLEXPWM2_PWMA00, 0x10B0u);
                    }
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)  
                {
                    if(PWM2_MODULE0_PWMB == A9)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_FLEXPWM2_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_FLEXPWM2_PWMB00, 0x10B0u);//ԭA9 ��Ӳ�������޸�ΪB9�ĳ�ʼ������
                        //IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01, 0U);  
                        //IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01, 0x10B0u);
                    }
                    else if(PWM2_MODULE0_PWMB == H4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_07_FLEXPWM2_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_07_FLEXPWM2_PWMB00, 0x10B0u);
                    } 
                }
                break;
            }
          case 1:       
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE1_PWMA == B9)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01, 0x10B0u);
                    }
                    else if(PWM2_MODULE1_PWMA == H3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_08_FLEXPWM2_PWMA01, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_08_FLEXPWM2_PWMA01, 0x10B0u);
                    }
                    
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE1_PWMB == C2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_09_FLEXPWM2_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_09_FLEXPWM2_PWMB01, 0x10B0u);
                    }
                    else if(PWM2_MODULE1_PWMB == C9)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01, 0x10B0u);  
                    }  
                }                                                         
                break;
            }
          case 2:       
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE2_PWMA == D9)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02, 0x10B0u);
                    }
                    else if(PWM2_MODULE2_PWMA == G1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_10_FLEXPWM2_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_10_FLEXPWM2_PWMA02, 0x10B0u); 
                    }
                     
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE2_PWMB == A10)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_11_FLEXPWM2_PWMB02, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_11_FLEXPWM2_PWMB02, 0x10B0u);
                    }
                    else if(PWM2_MODULE2_PWMB == G3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_11_FLEXPWM2_PWMB02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_11_FLEXPWM2_PWMB02, 0x10B0u);
                    }
                    
                }
                break;
            }
          case 3:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE3_PWMA == B4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_19_FLEXPWM2_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_19_FLEXPWM2_PWMA03, 0x10B0u); 
                    }
                    else if(PWM2_MODULE3_PWMA == C11)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_02_FLEXPWM2_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_02_FLEXPWM2_PWMA03, 0x10B0u); 
                    }
                    else if(PWM2_MODULE3_PWMA == F14)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_09_FLEXPWM2_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_09_FLEXPWM2_PWMA03, 0x10B0u); 
                    }
                    else if(PWM2_MODULE3_PWMA == M3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03, 0x10B0u); 
                    }
                    else if(PWM2_MODULE3_PWMA == M14)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_FLEXPWM2_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_00_FLEXPWM2_PWMA03, 0x10B0u); 
                    }   
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM2_MODULE3_PWMB == A3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_20_FLEXPWM2_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_20_FLEXPWM2_PWMB03, 0x10B0u);
                    }
                    else if(PWM2_MODULE3_PWMB == D11)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_FLEXPWM2_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_FLEXPWM2_PWMB03, 0x10B0u);
                    }
                    else if(PWM2_MODULE3_PWMB == H10)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_FLEXPWM2_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_01_FLEXPWM2_PWMB03, 0x10B0u);
                    }
                    else if(PWM2_MODULE3_PWMB == M4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03, 0x10B0u);
                    }                 
                }
                
                break;
            }
          default:
#ifdef DEBUG
            PRINTF("\nPWM ��ʼ��ʧ�ܣ�\n ��ѡ��kPWM_Module_0 - kPWM_Module_3 \n");
#endif
            break;
        }
    }
    else if(base == PWM3)
    {
        switch(subModule)      //ѡ��PWM��ģ��
        {
          case 0:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B) 
                {
                    if(PWM3_MODULE0_PWMA == E1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_29_FLEXPWM3_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_29_FLEXPWM3_PWMA00, 0x10B0u);
                    }
                    
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)  
                {
                    if(PWM3_MODULE0_PWMB == C6)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_30_FLEXPWM3_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_30_FLEXPWM3_PWMB00, 0x10B0u);
                    }
                    
                }
                break;
            }
          case 1:       
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE1_PWMA == C5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_31_FLEXPWM3_PWMA01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_31_FLEXPWM3_PWMA01, 0x10B0u);
                    }
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE1_PWMB == D5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_32_FLEXPWM3_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_32_FLEXPWM3_PWMB01, 0x10B0u);
                    }
                    
                }                                                         
                break;
            }
          case 2:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE2_PWMA == C4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_33_FLEXPWM3_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_33_FLEXPWM3_PWMA02, 0x10B0u);
                    }
                   
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE2_PWMB == D4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_34_FLEXPWM3_PWMB02, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_34_FLEXPWM3_PWMB02, 0x10B0u);
                    }
                    
                }
                break;
            }
          case 3:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE3_PWMA == C1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_21_FLEXPWM3_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_21_FLEXPWM3_PWMA03, 0x10B0u); 
                    }
                      
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM3_MODULE3_PWMB == F1)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_22_FLEXPWM3_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_22_FLEXPWM3_PWMB03, 0x10B0u);
                    }
                           
                }
                
                break;
            }
          default:
#ifdef DEBUG
            PRINTF("\nPWM ��ʼ��ʧ�ܣ�\n ��ѡ��kPWM_Module_0 - kPWM_Module_3 \n");
#endif
            break;
        }
    }
    else if(base == PWM4)
    {
        switch(subModule)      //ѡ��PWM��ģ��
        {
          case 0:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B) 
                {
                    if(PWM4_MODULE0_PWMA == E3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_00_FLEXPWM4_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_00_FLEXPWM4_PWMA00, 0x10B0u);
                    }
                    else if(PWM4_MODULE0_PWMA == H13)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_08_FLEXPWM4_PWMA00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_08_FLEXPWM4_PWMA00, 0x10B0u);
                    }
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)  
                {
                    if(PWM4_MODULE0_PWMB == F3)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_01_FLEXPWM4_PWMB00,0U);
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_01_FLEXPWM4_PWMB00, 0x10B0u);
                    }
                   
                }
                break;
            }
          case 1:       
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE1_PWMA == F4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_02_FLEXPWM4_PWMA01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_02_FLEXPWM4_PWMA01, 0x10B0u);
                    }
                    else if(PWM4_MODULE1_PWMA == M13)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_09_FLEXPWM4_PWMA01, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_09_FLEXPWM4_PWMA01, 0x10B0u);
                    }
                    
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE1_PWMB == G4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_03_FLEXPWM4_PWMB01, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_03_FLEXPWM4_PWMB01, 0x10B0u);
                    }
                     
                }                                                         
                break;
            }
          case 2:       
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE2_PWMA == C14)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_FLEXPWM4_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_14_FLEXPWM4_PWMA02, 0x10B0u);
                    }
                    else if(PWM4_MODULE2_PWMA == F2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_04_FLEXPWM4_PWMA02, 0U);  
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_04_FLEXPWM4_PWMA02, 0x10B0u); 
                    }
                     
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE2_PWMB == G5)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_05_FLEXPWM4_PWMB02, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_05_FLEXPWM4_PWMB02, 0x10B0u);
                    }
                   
                    
                }
                break;
            }
          case 3:        
            {
                if(pwm_channels == kPWM_PwmA || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE3_PWMA == A4)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_17_FLEXPWM4_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_17_FLEXPWM4_PWMA03, 0x10B0u); 
                    }
                    else if(PWM4_MODULE3_PWMA == B14)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_B1_15_FLEXPWM4_PWMA03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_15_FLEXPWM4_PWMA03, 0x10B0u); 
                    }
                    
                }
                if(pwm_channels == kPWM_PwmB || pwm_channels == kPWM_PwmA_B)
                {
                    if(PWM4_MODULE3_PWMB == B2)
                    {
                        IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_18_FLEXPWM4_PWMB03, 0U);   
                        IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_18_FLEXPWM4_PWMB03, 0x10B0u);
                    }
                             
                }
                
                break;
            }
          default:
#ifdef DEBUG
            PRINTF("\nPWM ��ʼ��ʧ�ܣ�\n ��ѡ��kPWM_Module_0 - kPWM_Module_3 \n");
#endif
            break;
        }
    }
    else
    {
#ifdef DEBUG
        PRINTF("\nPWM ��ʼ��ʧ�ܣ�\n��ѡ��PWM1 - PWM4 \n");
#endif
    }
}
