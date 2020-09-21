/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月13日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】IAR8.30.1及以上版本
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "fsl_gpt.h"
#include "LQ_GPT.h"
#include "LQ_LED.h"
/**
  * @brief    GPT 通用定时器初始化
  *
  * @param    
  *
  * @return   
  *
  * @note     初始化GPT计数器  GPT1用与程序计时器  GPT2用作精确延时
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
void LQ_GPT_Init(void)
{
    gpt_config_t gptConfig;
    
    GPT_GetDefaultConfig(&gptConfig);
    
    GPT_Init(GPT1, &gptConfig);          //初始化GPT1
    GPT_Init(GPT2, &gptConfig);          //初始化GPT2
    
    /* GPT 和 PIT 使用同一时钟源IGP的2分频 默认75M */
    GPT_SetClockDivider(GPT1, 75);       // GPT1 时钟 75M / 75 = 1M
    GPT_SetClockDivider(GPT2, 15);       // GPT2 时钟 75M /  5 = 5M
    
    GPT_SetOutputCompareValue(GPT1, kGPT_OutputCompare_Channel1, 0XFFFFFFFF);   //GPT 是32为计数器 设置最大计数值 0xFFFF FFFF 
    GPT_SetOutputCompareValue(GPT2, kGPT_OutputCompare_Channel1, 0XFFFFFFFF);   //GPT 是32为计数器 设置最大计数值 0xFFFF FFFF 

}




/**
  * @brief    gpt 定时器精确延时
  *
  * @param    ms ： 延时时间 ms
  *
  * @return   
  *
  * @note     GPT 是32为计数器 最大延时4294967295 / 5 000000 = 868s
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
void GPT_DelayMs(uint32_t ms)
{
    GPT_DelayUs(ms * 1000);
}



/**
  * @brief    gpt 定时器精确延时
  *
  * @param    us ： 延时时间 us
  *
  * @return   
  *
  * @note     GPT 是32为计数器 最大延时4294967295 / 5 000000 = 868s
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
inline void GPT_DelayUs(uint32_t us)
{
    /* Start Timer */
    GPT_StartTimer(GPT2);
    uint32_t time = us * 5 - 4;
    while(GPT_GetCurrentTimerCount(GPT2) <= time);
    
    GPT_StopTimer(GPT2);
}



/**
  * @brief    gpt 计时器开始计时
  *
  * @param   
  *
  * @return   
  *
  * @note     GPT 是32为计数器 最大延时4294967295 / 1 000000 = 4290s 
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
void GPT_TimeStart(void)
{
    GPT_StopTimer(GPT1);
    GPT_StartTimer(GPT1); 
}



/**
  * @brief    gpt 计时器开始计时
  *
  * @param   
  *
  * @return   
  *
  * @note     GPT 是32为计数器 最大延时4294967295 / 1 000000 = 4290s 
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
uint32_t GPT_TimeGetUs(void)
{
    if(GPT_GetStatusFlags(GPT1, kGPT_RollOverFlag))   //溢出
    {
#ifdef DEBUG
        PRINTF("\n GPT 计时器溢出！\n");
#endif
        return 0;
    }
        
    else
        return GPT_GetCurrentTimerCount(GPT1) - 1;
}



/**
  * @brief    测试GPT 延时
  *
  * @param    
  *
  * @return   
  *
  * @note     示波器 A7   500Hz翻转
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
void Test_GPTDelay(void)
{
    LQ_GPT_Init();                       //初始化GPT计数器  GPT1用与程序计时器  GPT2用作精确延时            
    
    LED_Init();                          //LED初始化
    GPT_DelayMs(100);

    while(1)
    {
        GPT_DelayMs(1);  
        LED_ColorReverse(red);           //翻转红灯 用于指示程序运行
    }
}


/**
  * @brief    测试GPT 计时器
  *
  * @param    
  *
  * @return   
  *
  * @note     示波器 A7   500Hz翻转
  *
  * @example  
  *
  * @date     2019/6/10 星期一
  */
void Test_GPTTimer(void)
{
    LQ_GPT_Init();                       //初始化GPT计数器  GPT1用与程序计时器  GPT2用作精确延时            
    
    LED_Init();                          //LED初始化
    GPT_DelayMs(100);

    uint32_t now_time = 0;
    GPT_TimeStart();
    while(1)
    {
        if(GPT_TimeGetUs() - now_time >= 1000) 
        {
            now_time = GPT_TimeGetUs();
            
            LED_ColorReverse(red);       //翻转红灯 用于指示程序运行
        }  
    }
}
