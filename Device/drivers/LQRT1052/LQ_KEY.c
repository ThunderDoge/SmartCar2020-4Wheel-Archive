/*-----------------------------------------------------------------
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月12日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【dev.env.】IAR8.30.1及以上版本
--------------------------------------------------------------------*/
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "LQ_LED.h"
#include "LQ_KEY.h"
#include "LQ_GPIO.h"
/* 声明外部延时函数 */
extern void delayms(uint16_t ms);


#if  1   //使用fsl库 操作GPIO

/**
  * @brief    初始化龙邱母板上的按键
  *
  * @param    
  *
  * @return   
  *
  * @note     GPIO3_IO04 ------ 管脚P2   ----- >  母板按键K2
  * @note     GPIO2_IO27 ------ 管脚C13  ----- >  母板按键K0
  * @note     GPIO2_IO30 ------ 管脚C14  ----- >  母板按键k1
  *
  * @example  
  *
  * @date     2019/6/4 星期二
  */
void KEY_Init(void)
{  
    CLOCK_EnableClock(kCLOCK_Iomuxc);                    // IO口时钟使能
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0U);     //管脚P2  ADD
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_GPIO2_IO27,0U);     //管脚C13 UP
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_GPIO2_IO30,0U);     //管脚C14 DOWN
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_15_GPIO2_IO31,0U);     //管脚B14 SUB                                
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_10_GPIO2_IO26,0U);     //管脚B13 OK
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_05_GPIO3_IO05,0U);  //管脚N3  ESC
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_GPIO1_IO03,0U);  //管脚G11 激光接口1
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_06_GPIO1_IO22,0U);  //管脚J12 激光接口2
    //IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_GPIO2_IO28,0U);  //管脚D13 激光接口2
    //IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_GPIO2_IO09,0U); //管脚C9 霍尔检测
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
    //一般输入口管脚定义
    gpio_pin_config_t GPIO_Input_Config = {kGPIO_DigitalInput,    //GPIO为输入方向
                                            1,                    //高电平
                                            kGPIO_NoIntmode,      //不触发中断
                                            };
    
    GPIO_PinInit(GPIO2, 27, &GPIO_Input_Config);        // GPIO输入口，非中断
    GPIO_PinInit(GPIO2, 30, &GPIO_Input_Config);        // GPIO输入口，非中断
    GPIO_PinInit(GPIO3, 04, &GPIO_Input_Config);        // GPIO输入口，非中断
    GPIO_PinInit(GPIO2, 31, &GPIO_Input_Config);
    GPIO_PinInit(GPIO2, 26, &GPIO_Input_Config);
    GPIO_PinInit(GPIO3, 05, &GPIO_Input_Config);
    
    gpio_pin_config_t GPIO_Input_Config1 = {kGPIO_DigitalInput,    //GPIO为输入方向
                                            0,                    //低电平
                                            kGPIO_NoIntmode,      //不触发中断
                                            };
    GPIO_PinInit(GPIO1, 03, &GPIO_Input_Config1);
    GPIO_PinInit(GPIO1, 22, &GPIO_Input_Config1);
}


/**
  * @brief    读取按键状态
  *
  * @param    mode： 0：不支持连续按;   1：支持连续按;
  *
  * @return   0：无按键按下  1：KEY0按下  2:KEY1按下  3:KEY2按下
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/4 星期二
  */
