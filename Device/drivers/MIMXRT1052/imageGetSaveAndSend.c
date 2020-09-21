/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】szx
【版    本】V1.0
【注    意】本文件包括一个函数：读取图像像素信息（rgb565），后期视需要更改为YUV灰度模式 
【最后更新】2020年1月13日
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#include "imageGetSaveAndSend.h"

#include "include.h"
#include "sys.h"
//#include "csi.h"
//#include "sdram.h"
//#include "LQ_OV7725M.h"
//#include "oled.h"
//#include "delay.h"
//
//#include "malloc.h"
//#include "usdhc1_sdcard.h"
//#include "ff.h"
//#include "exfuns.h"
//#include "uiControl.h"

#include "szx_image.h"
//#include "speedProcess.h"
//#include "LQ_PWM.h"
void Get_Use_Image(void);
void getImageRGB(void) {
	
	uint16_t rgb565Data;
	for (int i = 0; i < IMG_RGB_H; i++) {
		for (int j = 0; j < IMG_RGB_W; j++) {
			//rgb565Data = csiFrameBuf[count][(IMG_RGB_H - 1 - i) * 2][(IMG_RGB_W - 1 - j) * 2];
			rgb565Data = Image_Use[i][j].rgb565;
			//以下全忽略低3位(0-31)
			//R5 读取16位的高五位
			imageRGB[i][j][0] = (uint8_t)(rgb565Data >> 11); //32位右移11位，高位补0
			//G6取G5 读取中间6位的高5位
			imageRGB[i][j][1] = (uint8_t)((rgb565Data >> 6) & 0x1f);
			//B5读取低五位
			imageRGB[i][j][2] = (uint8_t)((rgb565Data) & 0x1f);
		}
	}

	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, fullCameraBufferAddr);//将照相机缓冲区提交到缓冲队列
}

//
//void sendImageRGB(void) {
//	LQ_UART_PutChar(LPUART4, 0x01);  //帧头
//	LQ_UART_PutChar(LPUART4, 0xfe);  //帧头
//
//	for (int i = 0; i < IMG_RGB_H; i++) {
//		for (int j = 0; j < IMG_RGB_W; j++) {
//			uint16_t rgb565Data = 0;
//			//R
//			rgb565Data += (imageRGB[i][j][0] << 11);
//			//G
//			rgb565Data += (imageRGB[i][j][1] << 6);
//			//B
//			rgb565Data += imageRGB[i][j][2];//恢复成rgb565的16位数据
//
//			uint8_t low8Bit = (uint8_t)rgb565Data;
//			uint8_t high8Bit = (rgb565Data >> 8);
//
//			if (low8Bit == 0xfe) //猜测防止与帧尾搞混
//				low8Bit = 0xff;
//			if (high8Bit == 0xfe)
//				high8Bit = 0xff;
//
//			LQ_UART_PutChar(LPUART4, low8Bit); //发送数据
//			LQ_UART_PutChar(LPUART4, high8Bit); //发送数据
//		}
//	}
//
//	LQ_UART_PutChar(LPUART4, 0xfe);  //帧尾
//	LQ_UART_PutChar(LPUART4, 0x01);  //帧尾
//}

//u8 imageInfo[IMG_INFO_NUM];  //6
//bool saveImgRGBInSdram(int k) { //sdram中储存rgb数据
//	if (k * (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM) + (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM) > SDRAM_SIZE_KBYTES * 1024)
//		return false;
//	
////pBuffer:字节指针 WriteAddr:要写入的地址 n:要写入的字节数
//	SEMC_SDRAM_WriteBuffer((u8*)imageRGB, k * (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM), IMG_RGB_H * IMG_RGB_W * 3);
//	
//	imageInfo[0] = (currentServoDty - SERVO_MID_PWM_VAL) + 120;
//	imageInfo[1] = currentTurnMode;
//	imageInfo[2] = currentSpeedLeft + 128;
//	imageInfo[3] = currentSpeedRight + 128;
//	imageInfo[4] = purpostEncValLeft;
//	imageInfo[5] = purpostEncValRight;
//	SEMC_SDRAM_WriteBuffer((u8*)imageInfo, k * (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM) + IMG_RGB_H * IMG_RGB_W * 3, IMG_INFO_NUM);
//	
//	return true;
//}

//void saveImgRGBsToSdCard(int imageNum) {
//	char info[20];
//	u32 total, free;
//
//	while (USDHC1_Init()) {
//		sprintf(getOledBuf(7), "SD Card Error!Please Check!"); LCD_refreshOneLine(7, false);
//		delay_ms(500);
//	}
//
//	exfuns_init();							//为fatfs相关变量申请内存				 
//	f_mount(fs[0], "0:", 1); 					//挂载SD卡 
//	f_mount(fs[2], "2:", 1); 				//挂载FLASH.
//
//	while (exf_getfree("0:", &total, &free))	//得到SD卡的总容量和剩余容量
//	{
//		sprintf(getOledBuf(7), "SD Card Fatfs Error"); LCD_refreshOneLine(7, false);
//		delay_ms(200);
//	}
//	sprintf(getOledBuf(5), "Fatfs OK"); LCD_refreshOneLine(5, false);
//	sprintf(getOledBuf(6), "imgsNum: %d", imageNum); LCD_refreshOneLine(6, false);
//	sprintf(getOledBuf(7), "SD Free Size: %d MB", free); LCD_refreshOneLine(7, false); delay_ms(500);
//
//	sprintf(getOledBuf(7), "going to write file"); LCD_refreshOneLine(7, false); delay_ms(500);
//	
//	int fileNum = imageNum / 200;
//	if (imageNum % 200 != 0) fileNum++;
//	for (int i = 0; i < fileNum; i++) {
//		sprintf(info, "0:imgs%d.txt", i);
//		if (mf_open(info, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
//			sprintf(getOledBuf(7), "successfully created"); LCD_refreshOneLine(7, false); delay_ms(500);
//			
//			int endIndex = (i + 1) * 200;
//			if (endIndex > imageNum)
//				endIndex = imageNum;
//			for (int j = i * 200; j < endIndex; j++) {
//				sprintf(getOledBuf(7), "writing %d img", j); LCD_refreshOneLine(7, false);
//				keyScanForUI();
//				SEMC_SDRAM_ReadBuffer((u8*)imageRGB, j * (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM), IMG_RGB_H * IMG_RGB_W * 3);
//				mf_write(imageRGB, IMG_RGB_H * IMG_RGB_W * 3);
//				SEMC_SDRAM_ReadBuffer((u8*)imageInfo, j * (IMG_RGB_H * IMG_RGB_W * 3 + IMG_INFO_NUM) + IMG_RGB_H * IMG_RGB_W * 3, IMG_INFO_NUM);
//				mf_write(imageInfo, IMG_INFO_NUM);
//			}
//			
//			mf_close();
//			sprintf(getOledBuf(7), "successfully closed"); LCD_refreshOneLine(7, false); delay_ms(500);
//		}
//	}
//}
