/*---------------------------------------------
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月12日
【dev.env.】IAR8.30.1及以上版本
【Target 】 i.MX RT1064
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 600MHz
【USB PLL】 480MHz
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
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
uint16_t W25QXX_TYPE=W25Q256;	//默认是W25Q256

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q256
//容量为32M字节,共有512个Block,8192个Sector 
													 
//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{ 
    uint8_t temp;
    gpio_pin_config_t spics_config;
    
    //片选作为普通IO口，软件控制片选，这样一个SPI口就不止4个外设
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,0x10B0); 
    
    spics_config.direction=kGPIO_DigitalOutput;	//输出
	spics_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	spics_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO1,4,&spics_config); 	    //初始化GPIO1_4
    
    W25QXX_CS(1);                           //SPI FLASH不选中
    LPSPI3_Init(20);                        //SPI波特率96M
    W25QXX_TYPE=W25QXX_ReadID();	        //读取FLASH ID.
    if(W25QXX_TYPE==W25Q256)                //SPI FLASH为W25Q256
    {
        temp=W25QXX_ReadSR(3);              //读取状态寄存器3，判断地址模式
        if((temp&0X01)==0)			        //如果不是4字节地址模式,则进入4字节地址模式
		{
            W25QXX_CS(0); 			        //选中
			LPSPI3_ReadWriteByte(W25X_Enable4ByteAddr);//发送进入4字节地址模式指令 
            W25QXX_CS(1); 			        //取消片选             
		}
    }
}  

//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
uint8_t W25QXX_ReadSR(uint8_t regno)   
{  
	uint8_t byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }                                       
    W25QXX_CS(0); 			                    //选中
	LPSPI3_ReadWriteByte(command);              //发送读取状态寄存器命令    
	byte=LPSPI3_ReadWriteByte(0Xff);            //读取一个字节  
    W25QXX_CS(1); 			                    //取消选中
	return byte;   
} 

//写W25QXX状态寄存器
void W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
    uint8_t command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }     
    W25QXX_CS(0);                               //使能器件 
	LPSPI3_ReadWriteByte(command);              //发送写取状态寄存器命令    
	LPSPI3_ReadWriteByte(sr);                   //写入一个字节   
	W25QXX_CS(1);                               //取消片选      
}  

//W25QXX写使能	
//将WEL置位   
void W25QXX_Write_Enable(void)   
{ 
    W25QXX_CS(0);                               //使能器件  
    LPSPI3_ReadWriteByte(W25X_WriteEnable);     //发送写使能  
    W25QXX_CS(1);                               //取消片选     
} 

//W25QXX写禁止	
//将WEL清零  
void W25QXX_Write_Disable(void)   
{ 
    W25QXX_CS(0);                               //使能器件       
    LPSPI3_ReadWriteByte(W25X_WriteDisable);    //发送写禁止指令 
    W25QXX_CS(1);                               //使能器件       
} 

//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
//0XEF18,表示芯片型号为W25Q256
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
    
    W25QXX_CS(0);
	LPSPI3_ReadWriteByte(0x90);//发送读取ID命令	
	LPSPI3_ReadWriteByte(0x00); 
	LPSPI3_ReadWriteByte(0x00); 
	LPSPI3_ReadWriteByte(0x00);
	Temp|=LPSPI3_ReadWriteByte(0xFF)<<8;  
	Temp|=LPSPI3_ReadWriteByte(0xFF);
    W25QXX_CS(1);
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   
    W25QXX_CS(0);                               //使能器件    
    LPSPI3_ReadWriteByte(W25X_ReadData);        //发送读取命令  
    if(W25QXX_TYPE==W25Q256)                    //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>24));    
    }
    LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //发送24bit地址    
    LPSPI3_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=LPSPI3_ReadWriteByte(0XFF);    //循环读数  
    }	
    W25QXX_CS(1);     
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                      //SET WEL 
    W25QXX_CS(0);                               //使能器件      
    LPSPI3_ReadWriteByte(W25X_PageProgram);     //发送写页命令   
    if(W25QXX_TYPE==W25Q256)                    //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>24)); 
    }
    LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    LPSPI3_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)LPSPI3_ReadWriteByte(pBuffer[i]);//循环写数  
	W25QXX_CS(1);                            //取消片选 
    W25QXX_Wait_Busy();					    //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}
//擦除整个芯片		  
//等待时间超长...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                      //SET WEL 
    W25QXX_Wait_Busy();  
    W25QXX_CS(0);                               //使能器件      
    LPSPI3_ReadWriteByte(W25X_ChipErase);       //发送片擦除命令 
    W25QXX_CS(1);                               //取消片选     
	W25QXX_Wait_Busy();   				        //等待芯片擦除结束
}  

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 	//printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                      //SET WEL 	 
    W25QXX_Wait_Busy();  
    W25QXX_CS(0);                               //使能器件     
    LPSPI3_ReadWriteByte(W25X_SectorErase);     //发送扇区擦除指令 
    if(W25QXX_TYPE==W25Q256)                    //如果是W25Q256的话地址为4字节的，要发送最高8位
    {
        LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>24)); 
    }
    LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    LPSPI3_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    LPSPI3_ReadWriteByte((uint8_t)Dst_Addr);     	
    W25QXX_CS(1);                               //取消片选    
    W25QXX_Wait_Busy();   				        //等待擦除完成
}  

//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(1)&0x01)==0x01);       // 等待BUSY位清空
} 

//进入掉电模式
void W25QXX_PowerDown(void)   
{  
    W25QXX_CS(0);                               //使能器件  
    LPSPI3_ReadWriteByte(W25X_PowerDown);       //发送掉电命令   
    W25QXX_CS(1);                               //取消片选    
    delayus(3);                                //等待TPD  
}   

//唤醒
void W25QXX_WAKEUP(void)   
{ 
    W25QXX_CS(0);                                   //使能器件     
    LPSPI3_ReadWriteByte(W25X_ReleasePowerDown);    // send W25X_PowerDown command 0xAB        	      
    W25QXX_CS(1);                                   //取消片选
    delayus(3);                                    //等待TRES1
}   

/**
  * @brief    flash 管脚初始化 
  *
  * @param    
  *
  * @return   
  *
  * @note     内部调用 
  *
  * @example  
  *
  * @date     2019/6/19 星期三
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
  * @brief    擦出扇区
  *
  * @param    sector_num ： 扇区编号  0 - 4095（16MB FLASH  一个扇区4K 一共4096个扇区）
  *
  * @return   0:擦出成功    其他失败
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 星期四
  */
