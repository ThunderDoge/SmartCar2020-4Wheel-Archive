/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】Z
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2018年12月22日
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
================================================*/
#include "LQ_SPI.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_lpspi.h"

lpspi_master_config_t lpspi1_config; //LPSPI1主机配置结构体
lpspi_master_config_t lpspi3_config; //LPSPI3主机配置结构体
lpspi_master_config_t lpspi4_config; //LPSPI4主机配置结构体

void LPSPI3_Init(uint8_t baudrate)
{
    uint32_t lpspiclk=0;
    uint32_t spi3_baudrate=baudrate*1000000;
    
    //设置LPSPI的时钟源为PLL3_PFD0/(1+5)=576Mhz/(1+5)=96MHz,W25Q256时钟最高104MHz
    CLOCK_SetMux(kCLOCK_LpspiMux,1);    //USB1 PLL PFD0(576MHz)作为LPSPI时钟源
    CLOCK_SetDiv(kCLOCK_LpspiDiv,5);    //分频设置，分频值为5+1，所以LPSPI CLK=576/(1+5)=96Mhz
    
    //配置IO作为LPSPI3，片选采用软件的，这样一个SPI的外设就不会限制为4个!
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI,0);                                    
    
    //配置LPSPI引脚功能，
    //低转换速度,驱动能力为R0/6,速度为100Mhz，开路输出，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0x10B0);                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0x10B0);                                                                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI,0x10B0);                                
    
    //配置LPSPI3主模式
    lpspiclk=(CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk)/(5+1));//LPSPI3时钟
    lpspi3_config.baudRate=spi3_baudrate;               //SPI速度
    lpspi3_config.whichPcs=kLPSPI_Pcs1;                 //片选信号，PCS1
    lpspi3_config.pcsActiveHighOrLow=kLPSPI_PcsActiveLow;//片选信号低电平有效
    lpspi3_config.bitsPerFrame=8;                       //设置SPI的数据大小:SPI发送接收8位帧结构
    lpspi3_config.cpol=kLPSPI_ClockPolarityActiveLow;   //串行同步时钟低电平有效
    lpspi3_config.cpha=kLPSPI_ClockPhaseSecondEdge;      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    lpspi3_config.direction=kLPSPI_MsbFirst;            //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    lpspi3_config.pinCfg=kLPSPI_SdiInSdoOut;            //SDI输入引脚，SDO输出引脚
    lpspi3_config.dataOutConfig=kLpspiDataOutRetained;  //输出数据保留
    lpspi3_config.pcsToSckDelayInNanoSec=10;            //片选拉低到时钟有效之间的延时时间,单位ns
    lpspi3_config.lastSckToPcsDelayInNanoSec=10;        //最后一个时钟到片选拉高之间的延时，单位ns
    lpspi3_config.betweenTransferDelayInNanoSec=10;     //两次传输之间的延时，单位ns
    LPSPI_MasterInit(LPSPI3,&lpspi3_config,lpspiclk);
    LPSPI_Enable(LPSPI3,true);                          //使能LPSPI3
}

//LPSPI3 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t LPSPI3_ReadWriteByte(uint8_t TxData)
{ 
    uint8_t spirxdata=0;
    uint8_t spitxdata=TxData;
    lpspi_transfer_t spi_tranxfer;
    
    spi_tranxfer.configFlags=kLPSPI_MasterPcs1|kLPSPI_MasterPcsContinuous;     //PCS1
    spi_tranxfer.txData=&spitxdata;                 //要发送的数据
    spi_tranxfer.rxData=&spirxdata;                 //接收到的数据
    spi_tranxfer.dataSize=1;                        //数据长度
    LPSPI_MasterTransferBlocking(LPSPI3,&spi_tranxfer);	    //SPI阻塞发送   					    
    return spirxdata;
}

/**
  * @brief    SPI 主机模式初始化
  *
  * @param    base      ：  LPSPI1 - LPSPI4
  * @param    cs_num    ：  片选引脚编号 
  * @param      mode            0: CLK空闲时刻为低电平 在第一个CLK跳边沿（下降沿）捕获数据
  * @param      mode            1: CLK空闲时刻为低电平 在第二个CLK跳边沿（上升沿）捕获数据
  * @param      mode            2: CLK空闲时刻为高电平 在第一个CLK跳边沿（上升沿）捕获数据
  * @param      mode            3: CLK空闲时刻为高电平 在第二个CLK跳边沿（下降沿）捕获数据
  * @param    baudrate  ：  SPI 波特率    
  *
  * @return   注意 ICM20602 时钟空闲时为高电平 时钟有效时为低电平 在时钟第二个跳边沿传输数据    
  *
  * @note     SPI 默认时钟源 66M
  *
  * @example  
  *
  * @date     2019/6/3 星期一
  */
