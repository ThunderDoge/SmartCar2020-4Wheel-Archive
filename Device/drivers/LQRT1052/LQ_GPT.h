/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2018年2月1日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】IAR8.20.1及以上版本
【Target 】 i.MX RT1052
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 528MHz
【USB PLL】 480MHz
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#ifndef _LQ_GPT_H_
#define _LQ_GPT_H_


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
void Test_GPTTimer(void);


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
void Test_GPTDelay(void);




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
void LQ_GPT_Init(void);



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
void GPT_DelayMs(uint32_t ms);



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
void GPT_DelayUs(uint32_t us);



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
void GPT_TimeStart(void);



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
uint32_t GPT_TimeGetUs(void);













#endif  
