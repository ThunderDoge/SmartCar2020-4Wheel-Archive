/*---------------------------------------------
��ƽ    ̨������i.MX RT1064���İ�-���ܳ���
����    д��LQ-005
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2019��3��12��
��dev.env.��IAR8.30.1�����ϰ汾
��Target �� i.MX RT1064
��Crystal�� 24.000Mhz
��ARM PLL�� 1200MHz
��SYS PLL�� 600MHz
��USB PLL�� 480MHz
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
---------------------------------------------------------*/
#include "fsl_flexspi.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "LQ_W25Q128.h"
#include "LQ_SPI.h"
#include "LQ_FLASH.h"
#include "include.h"
#include "main.h"
uint16_t W25QXX_TYPE=W25Q256;	//Ĭ����W25Q256

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector 
													 
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
    uint8_t temp;
    gpio_pin_config_t spics_config;
    
    //Ƭѡ��Ϊ��ͨIO�ڣ��������Ƭѡ������һ��SPI�ھͲ�ֹ4������
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,0x10B0); 
    
    spics_config.direction=kGPIO_DigitalOutput;	//���
	spics_config.interruptMode=kGPIO_NoIntmode;	//��ʹ���жϹ���
	spics_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO1,4,&spics_config); 	    //��ʼ��GPIO1_4
    
    W25QXX_CS(1);                           //SPI FLASH��ѡ��
    LPSPI3_Init(20);                        //SPI������96M
    W25QXX_TYPE=W25QXX_ReadID();	        //��ȡFLASH ID.
    if(W25QXX_TYPE==W25Q256)                //SPI FLASHΪW25Q256
    {
        temp=W25QXX_ReadSR(3);              //��ȡ״̬�Ĵ���3���жϵ�ַģʽ
        if((temp&0X01)==0)			        //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
		{
            W25QXX_CS(0); 			        //ѡ��
			LPSPI3_ReadWriteByte(W25X_Enable4ByteAddr);//���ͽ���4�ֽڵ�ַģʽָ�� 
            W25QXX_CS(1); 			        //ȡ��Ƭѡ             
		}
    }
}  

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
uint8_t W25QXX_ReadSR(uint8_t regno)   
{  
	uint8_t byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }                                       
    W25QXX_CS(0); 			                    //ѡ��
	LPSPI3_ReadWriteByte(command);              //���Ͷ�ȡ״̬�Ĵ�������    
	byte=LPSPI3_ReadWriteByte(0Xff);            //��ȡһ���ֽ�  
    W25QXX_CS(1); 			                    //ȡ��ѡ��
	return byte;   
} 

//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
    uint8_t command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }     
    W25QXX_CS(0);                               //ʹ������ 
	LPSPI3_ReadWriteByte(command);              //����дȡ״̬�Ĵ�������    
	LPSPI3_ReadWriteByte(sr);                   //д��һ���ֽ�   
	W25QXX_CS(1);                               //ȡ��Ƭѡ      
}  

//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{ 
    W25QXX_CS(0);                               //ʹ������  
    LPSPI3_ReadWriteByte(W25X_WriteEnable);     //����дʹ��  
    W25QXX_CS(1);                               //ȡ��Ƭѡ     
} 

