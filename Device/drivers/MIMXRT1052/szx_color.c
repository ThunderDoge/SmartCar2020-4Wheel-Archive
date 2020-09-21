/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】坤哥
【版    本】V1.0
【注    意】本文件为坤哥移植，使用rgb565的HSV色彩变换
【最后更新】2020年1月12日
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "szx_color.h"
#include "LQ_Camera.h"
volatile bool colorValid[IMG_RGB_H][IMG_RGB_W];
volatile bool whiteBuffer[IMG_RGB_H][IMG_RGB_W];
volatile bool blueOrBlackBuffer[IMG_RGB_H][IMG_RGB_W];
//extern volatile bool redBuffer[IMG_RGB_H][IMG_RGB_W];

void colorSegmentationInit(void) { //颜色区分初始化
	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			colorValid[i][j] = false;   //存放每个像素颜色的元素均为0
		}
	}
}

void doPixelColorSegmentation(int i, int j) { //每个像素的颜色分割，每个像素有三种颜色 256×256×256
	Get_Use_Image();
        Get_01_Value(0);
        
  
  
  
  
  
  
  
//        static short r, g, b, cmax, cmin, delta;    //2020年无路障，可使用灰度 二值化处理即可
//	static uint8 H, S, V; //0-180 0-255 0-255 
//
//	//R G B
//	r = imageRGB[i][j][0];//imageRGB[IMG_RGB_H][IMG_RGB_W][3] 该数组信息的通过摄像头获取
//	g = imageRGB[i][j][1];
//	b = imageRGB[i][j][2];
//
//	//cmax cmin
//	cmax = cmin = r;
//	if (g > cmax) cmax = g; 
//	if (b > cmax) cmax = b;//选出每个像素中最大和最小的三原色
//	if (g < cmin) cmin = g; 
//	if (b < cmin) cmin = b;
//
//	//delta
//	delta = cmax - cmin;
//
//	//H
//	if (delta == 0) {
//		H = 0;
//	}
//	else if (cmax == r) {
//		if (g >= b)
//			H = (30 * (g - b)) / delta;
//		else
//			H = 180 - (30 * (b - g)) / delta;
//	}
//	else if (cmax == g) {
//		H = 60 + (30 * (b - r)) / delta;
//	}
//	else if (cmax == b) {
//		H = 120 + (30 * (r - g)) / delta;
//	}
//	//S
//	if (cmax == 0)
//		S = 0;
//	else
//		S = (delta * 255) / cmax;
//
//	//V
//	V = cmax * 8;
//
//	//update color buffer
//	whiteBuffer[i][j] = blueOrBlackBuffer[i][j] = redBuffer[i][j] = false;
//	//注意颜色划分范围不要重复,一个RGB取值对应一种颜色
//	//优先判白
//	if (LIGHTNESS_THRESHOLD <= V && V <= REFLECT_THRESHOLD) {
//		//饱和度不足且(蓝色不同时高于红绿通道)
//		if (S <= 43 && !(b > r && b > g)) 
//                {
//			whiteBuffer[i][j] = true;
//		}
//		//一定范围的偏色(偏冷)
//		else if (S <= 110 && 10 < H && H <= 50) 
//                {
//			whiteBuffer[i][j] = true;
//		}
//		//一定范围的偏色(偏暖), 注意不能与红色划分范围重复
//		else if (S < MIN_RED_S && ((0 <= H && H <= 10) || (156 <= H && H <= 180))) {
//			whiteBuffer[i][j] = true;
//		}
//	}
//
//	//若判为非白,则判蓝
//	if (!whiteBuffer[i][j]) {
//		if (100 <= H && H <= 124 && S >= 43 && V >= 46)
//			blueOrBlackBuffer[i][j] = true;
//		else if (V <= LIGHTNESS_THRESHOLD)
//			blueOrBlackBuffer[i][j] = true;
//	}
//	
//	//若判为非白且非蓝,则判红
//	if (!whiteBuffer[i][j] && !blueOrBlackBuffer[i][j]) {
//		if (S >= MIN_RED_S && V >= 46
//			&& ((0 <= H && H <= 10) || (156 <= H && H <= 180))
//		)
//			redBuffer[i][j] = true;
//	}
//
//	colorValid[i][j] = true;
}

bool isWhite(int i, int j) {
	//越界保护
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))//不满足（i，j） 在oled 30，40的范围内
		return false;

	if (!colorValid[i][j])  //若（i，j）不是拥有有效颜色的点 if（！a）,当a=0，！a为1，执行后续操作
		                      //就是判断了每个像素的颜色区分都初始化了以后开始颜色区分
         
		doPixelColorSegmentation(i, j);//每个像素区分颜色

	return whiteBuffer[i][j]; //白色缓冲
}

bool isBlueOrBlack(int i, int j) {
	//越界保护
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))
		return true;

	if (!colorValid[i][j])
		doPixelColorSegmentation(i, j);

	return blueOrBlackBuffer[i][j];
}

bool isRed(int i, int j) {
	//越界保护
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))
		return false;
	
	if (!colorValid[i][j])
		doPixelColorSegmentation(i, j);
	
	return redBuffer[i][j];
}
