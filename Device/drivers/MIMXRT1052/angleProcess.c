/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】坤哥
【修    改】szx
【版    本】V1.0
【注    意】本文件为舵机打角控制文件，前期不需要电感控制，去掉ADC，只用摄像头控制，建立与szx_image的联系
            V1.0只考虑直道和普通拐弯模式
【最后更新】2020年1月14日
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "angleProcess.h"

#include "szx_image.h"
#include "speedProcess.h"
#include "adcprocess.h"
#include "myUtil.h"
#include "UIControl.h"

//#include "elementCommom.h" //除了getGlobalRuningMileage外不应调用其他函数 TODO check

#include "LQ_PWM.h"

/*****************************************/
/************内部变量and函数**************/
/*****************************************/
float preCamError = 0;

volatile short Cam_Process_Result = 0;

/////////////////////////////////设置UI参数（依据坤哥代码）///////////////

///////////////////////////////////////////////////////////




//更新图像、中线、摄像头打角(注意getImageRGB,imageProcessInit,findCenterLine仅应出现在此函数中)
void camProcessAndGetValue(void);
//更新电感值、电感打角
//void adcProcessAndGetValue(void);

/*****************************************/
/************外部变量and函数**************/
/*****************************************/
volatile short MaxCenterError = 0;

//注意：这里的几个打角和控速函数必须一起改！！！！！！！！！！

//本函数分别更新图像中线和电感值1次
//根据camFirst有先后顺序的区别
//fixedSpeed进行限速
//务必确保本函数每次执行是分别更新图像中线和电感值1次,并根据其中一种调整速度和打角
void updateCamAndAdcThenTurn(bool camFirst, bool fixedSpeed, int minFixedSpeed, int maxFixedSpeed, int turnMode) {
	//static int centerUnfoundCnt = 0;
	
	//限速/不限速
//	if (fixedSpeed)
//		setToFixedSpeed(minFixedSpeed, maxFixedSpeed);
//	//不限速一般是正常赛道摄像头跑，则要求不限速的前提是上一次找到了中线 //TODO 检查
//	else if (!CENTER_UNFOUND)
//		cancelSetToFixedSpeed();
	/*****************************************/
	/*********先摄像头打角再更新电感值********/
	/*****************************************/
	if (camFirst) {
		//更新图像、中线、摄像头打角
		camProcessAndGetValue();
		//根据路程设定值切换电感(通过设置为中线未找到来实现) //TODO check
//		for (int i = 0; i < Ui_SetToAdc_N; i++) {
//			if (Ui_SetToAdc_StartMileage[i] <= getGlobalRuningMileage()
//				&& getGlobalRuningMileage() <= Ui_SetToAdc_EndMileage[i])
//				CENTER_UNFOUND = true;
//		}
		//分情况 //TODO 检查
		if (!CENTER_UNFOUND) {
			//摄像头打角
			LQ_SetServoDty(Cam_Process_Result + SERVO_MID_PWM_VAL);
			//更新电感值、电感打角
//			adcProcessAndGetValue();
			
			//更新中线未找到计数
			//centerUnfoundCnt = 0;
		}
		//else {
                        //Ui_Stop_Flag = true;
//			//更新电感值、电感打角
//			adcProcessAndGetValue();
//			
//			//更新中线未找到计数
//			centerUnfoundCnt++;
//			//若中线未找到计数超过阈值/isBrokenRoadAngleCount，则切换电磁
//			if (centerUnfoundCnt >= Ui_CenterUnfound_Threshold || isBrokenRoadAngleCount >= 1) {
//				//限速
//				setToFixedSpeed(Ui_CenterUnfound_Speed, Ui_CenterUnfound_Speed);
//				//电感打角
//				LQ_AdcSetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL, TURN_USE_ADC_CAM_NOT_FOUND);
//			}
//				
		//}
	}
	/*****************************************/
	/**********先电感打角再更新摄像头*********/
	/*****************************************/
//	else {
//		//更新电感值、电感打角
//		adcProcessAndGetValue();
//		//电感打角
//		LQ_AdcSetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL, turnMode);
//		//更新图像、中线、摄像头打角
//		camProcessAndGetValue();
//	}
}

