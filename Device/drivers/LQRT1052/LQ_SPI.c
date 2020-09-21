/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨������i.MX RT1052���İ�-���ܳ���
����    д��Z
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2018��12��22��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��IAR8.20.1�����ϰ汾
��Target �� i.MX RT1052
��Crystal�� 24.000Mhz
��ARM PLL�� 1200MHz
��SYS PLL�� 528MHz
��USB PLL�� 480MHz
================================================*/
#include "LQ_SPI.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_lpspi.h"

lpspi_master_config_t lpspi1_config; //LPSPI1�������ýṹ��
lpspi_master_config_t lpspi3_config; //LPSPI3�������ýṹ��
lpspi_master_config_t lpspi4_config; //LPSPI4�������ýṹ��

void LPSPI3_Init(uint8_t baudrate)
{
    uint32_t lpspiclk=0;
    uint32_t spi3_baudrate=baudrate*1000000;
    
    //����LPSPI��ʱ��ԴΪPLL3_PFD0/(1+5)=576Mhz/(1+5)=96MHz,W25Q256ʱ�����104MHz
    CLOCK_SetMux(kCLOCK_LpspiMux,1);    //USB1 PLL PFD0(576MHz)��ΪLPSPIʱ��Դ
    CLOCK_SetDiv(kCLOCK_LpspiDiv,5);    //��Ƶ���ã���ƵֵΪ5+1������LPSPI CLK=576/(1+5)=96Mhz
    
    //����IO��ΪLPSPI3��Ƭѡ��������ģ�����һ��SPI������Ͳ�������Ϊ4��!
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI,0);                                    
    
    //����LPSPI���Ź��ܣ�
    //��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz����·�����ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK,0x10B0);                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO,0x10B0);                                                                              
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI,0x10B0);                                
    
    //����LPSPI3��ģʽ
    lpspiclk=(CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk)/(5+1));//LPSPI3ʱ��
    lpspi3_config.baudRate=spi3_baudrate;               //SPI�ٶ�
    lpspi3_config.whichPcs=kLPSPI_Pcs1;                 //Ƭѡ�źţ�PCS1
    lpspi3_config.pcsActiveHighOrLow=kLPSPI_PcsActiveLow;//Ƭѡ�źŵ͵�ƽ��Ч
    lpspi3_config.bitsPerFrame=8;                       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    lpspi3_config.cpol=kLPSPI_ClockPolarityActiveLow;   //����ͬ��ʱ�ӵ͵�ƽ��Ч
    lpspi3_config.cpha=kLPSPI_ClockPhaseSecondEdge;      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    lpspi3_config.direction=kLPSPI_MsbFirst;            //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    lpspi3_config.pinCfg=kLPSPI_SdiInSdoOut;            //SDI�������ţ�SDO�������
    lpspi3_config.dataOutConfig=kLpspiDataOutRetained;  //������ݱ���
    lpspi3_config.pcsToSckDelayInNanoSec=10;            //Ƭѡ���͵�ʱ����Ч֮�����ʱʱ��,��λns
    lpspi3_config.lastSckToPcsDelayInNanoSec=10;        //���һ��ʱ�ӵ�Ƭѡ����֮�����ʱ����λns
    lpspi3_config.betweenTransferDelayInNanoSec=10;     //���δ���֮�����ʱ����λns
    LPSPI_MasterInit(LPSPI3,&lpspi3_config,lpspiclk);
    LPSPI_Enable(LPSPI3,true);                          //ʹ��LPSPI3
}

//LPSPI3 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t LPSPI3_ReadWriteByte(uint8_t TxData)
{ 
    uint8_t spirxdata=0;
    uint8_t spitxdata=TxData;
    lpspi_transfer_t spi_tranxfer;
    
    spi_tranxfer.configFlags=kLPSPI_MasterPcs1|kLPSPI_MasterPcsContinuous;     //PCS1
    spi_tranxfer.txData=&spitxdata;                 //Ҫ���͵�����
    spi_tranxfer.rxData=&spirxdata;                 //���յ�������
    spi_tranxfer.dataSize=1;                        //���ݳ���
    LPSPI_MasterTransferBlocking(LPSPI3,&spi_tranxfer);	    //SPI��������   					    
    return spirxdata;
}

