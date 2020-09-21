/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨������i.MX RT1052���İ�-���ܳ���
����    д������
����    ����V1.0
��ע    �⡿���ļ�Ϊ������ֲ��ʹ��rgb565��HSVɫ�ʱ任
�������¡�2020��1��12��
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "szx_color.h"
#include "LQ_Camera.h"
volatile bool colorValid[IMG_RGB_H][IMG_RGB_W];
volatile bool whiteBuffer[IMG_RGB_H][IMG_RGB_W];
volatile bool blueOrBlackBuffer[IMG_RGB_H][IMG_RGB_W];
//extern volatile bool redBuffer[IMG_RGB_H][IMG_RGB_W];

void colorSegmentationInit(void) { //��ɫ���ֳ�ʼ��
	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			colorValid[i][j] = false;   //���ÿ��������ɫ��Ԫ�ؾ�Ϊ0
		}
	}
}

void doPixelColorSegmentation(int i, int j) { //ÿ�����ص���ɫ�ָÿ��������������ɫ 256��256��256
	Get_Use_Image();
        Get_01_Value(0);
        
  
  
  
  
  
  
  
//        static short r, g, b, cmax, cmin, delta;    //2020����·�ϣ���ʹ�ûҶ� ��ֵ��������
//	static uint8 H, S, V; //0-180 0-255 0-255 
//
//	//R G B
//	r = imageRGB[i][j][0];//imageRGB[IMG_RGB_H][IMG_RGB_W][3] ��������Ϣ��ͨ������ͷ��ȡ
//	g = imageRGB[i][j][1];
//	b = imageRGB[i][j][2];
//
//	//cmax cmin
//	cmax = cmin = r;
//	if (g > cmax) cmax = g; 
//	if (b > cmax) cmax = b;//ѡ��ÿ��������������С����ԭɫ
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
//	//ע����ɫ���ַ�Χ��Ҫ�ظ�,һ��RGBȡֵ��Ӧһ����ɫ
//	//�����а�
//	if (LIGHTNESS_THRESHOLD <= V && V <= REFLECT_THRESHOLD) {
//		//���ͶȲ�����(��ɫ��ͬʱ���ں���ͨ��)
//		if (S <= 43 && !(b > r && b > g)) 
//                {
//			whiteBuffer[i][j] = true;
//		}
//		//һ����Χ��ƫɫ(ƫ��)
//		else if (S <= 110 && 10 < H && H <= 50) 
//                {
//			whiteBuffer[i][j] = true;
//		}
//		//һ����Χ��ƫɫ(ƫů), ע�ⲻ�����ɫ���ַ�Χ�ظ�
//		else if (S < MIN_RED_S && ((0 <= H && H <= 10) || (156 <= H && H <= 180))) {
//			whiteBuffer[i][j] = true;
//		}
//	}
//
//	//����Ϊ�ǰ�,������
//	if (!whiteBuffer[i][j]) {
//		if (100 <= H && H <= 124 && S >= 43 && V >= 46)
//			blueOrBlackBuffer[i][j] = true;
//		else if (V <= LIGHTNESS_THRESHOLD)
//			blueOrBlackBuffer[i][j] = true;
//	}
//	
//	//����Ϊ�ǰ��ҷ���,���к�
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
	//Խ�籣��
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))//�����㣨i��j�� ��oled 30��40�ķ�Χ��
		return false;

	if (!colorValid[i][j])  //����i��j������ӵ����Ч��ɫ�ĵ� if����a��,��a=0����aΪ1��ִ�к�������
		                      //�����ж���ÿ�����ص���ɫ���ֶ���ʼ�����Ժ�ʼ��ɫ����
         
		doPixelColorSegmentation(i, j);//ÿ������������ɫ

	return whiteBuffer[i][j]; //��ɫ����
}

bool isBlueOrBlack(int i, int j) {
	//Խ�籣��
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))
		return true;

	if (!colorValid[i][j])
		doPixelColorSegmentation(i, j);

	return blueOrBlackBuffer[i][j];
}

bool isRed(int i, int j) {
	//Խ�籣��
	if (!(i >= 0 && i < IMG_RGB_H && j >= 0 && j < IMG_RGB_W))
		return false;
	
	if (!colorValid[i][j])
		doPixelColorSegmentation(i, j);
	
	return redBuffer[i][j];
}