////圆环用打角(摄像头优先)
//void updateCamAndAdcThenTurnWithFixed(int fixedPwm, bool fixedSpeed, int setSpeed, int turnMode) {
//	//限速/不限速
//	if (fixedSpeed)
//		setToFixedSpeed(setSpeed, setSpeed);
//	else
//		cancelSetToFixedSpeed();
//	
//	//更新图像、中线、摄像头打角
//	camProcessAndGetValue();
//	//固定打角
//	LQ_SetServoDty(fixedPwm);
//	//更新电感值、电感打角
//	adcProcessAndGetValue();
//}
////圆环用打角(摄像头优先)
//void updateCamAndAdcThenTurnWithMin(int minPwm, bool fixedSpeed, int setSpeed, int turnMode) {
//	//限速/不限速
//	if (fixedSpeed)
//		setToFixedSpeed(setSpeed, setSpeed);
//	else
//		cancelSetToFixedSpeed();
//	
//	//更新图像、中线、摄像头打角
//	camProcessAndGetValue();
//	//最小打角
//	int servoPwm = Cam_Process_Result + SERVO_MID_PWM_VAL;
//	if (minPwm < SERVO_MID_PWM_VAL) { //左转
//		if (servoPwm > minPwm)
//			servoPwm = minPwm;
//	}
//	else { //右转
//		if (servoPwm < minPwm)
//			servoPwm = minPwm;
//	}
//	LQ_SetServoDty(servoPwm, turnMode);
//	//更新电感值、电感打角
//	adcProcessAndGetValue();
//}
/////////////////////////上面三个函数都未用功能所以注释的，后期打开/////////////////////////////
/*****************************************/
/*************内部函数实现****************/
/*****************************************/
//inline void LQ_SetAdcServoDty(uint16_t duty);
//inline void LQ_SetAdcServoDty(uint16_t duty)    
//{
//	if(duty > SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL;
//	if(duty < SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL;
//    
//        LQ_PWM_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmB, duty); //A9   
//}
//期望值是IMG_RGB_W / 2
short getCenterError(void) {
	//short thisCenterError = centerLine[Ui_Control_Line] - IMG_RGB_W / 2;
	//if (thisCenterError > (SERVO_INTERVAL_PWM_VAL / Ui_Cam_P))
		//thisCenterError = SERVO_INTERVAL_PWM_VAL / Ui_Cam_P;
	//return thisCenterError;
}

//左负右正
void camProcessAndGetValue(void) {
	//更新图像
	//getImageRGB();
	//更新中线
	imageProcessParamsInit();
	findCenterLine();
	centerLineValidation();

	short curRowAbsCenterError = 0;
	short tmpMaxCenterError = 0;
	for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {		
		curRowAbsCenterError = abs(centerLine[i] - IMG_RGB_W / 2);
		if (curRowAbsCenterError > tmpMaxCenterError) {
			tmpMaxCenterError = curRowAbsCenterError;
		}
	}
	MaxCenterError = tmpMaxCenterError;
	
	float nowError = getCenterError();
	
	//大于最低速时增加P
	short currentSpeed = (currentSpeedLeft + currentSpeedRight) / 2;
	short moreSpeed = 0;
	if (currentSpeed > Ui_Min_Speed[Ui_Speed_Mode]) moreSpeed = currentSpeed - Ui_Min_Speed[Ui_Speed_Mode]; 
	
	//Cam_Process_Result = (Ui_Cam_P + moreSpeed * Ui_Cam_P_More) * nowError + Ui_Cam_D* (nowError - preCamError);
	preCamError = nowError;
}

//圆环用打角(摄像头优先)
void updateCamAndAdcThenTurnWithFixed(int fixedPwm, bool fixedSpeed, int setSpeed, int turnMode) {
	//限速/不限速
	if (fixedSpeed)
		setToFixedSpeed(setSpeed, setSpeed);
	else
		cancelSetToFixedSpeed();
	
	//更新图像、中线、摄像头打角
	camProcessAndGetValue();
	//固定打角
	LQ_SetServoDty(fixedPwm);
	//更新电感值、电感打角
	//adcProcessAndGetValue();
}

//圆环用打角(摄像头优先)
void updateCamAndAdcThenTurnWithMin(int minPwm, bool fixedSpeed, int setSpeed, int turnMode) {
	//限速/不限速
	if (fixedSpeed)
		setToFixedSpeed(setSpeed, setSpeed);
	else
		cancelSetToFixedSpeed();
	
	//更新图像、中线、摄像头打角
	camProcessAndGetValue();
	//最小打角
	int servoPwm = Cam_Process_Result + SERVO_MID_PWM_VAL;
	if (minPwm < SERVO_MID_PWM_VAL) { //左转
		if (servoPwm > minPwm)
			servoPwm = minPwm;
	}
	else { //右转
		if (servoPwm < minPwm)
			servoPwm = minPwm;
	}
	LQ_SetServoDty(servoPwm);
	//更新电感值、电感打角
	//adcProcessAndGetValue();
}

//float getAdcError(void) {
//	//更新左右电感值
//	updateLeftAdcVal();
//	updateRightAdcVal();
//	
//	//一次归一化
//	float leftAdcPercentage = leftAdcVal / Ui_Max_Left_ADC_VAL;
//	float rightAdcPercentage = rightAdcVal / Ui_Max_Right_ADC_VAL;
//	
//	//二次归一化
//	return 100 * (leftAdcPercentage - rightAdcPercentage) / (leftAdcPercentage + rightAdcPercentage);
//}

//void adcProcessAndGetValue(void)
//{
//	float nowAdcError = getAdcError();
//
//	
//	if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) >= Ui_Adc_Valid_Threshold) {
//		if (-Ui_Adc_Thr <= nowAdcError && nowAdcError <= Ui_Adc_Thr)
//			Adc_Process_Result = -((short)(Ui_Adc_P / 2 * nowAdcError + Ui_Adc_D * (nowAdcError - preAdcError)));
//		else
//			Adc_Process_Result = -((short)(Ui_Adc_P * nowAdcError + Ui_Adc_D * (nowAdcError - preAdcError)));
//		preAdcError = nowAdcError;
//	}
//}