//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{ 
    W25QXX_CS(0);                               //ʹ������       
    LPSPI3_ReadWriteByte(W25X_WriteDisable);    //����д��ָֹ�� 
    W25QXX_CS(1);                               //ʹ������       
} 

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
    
    W25QXX_CS(0);
	LPSPI3_ReadWriteByte(0x90);//���Ͷ�ȡID����	
	LPSPI3_ReadWriteByte(0x00); 
	LPSPI3_ReadWriteByte(0x00); 
	LPSPI3_ReadWriteByte(0x00);
	Temp|=LPSPI3_ReadWriteByte(0xFF)<<8;  
	Temp|=LPSPI3_ReadWriteByte(0xFF);
    W25QXX_CS(1);
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   
    W25QXX_CS(0);                               //ʹ������    
    LPSPI3_ReadWriteByte(W25X_ReadData);        //���Ͷ�ȡ����  
    if(W25QXX_TYPE==W25Q256)                    //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>24));    
    }
    LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //����24bit��ַ    
    LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=LPSPI3_ReadWriteByte(0XFF);    //ѭ������  
    }	
    W25QXX_CS(1);     
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                      //SET WEL 
    W25QXX_CS(0);                               //ʹ������      
    LPSPI3_ReadWriteByte(W25X_PageProgram);     //����дҳ����   
    if(W25QXX_TYPE==W25Q256)                    //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>24)); 
    }
    LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)LPSPI3_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	W25QXX_CS(1);                            //ȡ��Ƭѡ 
    W25QXX_Wait_Busy();					    //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                      //SET WEL 
    W25QXX_Wait_Busy();  
    W25QXX_CS(0);                               //ʹ������      
    LPSPI3_ReadWriteByte(W25X_ChipErase);       //����Ƭ�������� 
    W25QXX_CS(1);                               //ȡ��Ƭѡ     
	W25QXX_Wait_Busy();   				        //�ȴ�оƬ��������
}  

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//����falsh�������,������   
 	//printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                      //SET WEL 	 
    W25QXX_Wait_Busy();  
    W25QXX_CS(0);                               //ʹ������     
    LPSPI3_ReadWriteByte(W25X_SectorErase);     //������������ָ�� 
    if(W25QXX_TYPE==W25Q256)                    //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>24)); 
    }
    LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)Dst_Addr);     	
    W25QXX_CS(1);                               //ȡ��Ƭѡ    
    W25QXX_Wait_Busy();   				        //�ȴ��������
}  

//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(1)&0x01)==0x01);       // �ȴ�BUSYλ���
} 

//�������ģʽ
void W25QXX_PowerDown(void)   
{  
    W25QXX_CS(0);                               //ʹ������  
    LPSPI3_ReadWriteByte(W25X_PowerDown);       //���͵�������   
    W25QXX_CS(1);                               //ȡ��Ƭѡ    
    delayus(3);                                //�ȴ�TPD  
}   

//����
void W25QXX_WAKEUP(void)   
{ 
    W25QXX_CS(0);                                   //ʹ������     
    LPSPI3_ReadWriteByte(W25X_ReleasePowerDown);    // send W25X_PowerDown command 0xAB        	      
    W25QXX_CS(1);                                   //ȡ��Ƭѡ
    delayus(3);                                    //�ȴ�TRES1
}   

/**
  * @brief    flash �ܽų�ʼ�� 
  *
  * @param    
  *
  * @return   
  *
  * @note     �ڲ����� 
  *
  * @example  
  *
  * @date     2019/6/19 ������
  */
void  FLASH_PinInit(void)
{
	CLOCK_EnableClock(kCLOCK_Iomuxc); 
	
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_05_FLEXSPIA_DQS, 1U);   
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,1U);                                 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,1U);                                  
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,1U);                                
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,1U);                                
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,1U);                                
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,1U);                                
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_05_FLEXSPIA_DQS,0x10F1u); 
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,0x10F1u);                               
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,0x10F1u);                               
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00,0x10F1u);                               
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01,0x10F1u);                               
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02,0x10F1u);                               
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03,0x10F1u);                               
    
    
}

/**
  * @brief    ��������
  *
  * @param    sector_num �� �������  0 - 4095��16MB FLASH  һ������4K һ��4096��������
  *
  * @return   0:�����ɹ�    ����ʧ��
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 ������
  */
int FLASH_EraseSector(uint16_t sector_num)
{
    return flexspi_nor_flash_erase_sector(FLEXSPI, sector_num * SECTOR_SIZE);
}


/**
  * @brief    ��������flash
  *
  * @param    
  *
  * @return   
  *
  * @note     ʱ��ܳ�
  *
  * @example  
  *
  * @date     2019/6/20 ������
  */
