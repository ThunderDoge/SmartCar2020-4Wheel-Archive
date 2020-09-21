#include "sys.h"


#include "szx_image.h"

#ifndef __SZX_COLOR_H__
#define __SZX_COLOR_H__
/********************************************/
/*******************颜色分割*****************/
/********************************************/
#define LIGHTNESS_THRESHOLD (64) //作为阈值,低于这个阈值不会是路,高于这个阈值除了蓝色外不会是背景布
#define REFLECT_THRESHOLD (239) //反光阈值
#define MIN_RED_S (70) //判定为红色的点的最小饱和度取值

//imageRGB[IMG_RGB_H][IMG_RGB_W][3]必须满足0-31 0-31 0-31 R G B
//必须先调用colorSegmentationInit再调用isWhite/isBlueOrBlack
extern void colorSegmentationInit(void);
extern bool isWhite(int i, int j);
extern bool isBlueOrBlack(int i, int j);
extern bool isRed(int i, int j);
extern volatile bool colorValid[IMG_RGB_H][IMG_RGB_W];
extern volatile bool whiteBuffer[IMG_RGB_H][IMG_RGB_W];
extern volatile bool blueOrBlackBuffer[IMG_RGB_H][IMG_RGB_W];
extern volatile bool redBuffer[IMG_RGB_H][IMG_RGB_W];
#endif
