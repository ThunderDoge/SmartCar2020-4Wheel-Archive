#include "sys.h"


#include "szx_image.h"

#ifndef __SZX_COLOR_H__
#define __SZX_COLOR_H__
/********************************************/
/*******************��ɫ�ָ�*****************/
/********************************************/
#define LIGHTNESS_THRESHOLD (64) //��Ϊ��ֵ,���������ֵ������·,���������ֵ������ɫ�ⲻ���Ǳ�����
#define REFLECT_THRESHOLD (239) //������ֵ
#define MIN_RED_S (70) //�ж�Ϊ��ɫ�ĵ����С���Ͷ�ȡֵ

//imageRGB[IMG_RGB_H][IMG_RGB_W][3]��������0-31 0-31 0-31 R G B
//�����ȵ���colorSegmentationInit�ٵ���isWhite/isBlueOrBlack
extern void colorSegmentationInit(void);
extern bool isWhite(int i, int j);
extern bool isBlueOrBlack(int i, int j);
extern bool isRed(int i, int j);
extern volatile bool colorValid[IMG_RGB_H][IMG_RGB_W];
extern volatile bool whiteBuffer[IMG_RGB_H][IMG_RGB_W];
extern volatile bool blueOrBlackBuffer[IMG_RGB_H][IMG_RGB_W];
extern volatile bool redBuffer[IMG_RGB_H][IMG_RGB_W];
#endif