int FLASH_EraseAll(void)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(FLEXSPI, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = 0;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASECHIP;

    status = FLEXSPI_TransferBlocking(FLEXSPI, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(FLEXSPI);

    return status;

}



/**
  * @brief    ��ȡ����
  *
  * @param    sector_num �� �������      0 - 4095��16MB FLASH  һ������4K һ��4096�������� 
  * @param    page       �� ������ҳ���  0 - 15  ��һ������4K  һҳ256�ֽ� һ��16ҳ�� 
  * @param    offset     �� ҳ�еڼ����ֽ�  0 - 255��1һҳ256���ֽڣ� 
  * @param    buff       �� �������ݴ�ŵ�ַ 
  * @param    len        �� ��������
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 ������
  */
void FLASH_ReadBuff(uint16_t sector_num, uint16_t page, uint16_t offset, uint8_t *buff, uint32_t len)
{
    uint8_t *addr = (uint8_t *)((0x60000000U)+ sector_num * SECTOR_SIZE + page * FLASH_PAGE_SIZE + offset);
    for(int i = 0; i < len; i++)
    {
        *(buff++) = *(addr++);
    
    }
}


/**
  * @brief    д����
  *
  * @param    sector_num �� �������      0 - 4095��16MB FLASH  һ������4K һ��4096�������� 
  * @param    page       �� ������ҳ���  0 - 15  ��һ������4K  һҳ256�ֽ� һ��16ҳ�� 
  * @param    offset     �� ҳ�еڼ����ֽ�  0 - 255��1һҳ256���ֽڣ� 
  * @param    buff       �� д�����ݴ�ŵ�ַ 
  * @param    len        �� д�볤��
  *
  * @return   
  *
  * @note     д֮ǰҪ����  
  *           flash Ϊʲôд֮ǰҪ��������Ϊflashֻ��д0, ������ʱ��flash���0xff �Ϳ���д0�ı�������
  * @example  
  *
  * @date     2019/6/20 ������
  */
int FLASH_WriteBuff(uint16_t sector_num, uint16_t page, uint16_t offset, uint8_t *buff, uint32_t len)
{
    int temp = 1;
    uint32_t len_temp;
    uint8_t *head;
    /* �����ҳд���� */
    if(offset + len > 255)
    {
        temp = (offset + len) / 256 + 1;
        len_temp = 256 - offset;
    }
    else
    {
        temp = 1;
        len_temp = len;
    }
        
    uint32_t dstAddr = (sector_num * SECTOR_SIZE + page * FLASH_PAGE_SIZE + offset);
    status_t status;
    flexspi_transfer_t flashXfer;

    do{
        /* Write neable */
        status = flexspi_nor_write_enable(FLEXSPI, dstAddr);
        
        if (status != kStatus_Success)
        {
            return status;
        }
        
        /* Prepare page program command */
        flashXfer.deviceAddress = dstAddr;
        flashXfer.port = kFLEXSPI_PortA1;
        flashXfer.cmdType = kFLEXSPI_Write;
        flashXfer.SeqNumber = 1;
        flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
        flashXfer.data = (uint32_t *)buff;
        flashXfer.dataSize = len_temp;
        status = FLEXSPI_TransferBlocking(FLEXSPI, &flashXfer);
        
        if (status != kStatus_Success)
        {
            return status;
        }
        
        status = flexspi_nor_wait_bus_busy(FLEXSPI);
        dstAddr += len_temp;
        head = (uint8_t *)buff;
        head += len_temp;
        buff = head;
        len = len - len_temp;
        if(len > 255)
        {
            len_temp = 256;
        }
        else
        {
            len_temp = len;
        }
        
    }while(--temp);
    /* �������AHB������ */
    FLEXSPI_SoftwareReset(FLEXSPI);
    
    return status;
}





/**
  * @brief    16M  �ⲿFLASH��ʼ��
  *
  * @param    
  *
  * @return   0����ʼ��OK   ��������
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/19 ������
  */
int LQ_FLASH_Init(void)
{
    status_t status;
    FLASH_PinInit();
    
    /* ����ʱ�� */
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0x2); /* Choose PLL2 PFD2 clock as flexspi source clock. 396M */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 2);   /* flexspi clock 133M. */
    
    flexspi_config_t config;
    

    FLEXSPI_GetDefaultConfig(&config);

    /*����ͨ��AHB���߶�ȡ���ݵ�AHB��������С. */
    config.ahbConfig.enableAHBPrefetch = true;
    config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
    FLEXSPI_Init(FLEXSPI, &config);

    /* ����FLASH. */
    FLEXSPI_SetFlashConfig(FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);

    /* ���� LUT ��. */
    FLEXSPI_UpdateLUT(FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);

    /* ��λSPI. */
    FLEXSPI_SoftwareReset(FLEXSPI);
    
    uint8_t vendorID = 0;
    
    /* ��ȡID. */
    status = flexspi_nor_get_vendor_id(FLEXSPI, &vendorID);
    if (status != kStatus_Success)
    {
        return status;
    }
    if(vendorID != 0x17)
    {   
        PRINTF("\r\nû�ж���FLASH ID \r\n");
        PRINTF("Vendor ID: 0x%x\r\n", vendorID);
        return 1;
    }
    /* �������ģʽ. */
    status = flexspi_nor_enable_quad_mode(FLEXSPI);
    if (status != kStatus_Success)
    {
        return status;
    }
    
    
    return 0;
}

