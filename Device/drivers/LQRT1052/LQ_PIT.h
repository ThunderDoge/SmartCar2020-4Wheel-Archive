#ifndef __LQ_PIT_H_
#define __LQ_PIT_H_
/**
  * @brief    PIT定时器初始化
  *
  * @param    channel： PIT通道 kPIT_Chnl_0 - kPIT_Chnl_3
  * @param    count  ： 定时器时间 us
  *
  * @return   
  *
  * @note     PIT和GPT使用 同一时钟源 默认IPG/2时钟 75M
  *
  * @example  LQ_PIT_Init(kPIT_Chnl_0, 5000);  //开启通道0定时器中断，每5000us中断一次
  *
  * @date     2019/6/4 星期二
  */
void LQ_PIT_Init(pit_chnl_t channel, uint32_t count);


/**
  * @brief    测试PIT中断
  *
  * @param    
  *
  * @return   
  *
  * @note     用示波器测 C12 500Hz   A13 100Hz
  *
  * @example  
  *
  * @date     2019/6/4 星期二
  */
//void Test_PIT(void);





#endif  
