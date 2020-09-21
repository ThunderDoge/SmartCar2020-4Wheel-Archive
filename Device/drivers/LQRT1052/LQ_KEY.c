/*-----------------------------------------------------------------
��ƽ    ̨������i.MX RT1064���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2019��3��12��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
��dev.env.��IAR8.30.1�����ϰ汾
--------------------------------------------------------------------*/
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "LQ_LED.h"
#include "LQ_KEY.h"
#include "LQ_GPIO.h"
/* �����ⲿ��ʱ���� */
extern void delayms(uint16_t ms);


#if  1   //ʹ��fsl�� ����GPIO

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
void KEY_Init(void)
{  
    CLOCK_EnableClock(kCLOCK_Iomuxc);                    // IO��ʱ��ʹ��
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0U);     //�ܽ�P2  ADD
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_GPIO2_IO27,0U);     //�ܽ�C13 UP
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_GPIO2_IO30,0U);     //�ܽ�C14 DOWN
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_15_GPIO2_IO31,0U);     //�ܽ�B14 SUB                                
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_10_GPIO2_IO26,0U);     //�ܽ�B13 OK
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_05_GPIO3_IO05,0U);  //�ܽ�N3  ESC
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0U);  //�ܽ�G11 ����ӿ�1
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_06_GPIO1_IO22,0U);  //�ܽ�J12 ����ӿ�2
    //IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_GPIO2_IO28,0U);  //�ܽ�D13 ����ӿ�2
    //IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_GPIO2_IO09,0U); //�ܽ�C9 �������
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_GPIO2_IO27,0xF080); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_14_GPIO2_IO30,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_15_GPIO2_IO31,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_10_GPIO2_IO26,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_05_GPIO3_IO05,0xF080);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0xF080);//0x10B0u
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_06_GPIO1_IO22,0xF080);
    //IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_12_GPIO2_IO28,0xF080);//0x10B0u
    //IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_GPIO2_IO09,0xF080);
    //һ������ڹܽŶ���
    gpio_pin_config_t GPIO_Input_Config = {kGPIO_DigitalInput,    //GPIOΪ���뷽��
                                            1,                    //�ߵ�ƽ
                                            kGPIO_NoIntmode,      //�������ж�
                                            };
    
    GPIO_PinInit(GPIO2, 27, &GPIO_Input_Config);        // GPIO����ڣ����ж�
    GPIO_PinInit(GPIO2, 30, &GPIO_Input_Config);        // GPIO����ڣ����ж�
    GPIO_PinInit(GPIO3, 04, &GPIO_Input_Config);        // GPIO����ڣ����ж�
    GPIO_PinInit(GPIO2, 31, &GPIO_Input_Config);
    GPIO_PinInit(GPIO2, 26, &GPIO_Input_Config);
    GPIO_PinInit(GPIO3, 05, &GPIO_Input_Config);
    
    gpio_pin_config_t GPIO_Input_Config1 = {kGPIO_DigitalInput,    //GPIOΪ���뷽��
                                            0,                    //�͵�ƽ
                                            kGPIO_NoIntmode,      //�������ж�
                                            };
    GPIO_PinInit(GPIO1, 03, &GPIO_Input_Config1);
    GPIO_PinInit(GPIO1, 22, &GPIO_Input_Config1);
}


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
uint8_t KEY_Read(uint8_t mode)
{
    static uint8_t key_up=1;     //�����ɿ���־
    if(mode==1) 
    {
        key_up=1;      //֧������
    }
    if(key_up && (GPIO_PinRead(GPIO2, 27)==0 || GPIO_PinRead(GPIO2, 30)==0 || GPIO_PinRead(GPIO3, 4)==0 || GPIO_PinRead(GPIO2, 31)==0 || GPIO_PinRead(GPIO2, 26)==0 || GPIO_PinRead(GPIO1, 19)==0))
    {
        delayms(10);   //����
        key_up=0;
        if(GPIO_PinRead(GPIO2, 27)==0)      
        {
            return ADD;
        }
        
        else if(GPIO_PinRead(GPIO2, 30)==0) 
        {
            return DOWN;    
        }
        
        else if(GPIO_PinRead(GPIO3,  04)==0) 
        {
            return SUB;    
        }
        
        else if(GPIO_PinRead(GPIO2, 31)==0) 
        {
            return ESC;    
        }
        
        else if(GPIO_PinRead(GPIO2, 26)==0) 
        {
            return OK;    
        }
        
        else if(GPIO_PinRead(GPIO3, 05)==0) 
        {
            return UP;    
        }
    }
    if(GPIO_PinRead(GPIO2, 27)==1 && GPIO_PinRead(GPIO2, 30)==1 && GPIO_PinRead(GPIO3,  4)==1 && GPIO_PinRead(GPIO2, 31)==1 && GPIO_PinRead(GPIO2, 26)==1 && GPIO_PinRead(GPIO3, 5)==1) 
    {
        key_up=1;   
    }
    return 0;   //�ް�������
    
}



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
void Test_ExtiKEY(void)
{
    LED_Init();
    
    /*GPIO �ⲿ�ж����ÿ�ʼ*/
    CLOCK_EnableClock(kCLOCK_Iomuxc);                         // IO��ʱ��ʹ��
    gpio_pin_config_t GPIO_Input_Config =                     // GPIO��ʼ���ṹ��
    {
        .direction = kGPIO_DigitalInput,                      // ����
        .outputLogic = 0U,                                  
        .interruptMode = kGPIO_IntFallingEdge                 // �½����ж�
    };
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_GPIO2_IO27,0U);        // ���ùܽŸ��ù���                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_GPIO2_IO27,0x90B0U);// ���ùܽ� ���� 
    GPIO_PinInit(GPIO2, 27U, &GPIO_Input_Config);             // GPIO����ڣ����� �½��ش����ж�
    GPIO_PortEnableInterrupts(GPIO2,1<<27);			          // C13 GPIO2_27�ж�ʹ��
    
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_GPIO2_IO30,0U);        // ���ùܽŸ��ù���                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_14_GPIO2_IO30,0x90B0U);// ���ùܽ� ����
    GPIO_PinInit(GPIO2, 30U, &GPIO_Input_Config);             // GPIO����ڣ����� �½��ش����ж�   
    GPIO_PortEnableInterrupts(GPIO2,1<<30);			          // C14 GPIO2_30�ж�ʹ��
 
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0U);        // ���ùܽŸ��ù���                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0x90B0U);// ���ùܽ� ����
    GPIO_PinInit(GPIO3, 4U, &GPIO_Input_Config);                 // GPIO����ڣ����� �½��ش����ж�   
    GPIO_PortEnableInterrupts(GPIO3,1<<4);			             // P2 GPIO3_4�ж�ʹ��
    
    
    //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
    NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO2_Combined_16_31_IRQn);			          //ʹ��GPIO2_16~31IO���ж�  
    
    //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
    NVIC_SetPriority(GPIO3_Combined_0_15_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO3_Combined_0_15_IRQn);			          //ʹ��GPIO3_0~15IO���ж� 
    /*GPIO �ⲿ�ж����ý���*/  
    
    /* ��irq.c �� �����ж� */
    while(1);
}