/* �����õ�����*/
#define EXAMPLE_SECTOR 4000

/**
  * @brief    flash ��д����
  *
  * @param    
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 ������
  */
int Test_Flash(void)
{
    uint8_t s_nor_program_buffer[4096];
    uint8_t s_nor_read_buffer[4096];
    
    uint32_t i = 0;
    status_t status;

    PRINTF("\r\n�ⲿflash ��д���� !\r\n");
    
    /*ʹ���ⲿflashʱ����ð�D-Cache ���ж� �ص�*/
    DisableInterrupts;
    SCB_DisableDCache();

    /* ��ʼ�� */
    LQ_FLASH_Init();
    /* �������� */
    PRINTF("����һ������...\r\n");
    status = FLASH_EraseSector(EXAMPLE_SECTOR);
    if (status != kStatus_Success)
    {
        PRINTF("����ʧ�� !\r\n");
        return -1;
    }

    /* ����Ƿ������� */
    /* ��s_nor_program_buffer ��Ϊ 0xFF�� �����������Ӧ�ö��� 0xff */
    memset(s_nor_program_buffer, 0xFFU, sizeof(s_nor_program_buffer));
    
    /* �������� */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, sizeof(s_nor_program_buffer)))
    {
        PRINTF("����ʧ�� �����ݲ���0xff !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("�����ɹ�. \r\n");
    }

    
    /* д�����ݳ�ʼ�� */
    for (i = 0; i < sizeof(s_nor_program_buffer); i++)
    {
        s_nor_program_buffer[i] = i;
    }

    PRINTF("д��flash... !\r\n");
    /* д������ */
    status =
        FLASH_WriteBuff(EXAMPLE_SECTOR, 0, 0, s_nor_program_buffer, sizeof(s_nor_program_buffer));
    if (status != kStatus_Success)
    {
        PRINTF("д��flash ʧ�� !\r\n");
        return -1;
    }
    PRINTF("д��flash�ɹ� !\r\n");
    PRINTF("��ȡflash����... !\r\n");
    /* �������� */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_read_buffer, s_nor_program_buffer, sizeof(s_nor_program_buffer)) != 0)
    {
        PRINTF("flash ����������д�Ĳ�һ�� !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("flash ��д���Գɹ�. \r\n");
    }

    
    /* ��д���������� */
    PRINTF("flash ��д�����������. \r\n");
    float f_data_rade[100], f_data_write[100];
    
    /* �������� */
    PRINTF("����һ������...\r\n");
    status = FLASH_EraseSector(EXAMPLE_SECTOR);
    if (status != kStatus_Success)
    {
        PRINTF("����ʧ�� !\r\n");
        return -1;
    }

    /* ����Ƿ������� */
    /* ��s_nor_program_buffer ��Ϊ 0xFF�� �����������Ӧ�ö��� 0xff */
    memset(s_nor_program_buffer, 0xFFU, sizeof(s_nor_program_buffer));
    
    /* �������� */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, sizeof(s_nor_program_buffer)))
    {
        PRINTF("����ʧ�� �����ݲ���0xff !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("�����ɹ�. \r\n");
    }

    
    /* д�����ݳ�ʼ�� */
    for (i = 0; i < sizeof(f_data_write); i++)
    {
        f_data_write[i] = i * 1.25;
    }

    PRINTF("д��flash... !\r\n");
    /* д������ */
    status =
        FLASH_WriteBuff(EXAMPLE_SECTOR, 0, 0, (uint8_t *)f_data_write, sizeof(f_data_write));
    if (status != kStatus_Success)
    {
        PRINTF("д��flash ʧ�� !\r\n");
        return -1;
    }
    PRINTF("д��flash�ɹ� !\r\n");
    PRINTF("��ȡflash����... !\r\n");
    /* �������� */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, (uint8_t *)f_data_rade, sizeof(f_data_write));

    if (memcmp(f_data_rade, f_data_write, sizeof(f_data_write)) != 0)
    {
        PRINTF("flash ����������д�Ĳ�һ�� !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("flash ��������д���Գɹ�. \r\n");
    }
    
    while (1)
    {
    }
}