uint8_t KEY_Read(uint8_t mode)
{
    static uint8_t key_up=1;     //按键松开标志
    if(mode==1) 
    {
        key_up=1;      //支持连按
    }
    if(key_up && (GPIO_PinRead(GPIO2, 27)==0 || GPIO_PinRead(GPIO2, 30)==0 || GPIO_PinRead(GPIO3, 4)==0 || GPIO_PinRead(GPIO2, 31)==0 || GPIO_PinRead(GPIO2, 26)==0 || GPIO_PinRead(GPIO1, 19)==0))
    {
        delayms(10);   //消抖
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
    return 0;   //无按键按下
    
}



/*!
  * @brief    测试GPIIO外部中断
  *
  * @param    
  *
  * @return   
  *
  * @note     按下母板上不同的按键 不同颜色的灯闪烁
  *
  * @example  
  *
  * @date     2019/8/19 星期一
  */
void Test_ExtiKEY(void)
{
    LED_Init();
    
    /*GPIO 外部中断配置开始*/
    CLOCK_EnableClock(kCLOCK_Iomuxc);                         // IO口时钟使能
    gpio_pin_config_t GPIO_Input_Config =                     // GPIO初始化结构体
    {
        .direction = kGPIO_DigitalInput,                      // 输入
        .outputLogic = 0U,                                  
        .interruptMode = kGPIO_IntFallingEdge                 // 下降沿中断
    };
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_GPIO2_IO27,0U);        // 设置管脚复用功能                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_GPIO2_IO27,0x90B0U);// 配置管脚 上拉 
    GPIO_PinInit(GPIO2, 27U, &GPIO_Input_Config);             // GPIO输入口，上拉 下降沿触发中断
    GPIO_PortEnableInterrupts(GPIO2,1<<27);			          // C13 GPIO2_27中断使能
    
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_GPIO2_IO30,0U);        // 设置管脚复用功能                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_14_GPIO2_IO30,0x90B0U);// 配置管脚 上拉
    GPIO_PinInit(GPIO2, 30U, &GPIO_Input_Config);             // GPIO输入口，上拉 下降沿触发中断   
    GPIO_PortEnableInterrupts(GPIO2,1<<30);			          // C14 GPIO2_30中断使能
 
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0U);        // 设置管脚复用功能                          
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_04_GPIO3_IO04,0x90B0U);// 配置管脚 上拉
    GPIO_PinInit(GPIO3, 4U, &GPIO_Input_Config);                 // GPIO输入口，上拉 下降沿触发中断   
    GPIO_PortEnableInterrupts(GPIO3,1<<4);			             // P2 GPIO3_4中断使能
    
    
    //优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断
    NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO2_Combined_16_31_IRQn);			          //使能GPIO2_16~31IO的中断  
    
    //优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断
    NVIC_SetPriority(GPIO3_Combined_0_15_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO3_Combined_0_15_IRQn);			          //使能GPIO3_0~15IO的中断 
    /*GPIO 外部中断配置结束*/  
    
    /* 在irq.c 中 处理中断 */
    while(1);
}


#else //使用封装过的fsl库操作GPIO

/**
  * @brief    初始化龙邱母板上的按键
  *
  * @param    
  *
  * @return   
  *
  * @note     GPIO3_IO04 ------ 管脚P2   ----- >  母板按键K2
  * @note     GPIO2_IO27 ------ 管脚C13  ----- >  母板按键K0
  * @note     GPIO2_IO30 ------ 管脚C14  ----- >  母板按键k1
  *
  * @example  
  *
  * @date     2019/6/4 星期二
  */
void KEY_Init(void)
{  
    /* 设置上拉输入 */
    LQ_PinInit(P2,  PIN_MODE_INPUT_PULLUP, 1);
    LQ_PinInit(C13, PIN_MODE_INPUT_PULLUP, 1);
    LQ_PinInit(C14, PIN_MODE_INPUT_PULLUP, 1);  
}


/**
  * @brief    读取按键状态
  *
  * @param    mode： 0：不支持连续按;   1：支持连续按;
  *
  * @return   0：无按键按下  1：KEY0按下  2:KEY1按下  3:KEY2按下
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/4 星期二
  */
//uint8_t KEY_Read(uint8_t mode)
//{
//    static uint8_t key_up=1;     //按键松开标志
//    if(mode==1) 
//    {
//        key_up=1;      //支持连按
//    }
//    if(key_up && (LQ_PinRead(C13)==0 || LQ_PinRead(C14)==0 || LQ_PinRead(P2)==0))
//    {
//        delayms(10);   //消抖
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
//    return 0;   //无按键按下
//    
//}



/*!
  * @brief    测试GPIIO外部中断
  *
  * @param    
  *
  * @return   
  *
  * @note     按下母板上不同的按键 不同颜色的灯闪烁
  *
  * @example  
  *
  * @date     2019/8/19 星期一
  */
void Test_ExtiKEY(void)
{
    LED_Init();
    
    /* 配置下降沿触发中断 */
    LQ_PinExti(C13, PIN_IRQ_MODE_FALLING);
    LQ_PinExti(C14, PIN_IRQ_MODE_FALLING);
    LQ_PinExti(P2,  PIN_IRQ_MODE_FALLING);
    
    //优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断
    NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO2_Combined_16_31_IRQn);			          //使能GPIO2_16~31IO的中断  
    
    //优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断
    NVIC_SetPriority(GPIO3_Combined_0_15_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
    
	EnableIRQ(GPIO3_Combined_0_15_IRQn);			          //使能GPIO3_0~15IO的中断 
    /*GPIO 外部中断配置结束*/  
    
    /* 在irq.c 中 处理中断 */
    while(1);



}
#endif

/**
  * @brief    测试按键
  *
  * @param    
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/4 星期二
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


