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
#include "fsl_pit.h"
#include "LQ_GPIO_Cfg.h"
#include "LQ_PIT.h"
#include "LQ_LED.h"

/*����PIT��  volatile�ױ�ģ����ж������ı�ı�������ʱ��ü��ϣ����߱�������Ҫ�Ż���ÿ�ζ�ȡ���Ӹñ����ĵ�ַ��ȡ*/
//volatile uint16_t pitIsrCnt0 = 0;   
//volatile uint16_t pitIsrCnt1 = 0;
//volatile uint16_t pitIsrCnt2 = 0; 
//volatile uint16_t pitIsrCnt3 = 0; 

/**
  * @brief    ����PIT�ж�
  *
  * @param    
  *
  * @return   
  *
  * @note     ��ʾ������ C12 500Hz   A13 100Hz
  *
  * @example  
  *
  * @date     2019/6/4 ���ڶ�
  */
//void Test_PIT(void)
//{ 
//    LED_Init();                             //��ʼ��LED
//    
//    //LQ_PIT_Init(kPIT_Chnl_0, 1000000UL);       //1 000us һ���ж�
//    //LQ_PIT_Init(kPIT_Chnl_1, 5000000UL);       //5 000us һ���ж�
//    //LQ_PIT_Init(kPIT_Chnl_2, 2000000UL);    //1000 000us һ���ж�
//    
//    while (true)
//    {
//        if (pitIsrCnt0)
//        {
//            PT2_22 = !PT2_22;             //��ת C12         
//            pitIsrCnt0 = 0;
//            LED_Color(red);
//        }
//        if (pitIsrCnt1)
//        {
//            PT2_25 = !PT2_25;             //��ת A13    
//            pitIsrCnt1=0;
//            LED_Color(green);
//        }
//        if (pitIsrCnt2)
//        {
//            LED_ColorReverse(red); 
//            pitIsrCnt2=0;
//            LED_Color(yellow);
//        }
//    }
//}


/**
  * @brief    PIT��ʱ����ʼ��
  *
  * @param    channel�� PITͨ�� kPIT_Chnl_0 - kPIT_Chnl_3
  * @param    count  �� ��ʱ��ʱ�� us
  *
  * @return   
  *
  * @note     PIT��GPTʹ�� ͬһʱ��Դ Ĭ��IPG/2ʱ�� 75M
  *
  * @example  LQ_PIT_Init(kPIT_Chnl_0, 5000);  //����ͨ��0��ʱ���жϣ�ÿ5000us�ж�һ��
  *
  * @date     2019/6/4 ���ڶ�
 


 */

void LQ_PIT_Init(pit_chnl_t channel, uint32_t count)
{
    static uint8_t pit_count = 0;
    
    /* ��ֹPIT��γ�ʼ�� */
    if(pit_count == 0)
    {
        pit_count++;
        
        /* PIT��ʼ���ṹ�� */
        pit_config_t pitConfig;
        
        //CLOCK_SetMux(kCLOCK_PerclkMux, 1U);
        //CLOCK_SetDiv(kCLOCK_PerclkDiv, 0U);
        
        /*
        * pitConfig.enableRunInDebug = false;  
        */
        PIT_GetDefaultConfig(&pitConfig);   //��ȡĬ�ϲ���
        
        pitConfig.enableRunInDebug = false;
        PIT_Init(PIT, &pitConfig);          //��ʼ��PIT
        
        //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
        NVIC_SetPriority(PIT_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));//PIT_IRQnΪ�ж�������
        
        /* Enable at the NVIC */
        EnableIRQ(PIT_IRQn);                //ʹ���ж�
        
    }
    
    /* ����PITͨ���Ͷ�ʱʱ�� PITĬ��ʹ��IPGʱ�ӵĶ���Ƶ 75Mʱ�� */
    PIT_SetTimerPeriod(PIT, channel, USEC_TO_COUNT(count, CLOCK_GetFreq(kCLOCK_IpgClk)/2)); 
    
    //����жϱ�־λ
    PIT_ClearStatusFlags(PIT, channel, kPIT_TimerFlag); 
    
    //ʹ��ͨ���ж�
    PIT_EnableInterrupts(PIT, channel, kPIT_TimerInterruptEnable);  

    //������ʱ��
    PIT_StartTimer(PIT, channel);   
}

//void LQ_PIT_Init(pit_chnl_t channel, uint32_t count)
//{
//        /* PIT��ʼ���ṹ�� */
//        pit_config_t pitConfig;                
//        
//        /* Set PERCLK_CLK source to OSC_CLK*/
//        CLOCK_SetMux(kCLOCK_PerclkMux, 1U);
//        /* Set PERCLK_CLK divider to 1 */
//        CLOCK_SetDiv(kCLOCK_PerclkDiv, 0U);
//        
//        /*
//        * pitConfig.enableRunInDebug = false;  
//        */
//        PIT_GetDefaultConfig(&pitConfig);   //��ȡĬ�ϲ���
//        
//        pitConfig.enableRunInDebug = true;
//        PIT_Init(PIT, &pitConfig);          //��ʼ��PIT
//        
//        /* Set timer period for channel  */
//        PIT_SetTimerPeriod(PIT, channel, USEC_TO_COUNT(count, CLOCK_GetFreq(kCLOCK_OscClk))); //����PITͨ���Ͷ�ʱʱ��
//        
//        /* Enable timer interrupts for channel  */
//        PIT_EnableInterrupts(PIT, channel, kPIT_TimerInterruptEnable);  //ʹ��ͨ���ж�
//        
//        //���ȼ����� ��ռ���ȼ�1  �����ȼ�2   ԽС���ȼ�Խ��  ��ռ���ȼ��ɴ�ϱ���ж�
//        NVIC_SetPriority(PIT_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,2));
//        
//        /* Enable at the NVIC */
//        EnableIRQ(PIT_IRQn);                //ʹ���ж�
//        
//    
//    
//    /* ����PITͨ���Ͷ�ʱʱ�� PITĬ��ʹ��IPGʱ�ӵĶ���Ƶ 75Mʱ�� */
//    //PIT_SetTimerPeriod(PIT, channel, USEC_TO_COUNT(count, CLOCK_GetFreq(kCLOCK_IpgClk)/2)); 
//    
//    //����жϱ�־λ
//    //PIT_ClearStatusFlags(PIT, channel, kPIT_TimerFlag); 
//    
//    //ʹ��ͨ���ж�
//    //PIT_EnableInterrupts(PIT, channel, kPIT_TimerInterruptEnable);  
//
//    //������ʱ��
//        
//        /* Start channel  */
//    //PRINTF("\r\nStarting channel ...");
//        
//    PIT_StartTimer(PIT, channel);   
//}