/**
  * @brief    SPI ����ģʽ��ʼ��
  *
  * @param    base      ��  LPSPI1 - LPSPI4
  * @param    cs_num    ��  Ƭѡ���ű�� 
  * @param      mode            0: CLK����ʱ��Ϊ�͵�ƽ �ڵ�һ��CLK�����أ��½��أ���������
  * @param      mode            1: CLK����ʱ��Ϊ�͵�ƽ �ڵڶ���CLK�����أ������أ���������
  * @param      mode            2: CLK����ʱ��Ϊ�ߵ�ƽ �ڵ�һ��CLK�����أ������أ���������
  * @param      mode            3: CLK����ʱ��Ϊ�ߵ�ƽ �ڵڶ���CLK�����أ��½��أ���������
  * @param    baudrate  ��  SPI ������    
  *
  * @return   ע�� ICM20602 ʱ�ӿ���ʱΪ�ߵ�ƽ ʱ����ЧʱΪ�͵�ƽ ��ʱ�ӵڶ��������ش�������    
  *
  * @note     SPI Ĭ��ʱ��Դ 66M
  *
  * @example  
  *
  * @date     2019/6/3 ����һ
  */
void LQ_SPI_Master_Init(LPSPI_Type * base, uint8_t cs_num, uint8_t mode, uint32_t baudrate)
{
    SPI_PinInit(base, cs_num);
    
    lpspi_master_config_t masterConfig;
    
    /*Master config*/
    masterConfig.baudRate = baudrate;                  //���ò�����
    masterConfig.bitsPerFrame = 8;                     //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    if(mode / 2)
    {
        masterConfig.cpol = kLPSPI_ClockPolarityActiveLow;  //ʱ�ӿ���ʱΪ�ߵ�ƽ
    }
    else
    {
        masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh; //ʱ�ӿ���ʱΪ�͵�ƽ
    }
    
    if(mode % 2)
    {
        masterConfig.cpha = kLPSPI_ClockPhaseSecondEdge;  //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    }
    else
    {
        masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;  //����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����
    }
    
    masterConfig.direction = kLPSPI_MsbFirst;          //MSB��ǰ

    masterConfig.pcsToSckDelayInNanoSec = 10;       //Ƭѡ���͵���ʼ�����ʱ��
    masterConfig.lastSckToPcsDelayInNanoSec = 10;   //���һ���źŵ�Ƭѡ����ʱ��
    masterConfig.betweenTransferDelayInNanoSec = 10;//���δ���֮���ʱ��

    masterConfig.whichPcs = (lpspi_which_pcs_t)cs_num;     //Ƭѡ����
    masterConfig.pcsActiveHighOrLow = kLPSPI_PcsActiveLow; //Ƭѡ�źŵ͵�ƽ��Ч

    masterConfig.pinCfg = kLPSPI_SdiInSdoOut;              //SDI�������ţ�SDO�������
    masterConfig.dataOutConfig = kLpspiDataOutRetained;    //�����������

    LPSPI_MasterInit(base, &masterConfig, 66000000UL);

}


/**
  * @brief    SPI��дN���ֽ�
  *
  * @param    base    ��  LPSPI1 - LPSPI4
  * @param    cs_num  ��  Ƭѡ���ű��  
  * @param    txData  ��  д���buff
  * @param    rxData  ��  ������buff
  * @param    len     ��  ����
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/3 ����һ
  */
uint8_t SPI_ReadWriteNByte(LPSPI_Type * base, uint8_t cs_num, uint8_t *txData, uint8_t *rxData, uint32_t len)
{

    lpspi_transfer_t spi_tranxfer;
    spi_tranxfer.txData = txData;   //��������
    spi_tranxfer.rxData = rxData;   //��������
    spi_tranxfer.dataSize = len;    //����
    spi_tranxfer.configFlags = cs_num|kLPSPI_MasterPcsContinuous;     //Ƭѡ 
    
    return LPSPI_MasterTransferBlocking(base, &spi_tranxfer);        //spi��������;
}



/**
  * @brief    SPI �ܽų�ʼ��
  *
  * @param    base      ��  LPSPI1 - LPSPI4
  * @param    cs_num    ��  Ƭѡ���ű��  
  *
  * @return   
  *
  * @note     ��4�¹ܽŸ���
  *
  * @example  
  *
  * @date     2019/6/3 ����һ
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


