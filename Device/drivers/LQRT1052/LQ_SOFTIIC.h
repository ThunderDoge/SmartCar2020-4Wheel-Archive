#ifndef __IIC_H
#define __IIC_H

#include "LQ_GPIO_Cfg.h"
#include "fsl_common.h"

/* 注意 IIC总线规定，IIC空闲时 SCL和SDA都为高电平 最好外部上拉（一定不能下拉） */
/* 模拟 IIC需要注意，地址需要硬件IIC地址左移一位 例如MPU6050 硬件IIC地址用 0x68  用模拟就是0xD0*/
/* C12 GPIO2_22  作为 SCL */
/* A13 GPIO2_25  作为 SDA */
#define IIC_SCL_PIN  DIR2_22  //模拟IIC的SCL信号  1.修改引脚即可修改IIC接口
#define IIC_SDA_PIN  DIR2_25   //模拟IIC的SDA信号

#define SDA_IN()  IIC_SDA_PIN = 0;	//输入
#define SDA_OUT() IIC_SDA_PIN = 1;	//输出

#define IIC_SCL    PT2_22       //SCL              2.修改引脚即可修改IIC接口    
#define IIC_SDA    PT2_25       //SDA	 
#define READ_SDA   PT2_25       //输入SDA 


/*---------------------------------------------------------------
            IIC内部函数
----------------------------------------------------------------*/		 
void IIC_Start(void);			        //发送IIC开始信号
void IIC_Stop(void);	  	            //发送IIC停止信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				    //IIC不发送ACK信号
uint8_t IIC_WaitAck(void); 		        //IIC等待ACK信号
void IIC_SendByte(uint8_t data);        //IIC发送一个字节
uint8_t IIC_ReadByte(uint8_t ack);       //IIC读取一个字节


/*---------------------------------------------------------------
            IIC用户函数
----------------------------------------------------------------*/
void IIC_Init(void);                    //初始化IIC的IO口   
uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf);
uint8_t IIC_ReadMultByteFromSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t IIC_WriteByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t buf);
uint8_t IIC_WriteMultByteToSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);



#endif