void LQ_SPI_Master_Init(LPSPI_Type * base, uint8_t cs_num, uint8_t mode, uint32_t baudrate)
{
    SPI_PinInit(base, cs_num);
    
    lpspi_master_config_t masterConfig;
    
    /*Master config*/
    masterConfig.baudRate = baudrate;                  //设置波特率
    masterConfig.bitsPerFrame = 8;                     //设置SPI的数据大小:SPI发送接收8位帧结构
    if(mode / 2)
    {
        masterConfig.cpol = kLPSPI_ClockPolarityActiveLow;  //时钟空闲时为高电平
    }
    else
    {
        masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh; //时钟空闲时为低电平
    }
    
    if(mode % 2)
    {
        masterConfig.cpha = kLPSPI_ClockPhaseSecondEdge;  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    }
    else
    {
        masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;  //串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    }
    
    masterConfig.direction = kLPSPI_MsbFirst;          //MSB在前

    masterConfig.pcsToSckDelayInNanoSec = 10;       //片选拉低到开始传输的时间
    masterConfig.lastSckToPcsDelayInNanoSec = 10;   //最后一个信号到片选拉高时间
    masterConfig.betweenTransferDelayInNanoSec = 10;//两次传输之间的时间

    masterConfig.whichPcs = (lpspi_which_pcs_t)cs_num;     //片选引脚
    masterConfig.pcsActiveHighOrLow = kLPSPI_PcsActiveLow; //片选信号低电平有效

    masterConfig.pinCfg = kLPSPI_SdiInSdoOut;              //SDI输入引脚，SDO输出引脚
    masterConfig.dataOutConfig = kLpspiDataOutRetained;    //保留输出数据

    LPSPI_MasterInit(base, &masterConfig, 66000000UL);

}


/**
  * @brief    SPI读写N个字节
  *
  * @param    base    ：  LPSPI1 - LPSPI4
  * @param    cs_num  ：  片选引脚编号  
  * @param    txData  ：  写入的buff
  * @param    rxData  ：  读出的buff
  * @param    len     ：  长度
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/3 星期一
  */
uint8_t SPI_ReadWriteNByte(LPSPI_Type * base, uint8_t cs_num, uint8_t *txData, uint8_t *rxData, uint32_t len)
{

    lpspi_transfer_t spi_tranxfer;
    spi_tranxfer.txData = txData;   //发送数组
    spi_tranxfer.rxData = rxData;   //接收数组
    spi_tranxfer.dataSize = len;    //长度
    spi_tranxfer.configFlags = cs_num|kLPSPI_MasterPcsContinuous;     //片选 
    
    return LPSPI_MasterTransferBlocking(base, &spi_tranxfer);        //spi堵塞传输;
}



/**
  * @brief    SPI 管脚初始化
  *
  * @param    base      ：  LPSPI1 - LPSPI4
  * @param    cs_num    ：  片选引脚编号  
  *
  * @return   
  *
  * @note     第4章管脚复用
  *
  * @example  
  *
  * @date     2019/6/3 星期一
  */
void SPI_PinInit(LPSPI_Type * base, uint8_t cs_num) 
{
    CLOCK_EnableClock(kCLOCK_Iomuxc);           
    
    if(base == LPSPI1)
    {
        if(SPI1_SCK == J4)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK,0U);
        }
        else if(SPI1_SCK == A2)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_27_LPSPI1_SCK,0U);
        }
        
        if(SPI1_SDI == K1)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_03_LPSPI1_SDI,0U);
        }
        else if(SPI1_SDI == E1)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_29_LPSPI1_SDI, 0U); 
        }
        
        if(SPI1_SDO == J1)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO, 0U); 
        }
        else if(SPI1_SDO == D1)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_28_LPSPI1_SDO, 0U);
        }
        
        switch(cs_num)
        {
          case 0:
            if(SPI1_CS0 == J3)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_LPSPI1_PCS0, 0U); 
            }
            else if(SPI1_CS0 == C6)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_30_LPSPI1_PCS0, 0U);
            }
            break;
            
          case 1:
            if(SPI1_CS1 == C5)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_31_LPSPI1_PCS1, 0U);
            }
            break;
            
          case 2:
            if(SPI1_CS2 == A7)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_LPSPI1_PCS2, 0U);
            }
            break;
            
          case 3:
            if(SPI1_CS3 == C7)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_41_LPSPI1_PCS3, 0U);
            }
            break;
        }    
    }
    else if(base == LPSPI2)
    {
        if(SPI2_SCK == L4)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK,0U);
        }
        else if(SPI2_SCK == E3)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_00_LPSPI2_SCK,0U);
        }
        
        if(SPI2_SDI == G4)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_03_LPSPI2_SDI,0U);
        }
        else if(SPI2_SDI == N4)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI, 0U); 
        }
        
        if(SPI2_SDO == F4)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_02_LPSPI2_SDO, 0U); 
        }
        else if(SPI2_SDO == P3)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_LPSPI2_SD0, 0U);
        }
        
        switch(cs_num)
        {
          case 0:
            if(SPI2_CS0 == F3)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_01_LPSPI2_PCS0, 0U); 
            }
            else if(SPI2_CS0 == L3)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0, 0U);
            }
            break;
            
          case 1:
            if(SPI2_CS1 == B6)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_14_LPSPI2_PCS1, 0U);
            }
            break;
            
          case 2:
            if(SPI2_CS2 == P4)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_LPSPI2_PCS2, 0U);
            }
            break;
            
          case 3:
            if(SPI2_CS3 == P5)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_LPSPI2_PCS3, 0U);
            }
            break;
        }    
    }
    else if(base == LPSPI3)
    {
        if(SPI3_SCK == J14)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_LPSPI3_SCK,0U);
        }
        else if(SPI3_SCK == M14)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0U);
        }
        
        if(SPI3_SDI == H11)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI,0U);
        }
        else if(SPI3_SDI == M11)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI, 0U); 
        }
        
        if(SPI3_SDO == H10)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO, 0U); 
        }
        else if(SPI3_SDO == G12)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO, 0U);
        }
        
        switch(cs_num)
        {
          case 0:
            if(SPI3_CS0 == G11)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_LPSPI3_PCS0, 0U); 
            }
            else if(SPI3_CS0 == H12)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_12_LPSPI3_PCS0, 0U);
            }
            break;
            
          case 1:
            if(SPI3_CS1 == F11)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_04_LPSPI3_PCS1, 0U);
            }
            break;
            
          case 2:
            if(SPI3_CS2 == G14)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_LPSPI3_PCS2, 0U);
            }
            break;
            
          case 3:
            if(SPI3_CS3 == E14)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_06_LPSPI3_PCS3, 0U);
            }
            break;
        }    
    }
    else if(base == LPSPI4)
    {
        if(SPI4_SCK == D8)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LPSPI4_SCK,0U);
        }
        else if(SPI4_SCK == B12)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B1_07_LPSPI4_SCK,0U);
        }
        
        if(SPI4_SDI == E7)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LPSPI4_SDI,0U);
        }
        else if(SPI4_SDI == D12)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B1_05_LPSPI4_SDI, 0U); 
        }
        
        if(SPI4_SDO == E8)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LPSPI4_SDO, 0U); 
        }
        else if(SPI4_SDO == C12)
        {
            IOMUXC_SetPinMux(IOMUXC_GPIO_B1_06_LPSPI4_SDO, 0U);
        }
        
        switch(cs_num)
        {
          case 0:
            if(SPI4_CS0 == D7)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LPSPI4_PCS0, 0U); 
            }
            else if(SPI4_CS0 == E12)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_B1_04_LPSPI4_PCS0, 0U);
            }
            break;
            
          case 1:
            if(SPI4_CS1 == D11)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_LPSPI4_PCS1, 0U);
            }
            break;
            
          case 2:
            if(SPI4_CS3 == C11)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_B1_02_LPSPI4_PCS2, 0U);
            }
            break;
            
          case 3:
            if(SPI4_CS3 == C13)
            {
                IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_LPSPI4_PCS3, 0U);
            }
            break;
        }    
    }
                               
}