#else //ʹ�÷�װ����fsl�����GPIO

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
void KEY_Init(void)
{  
    /* ������������ */
    LQ_PinInit(P2,  PIN_MODE_INPUT_PULLUP, 1);
    LQ_PinInit(C13, PIN_MODE_INPUT_PULLUP, 1);
    LQ_PinInit(C14, PIN_MODE_INPUT_PULLUP, 1);  
}


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
//uint8_t KEY_Read(uint8_t mode)
//{
//    static uint8_t key_up=1;     //�����ɿ���־
//    if(mode==1) 
//    {
//        key_up=1;      //֧������
//    }
//    if(key_up && (LQ_PinRead(C13)==0 || LQ_PinRead(C14)==0 || LQ_PinRead(P2)==0))
//    {
//        delayms(10);   //����
//        key_up=0;
//        if(LQ_PinRead(C13)==0)      
//        {
//            return 1;
//        }
//        
//        else if(LQ_PinRead(C14)==0) 
//        {
//            return 2;    
//        }
//        
//        else if(LQ_PinRead(P2)==0) 
//        {
//            return 3;    
//        }
//        
//    }
//    if(LQ_PinRead(C13)==1 && LQ_PinRead(C14)==1 && LQ_PinRead(P2)==1) 
//    {
//        key_up=1;   
//    }
//    return 0;   //�ް�������
//    
//}



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
void Test_ExtiKEY(void)
{
    LED_Init();
    
    /* �����½��ش����ж� */
    LQ_PinExti(C13, PIN_IRQ_MODE_FALLING);
    LQ_PinExti(C14, PIN_IRQ_MODE_FALLING);
    LQ_PinExti(P2,  PIN_IRQ_MODE_FALLING);
    
    //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
    NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO2_Combined_16_31_IRQn);			          //ʹ��GPIO2_16~31IO���ж�  
    
    //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
    NVIC_SetPriority(GPIO3_Combined_0_15_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO3_Combined_0_15_IRQn);			          //ʹ��GPIO3_0~15IO���ж� 
    /*GPIO �ⲿ�ж����ý���*/  
    
    /* ��irq.c �� �����ж� */
    while(1);



}
#endif

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
void Test_KEY(void)
{ 
    LED_Init();
    KEY_Init(); 
    while (1)
    {  
        switch(KEY_Read(1))  
        {
            case 1:
                LED_Color(red);     
                break;           
            case 2:      
                LED_Color(green);   
                break;
            case 3:      
                LED_Color(blue);    
                break;
            default:
                LED_Color(white);   
                break;
        }

        delayms(50);
    }
}