int FLASH_EraseSector(uint16_t sector_num)
{
    return flexspi_nor_flash_erase_sector(FLEXSPI, sector_num * SECTOR_SIZE);
}


/**
  * @brief    擦出整块flash
  *
  * @param    
  *
  * @return   
  *
  * @note     时间很长
  *
  * @example  
  *
  * @date     2019/6/20 星期四
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
  * @brief    读取数据
  *
  * @param    sector_num ： 扇区编号      0 - 4095（16MB FLASH  一个扇区4K 一共4096个扇区） 
  * @param    page       ： 扇区内页编号  0 - 15  （一个扇区4K  一页256字节 一共16页） 
  * @param    offset     ： 页中第几个字节  0 - 255（1一页256个字节） 
  * @param    buff       ： 读出数据存放地址 
  * @param    len        ： 读出长度
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 星期四
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
  * @brief    写数据
  *
  * @param    sector_num ： 扇区编号      0 - 4095（16MB FLASH  一个扇区4K 一共4096个扇区） 
  * @param    page       ： 扇区内页编号  0 - 15  （一个扇区4K  一页256字节 一共16页） 
  * @param    offset     ： 页中第几个字节  0 - 255（1一页256个字节） 
  * @param    buff       ： 写入数据存放地址 
  * @param    len        ： 写入长度
  *
  * @return   
  *
  * @note     写之前要擦除  
  *           flash 为什么写之前要擦除，因为flash只能写0, 擦除的时候flash变成0xff 就可以写0改变数据了
  * @example  
  *
  * @date     2019/6/20 星期四
  */
int FLASH_WriteBuff(uint16_t sector_num, uint16_t page, uint16_t offset, uint8_t *buff, uint32_t len)
{
    int temp = 1;
    uint32_t len_temp;
    uint8_t *head;
    /* 如果本页写不完 */
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
    /* 软件重置AHB缓冲区 */
    FLEXSPI_SoftwareReset(FLEXSPI);
    
    return status;
}





/**
  * @brief    16M  外部FLASH初始化
  *
  * @param    
  *
  * @return   0：初始化OK   其他错误
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/19 星期三
  */
int LQ_FLASH_Init(void)
{
    status_t status;
    FLASH_PinInit();
    
    /* 配置时钟 */
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0x2); /* Choose PLL2 PFD2 clock as flexspi source clock. 396M */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 2);   /* flexspi clock 133M. */
    
    flexspi_config_t config;
    

    FLEXSPI_GetDefaultConfig(&config);

    /*设置通过AHB总线读取数据的AHB缓冲区大小. */
    config.ahbConfig.enableAHBPrefetch = true;
    config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
    FLEXSPI_Init(FLEXSPI, &config);

    /* 配置FLASH. */
    FLEXSPI_SetFlashConfig(FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);

    /* 更新 LUT 表. */
    FLEXSPI_UpdateLUT(FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);

    /* 复位SPI. */
    FLEXSPI_SoftwareReset(FLEXSPI);
    
    uint8_t vendorID = 0;
    
    /* 获取ID. */
    status = flexspi_nor_get_vendor_id(FLEXSPI, &vendorID);
    if (status != kStatus_Success)
    {
        return status;
    }
    if(vendorID != 0x17)
    {   
        PRINTF("\r\n没有读到FLASH ID \r\n");
        PRINTF("Vendor ID: 0x%x\r\n", vendorID);
        return 1;
    }
    /* 进入快速模式. */
    status = flexspi_nor_enable_quad_mode(FLEXSPI);
    if (status != kStatus_Success)
    {
        return status;
    }
    
    
    return 0;
}

