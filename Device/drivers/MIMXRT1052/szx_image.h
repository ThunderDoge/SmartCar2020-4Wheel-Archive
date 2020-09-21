#include "sys.h"

#ifndef __SZX_IMAGE_H__
#define __SZX_IMAGE_H__

#define IMG_RGB_H 30 //不超过255 oled图像高度
#define IMG_RGB_W 40 //不超过255 oled图像宽度

/*
* 数据类型声明
*/
typedef unsigned char  uint8;  /*  8 bits */
typedef unsigned short uint16; /* 16 bits */
typedef unsigned int   uint32; /* 32 bits */

/******************Buffer********************/
/*********除工具外,不改变传入图像数组********/
/*********只改变Buffer内的取值***************/
extern volatile uint8 imageRGB[IMG_RGB_H][IMG_RGB_W][3]; //0-31 0-31 0-31 R G B 

/********************************************/
/*********************中线*******************/
/********************************************/
#define MAX_LINE_POINT_NUM (IMG_RGB_H * IMG_RGB_W)

#define EFFECTIVE_ROW (IMG_RGB_H / 30) // >= 1
#define AMPLITUDE_LIMIT (IMG_RGB_W)
#define ATTENUATION_COEFFICIENT (0)
//道路长度相关,与前瞻高度相关
#define LOST_LINE_THRESHOLD 4
#define CROSS_EDGE_LEN (4)
#define FILTER_K_THRESHOLD 6
#define roadLenK (1.7333) //15
#define roadLenB (14)
#define roadMoreLostLine (8)
#define roadMoreMaxAveErrorLostLine (13)

extern void imageProcessParamsInit(void);//extern表示可在其他模块中寻找定义
extern void findCenterLine(void);
extern void centerLineValidation(void);

extern volatile bool CENTER_UNFOUND;
extern volatile bool leftIsLost;
extern volatile bool rightIsLost;
extern volatile short leftLine[IMG_RGB_H];
extern volatile short rightLine[IMG_RGB_H];
extern volatile short centerLine[IMG_RGB_H];
extern volatile bool leftValid[IMG_RGB_H];
extern volatile bool rightValid[IMG_RGB_H];
extern volatile bool centerValid[IMG_RGB_H];

//终点线
extern volatile unsigned int isFinishLineCount;
extern volatile unsigned int isNotFinishLineCount;
extern void updateFinishLineCount(void);

//断路
extern volatile short isBrokenRoadAngleCount;
extern bool isBeforeBrokenRoad(void);
extern volatile unsigned int outOfBrokenRoadCount;
extern void updateOutOfBrokenRoadCount(void);

//坡道
#define RAMP_END_ROW (13)
#define RAMP_MORE_WHITE_THR (10)
extern volatile unsigned int isRampCount;
extern void updateRampCount(void);

#endif
