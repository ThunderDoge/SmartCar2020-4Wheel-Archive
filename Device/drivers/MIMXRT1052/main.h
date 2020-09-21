#ifndef __MAIN_H__
#define __MAIN_H__

    #include "stdio.h"
    #include "math.h"
//SDK底层驱动库
    #include "fsl_adc.h"
    #include "fsl_cache.h"
    #include "fsl_csi.h"
    #include "fsl_csi_camera_adapter.h"
    #include "fsl_camera.h"
    #include "fsl_camera_receiver.h"
    #include "fsl_camera_device.h"
    #include "fsl_csi.h"
    #include "fsl_common.h"
    #include "fsl_clock.h"
    #include "fsl_debug_console.h"
    #include "fsl_elcdif.h"
    #include "fsl_enc.h"    
    #include "fsl_gpio.h"
    #include "fsl_gpt.h"
    #include "fsl_iomuxc.h"
    #include "fsl_lpuart.h"
    #include "fsl_lpi2c.h"
    #include "fsl_lpspi.h"
    #include "fsl_pxp.h"
    #include "fsl_pwm.h"
    #include "fsl_pit.h"
    #include "fsl_qtmr.h"
    #include "fsl_trng.h"
    #include "fsl_usdhc.h"
    #include "fsl_video_common.h"
    #include "fsl_wdog.h"
    #include "fsl_xbara.h"
    #include "board.h"
    #include "pin_mux.h"
    #include "fsl_sccb.h"
    #include "fsl_flexspi.h"
    #include "fsl_lpuart_edma.h"
    #include "fsl_edma.h"
    #include "fsl_dmamux.h"

//用户自定义文件    
    #include "LQ_IIC.h"
    //#include "LQ_GPIOmacro.h" //GPIO口51操作格式 
    #include "LQ_MT9V034M.h"   //神眼摄像头V3   
    #include "LQ_LED.h"       //LED指示
    #include "LQ_KEY.h"       //独立按键
    #include "LQ_PWM.h"       //电机，舵机PWM控制 XBARA
    #include "LQ_UART.h"       //UART串口
    #include "LQ_ADC.h"       //ADC转换采集
    #include "LQ_MPU6050.h"    //MPU6050
    #include "LQ_Encoder.h"   //编码器正交解码数据采集
    #include "LQ_CSI.h"       //摄像头接口
    //#include "m_systime.h"    //systick 定时器，用于计时
    //#include "status.h"       //标志位
    #include "LQ_QTMR.h"      //计时器可用于生成PWM  和 正交解
    #include "LQ_PIT.h"  
    #include "LQ_SPI.h" 
    //#include "YC_OLED.h"   
    //#include "YC_PID.h"        //PID
    //#include "YC_Control.h"        //PID
    //#include "YC_Image.h"        //PID
    #include "LQ_Flash.h"
    #include "LQ_12864.h"
    #include "szx_image.h"
//用户自定义函数
    void delayus(uint32_t us);
    void delayms(uint32_t ms);   

extern void InitAll(void);

#endif