/* 例子用的扇区*/
#define EXAMPLE_SECTOR 4000

/**
  * @brief    flash 读写测试
  *
  * @param    
  *
  * @return   
  *
  * @note     
  *
  * @example  
  *
  * @date     2019/6/20 星期四
  */
int Test_Flash(void)
{
    uint8_t s_nor_program_buffer[4096];
    uint8_t s_nor_read_buffer[4096];
    
    uint32_t i = 0;
    status_t status;

    PRINTF("\r\n外部flash 读写测试 !\r\n");
    
    /*使用外部flash时，最好把D-Cache 和中断 关掉*/
    DisableInterrupts;
    SCB_DisableDCache();

    /* 初始化 */
    LQ_FLASH_Init();
    /* 擦出扇区 */
    PRINTF("擦除一个扇区...\r\n");
    status = FLASH_EraseSector(EXAMPLE_SECTOR);
    if (status != kStatus_Success)
    {
        PRINTF("擦除失败 !\r\n");
        return -1;
    }

    /* 检查是否擦除完成 */
    /* 将s_nor_program_buffer 设为 0xFF， 擦除后的扇区应该都是 0xff */
    memset(s_nor_program_buffer, 0xFFU, sizeof(s_nor_program_buffer));
    
    /* 读出数据 */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, sizeof(s_nor_program_buffer)))
    {
        PRINTF("擦除失败 有数据不是0xff !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("擦除成功. \r\n");
    }

    
    /* 写入数据初始化 */
    for (i = 0; i < sizeof(s_nor_program_buffer); i++)
    {
        s_nor_program_buffer[i] = i;
    }

    PRINTF("写入flash... !\r\n");
    /* 写入数据 */
    status =
        FLASH_WriteBuff(EXAMPLE_SECTOR, 0, 0, s_nor_program_buffer, sizeof(s_nor_program_buffer));
    if (status != kStatus_Success)
    {
        PRINTF("写入flash 失败 !\r\n");
        return -1;
    }
    PRINTF("写入flash成功 !\r\n");
    PRINTF("读取flash数据... !\r\n");
    /* 读出数据 */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_read_buffer, s_nor_program_buffer, sizeof(s_nor_program_buffer)) != 0)
    {
        PRINTF("flash 读出数据与写的不一致 !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("flash 读写测试成功. \r\n");
    }

    
    /* 读写浮点数测试 */
    PRINTF("flash 读写浮点数组测试. \r\n");
    float f_data_rade[100], f_data_write[100];
    
    /* 擦出扇区 */
    PRINTF("擦除一个扇区...\r\n");
    status = FLASH_EraseSector(EXAMPLE_SECTOR);
    if (status != kStatus_Success)
    {
        PRINTF("擦除失败 !\r\n");
        return -1;
    }

    /* 检查是否擦除完成 */
    /* 将s_nor_program_buffer 设为 0xFF， 擦除后的扇区应该都是 0xff */
    memset(s_nor_program_buffer, 0xFFU, sizeof(s_nor_program_buffer));
    
    /* 读出数据 */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, s_nor_read_buffer, sizeof(s_nor_program_buffer));

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, sizeof(s_nor_program_buffer)))
    {
        PRINTF("擦除失败 有数据不是0xff !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("擦除成功. \r\n");
    }

    
    /* 写入数据初始化 */
    for (i = 0; i < sizeof(f_data_write); i++)
    {
        f_data_write[i] = i * 1.25;
    }

    PRINTF("写入flash... !\r\n");
    /* 写入数据 */
    status =
        FLASH_WriteBuff(EXAMPLE_SECTOR, 0, 0, (uint8_t *)f_data_write, sizeof(f_data_write));
    if (status != kStatus_Success)
    {
        PRINTF("写入flash 失败 !\r\n");
        return -1;
    }
    PRINTF("写入flash成功 !\r\n");
    PRINTF("读取flash数据... !\r\n");
    /* 读出数据 */
    FLASH_ReadBuff(EXAMPLE_SECTOR, 0, 0, (uint8_t *)f_data_rade, sizeof(f_data_write));

    if (memcmp(f_data_rade, f_data_write, sizeof(f_data_write)) != 0)
    {
        PRINTF("flash 读出数据与写的不一致 !\r\n ");
        return -1;
    }
    else
    {
        PRINTF("flash 浮点数读写测试成功. \r\n");
    }
    
    while (1)
    {
    }
}
