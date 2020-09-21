#ifndef __LQ_FLASH_H
#define __LQ_FLASH_H

#include "include.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//W25Q256 ID  0XEF18
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern uint16_t W25QXX_TYPE;					//定义W25QXX芯片型号		   

#define	W25QXX_CS(n)    (n?GPIO_PinWrite(GPIO1,4,1):GPIO_PinWrite(GPIO1,4,0)) 		//W25QXX的片选信号

////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

void W25QXX_Init(void);
uint16_t  W25QXX_ReadID(void);  	    		//读取FLASH ID
uint8_t W25QXX_ReadSR(uint8_t regno);             //读取状态寄存器 
void W25QXX_4ByteAddr_Enable(void);     //使能4字节地址模式
void W25QXX_Write_SR(uint8_t regno,uint8_t sr);   //写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(uint32_t Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒
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
int FLASH_EraseSector(uint16_t sector_num);

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
int FLASH_EraseAll(void);

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
void FLASH_ReadBuff(uint16_t sector_num, uint16_t page, uint16_t offset, uint8_t *buff, uint32_t len);



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
int FLASH_WriteBuff(uint16_t sector_num, uint16_t page, uint16_t offset, uint8_t *buff, uint32_t len);



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
int LQ_FLASH_Init(void);

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
int Test_Flash(void);




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
void  FLASH_PinInit(void);



#endif
