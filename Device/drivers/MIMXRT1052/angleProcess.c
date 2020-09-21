/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨������i.MX RT1052���İ�-���ܳ���
����    д������
����    �ġ�szx
����    ����V1.0
��ע    �⡿���ļ�Ϊ�����ǿ����ļ���ǰ�ڲ���Ҫ��п��ƣ�ȥ��ADC��ֻ������ͷ���ƣ�������szx_image����ϵ
            V1.0ֻ����ֱ������ͨ����ģʽ
�������¡�2020��1��14��
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "angleProcess.h"

#include "szx_image.h"
#include "speedProcess.h"
#include "adcprocess.h"
#include "myUtil.h"
#include "UIControl.h"

//#include "elementCommom.h" //����getGlobalRuningMileage�ⲻӦ������������ TODO check

#include "LQ_PWM.h"

/*****************************************/
/************�ڲ�����and����**************/
/*****************************************/
float preCamError = 0;

volatile short Cam_Process_Result = 0;

/////////////////////////////////����UI����������������룩///////////////

///////////////////////////////////////////////////////////




//����ͼ�����ߡ�����ͷ���(ע��getImageRGB,imageProcessInit,findCenterLine��Ӧ�����ڴ˺�����)
void camProcessAndGetValue(void);
//���µ��ֵ����д��
//void adcProcessAndGetValue(void);

/*****************************************/
/************�ⲿ����and����**************/
/*****************************************/
volatile short MaxCenterError = 0;

//ע�⣺����ļ�����ǺͿ��ٺ�������һ��ģ�������������������

//�������ֱ����ͼ�����ߺ͵��ֵ1��
//����camFirst���Ⱥ�˳�������
//fixedSpeed��������
//���ȷ��������ÿ��ִ���Ƿֱ����ͼ�����ߺ͵��ֵ1��,����������һ�ֵ����ٶȺʹ��
void updateCamAndAdcThenTurn(bool camFirst, bool fixedSpeed, int minFixedSpeed, int maxFixedSpeed, int turnMode) {
	//static int centerUnfoundCnt = 0;
	
	//����/������
//	if (fixedSpeed)
//		setToFixedSpeed(minFixedSpeed, maxFixedSpeed);
//	//������һ����������������ͷ�ܣ���Ҫ�����ٵ�ǰ������һ���ҵ������� //TODO ���
//	else if (!CENTER_UNFOUND)
//		cancelSetToFixedSpeed();
	/*****************************************/
	/*********������ͷ����ٸ��µ��ֵ********/
	/*****************************************/
	if (camFirst) {
		//����ͼ�����ߡ�����ͷ���
		camProcessAndGetValue();
		//����·���趨ֵ�л����(ͨ������Ϊ����δ�ҵ���ʵ��) //TODO check
//		for (int i = 0; i < Ui_SetToAdc_N; i++) {
//			if (Ui_SetToAdc_StartMileage[i] <= getGlobalRuningMileage()
//				&& getGlobalRuningMileage() <= Ui_SetToAdc_EndMileage[i])
//				CENTER_UNFOUND = true;
//		}
		//����� //TODO ���
		if (!CENTER_UNFOUND) {
			//����ͷ���
			LQ_SetServoDty(Cam_Process_Result + SERVO_MID_PWM_VAL);
			//���µ��ֵ����д��
//			adcProcessAndGetValue();
			
			//��������δ�ҵ�����
			//centerUnfoundCnt = 0;
		}
		//else {
                        //Ui_Stop_Flag = true;
//			//���µ��ֵ����д��
//			adcProcessAndGetValue();
//			
//			//��������δ�ҵ�����
//			centerUnfoundCnt++;
//			//������δ�ҵ�����������ֵ/isBrokenRoadAngleCount�����л����
//			if (centerUnfoundCnt >= Ui_CenterUnfound_Threshold || isBrokenRoadAngleCount >= 1) {
//				//����
//				setToFixedSpeed(Ui_CenterUnfound_Speed, Ui_CenterUnfound_Speed);
//				//��д��
//				LQ_AdcSetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL, TURN_USE_ADC_CAM_NOT_FOUND);
//			}
//				
		//}
	}
	/*****************************************/
	/**********�ȵ�д���ٸ�������ͷ*********/
	/*****************************************/
//	else {
//		//���µ��ֵ����д��
//		adcProcessAndGetValue();
//		//��д��
//		LQ_AdcSetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL, turnMode);
//		//����ͼ�����ߡ�����ͷ���
//		camProcessAndGetValue();
//	}
}

////Բ���ô��(����ͷ����)
//void updateCamAndAdcThenTurnWithFixed(int fixedPwm, bool fixedSpeed, int setSpeed, int turnMode) {
//	//����/������
//	if (fixedSpeed)
//		setToFixedSpeed(setSpeed, setSpeed);
//	else
//		cancelSetToFixedSpeed();
//	
//	//����ͼ�����ߡ�����ͷ���
//	camProcessAndGetValue();
//	//�̶����
//	LQ_SetServoDty(fixedPwm);
//	//���µ��ֵ����д��
//	adcProcessAndGetValue();
//}
////Բ���ô��(����ͷ����)
//void updateCamAndAdcThenTurnWithMin(int minPwm, bool fixedSpeed, int setSpeed, int turnMode) {
//	//����/������
//	if (fixedSpeed)
//		setToFixedSpeed(setSpeed, setSpeed);
//	else
//		cancelSetToFixedSpeed();
//	
//	//����ͼ�����ߡ�����ͷ���
//	camProcessAndGetValue();
//	//��С���
//	int servoPwm = Cam_Process_Result + SERVO_MID_PWM_VAL;
//	if (minPwm < SERVO_MID_PWM_VAL) { //��ת
//		if (servoPwm > minPwm)
//			servoPwm = minPwm;
//	}
//	else { //��ת
//		if (servoPwm < minPwm)
//			servoPwm = minPwm;
//	}
//	LQ_SetServoDty(servoPwm, turnMode);
//	//���µ��ֵ����д��
//	adcProcessAndGetValue();
//}
/////////////////////////��������������δ�ù�������ע�͵ģ����ڴ�/////////////////////////////
/*****************************************/
/*************�ڲ�����ʵ��****************/
/*****************************************/
//inline void LQ_SetAdcServoDty(uint16_t duty);
//inline void LQ_SetAdcServoDty(uint16_t duty)    
//{
//	if(duty > SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL + ADC_INTERVAL_PWM_VAL;
//	if(duty < SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL) duty = SERVO_MID_PWM_VAL - ADC_INTERVAL_PWM_VAL;
//    
//        LQ_PWM_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmB, duty); //A9   
//}
//����ֵ��IMG_RGB_W / 2
short getCenterError(void) {
	//short thisCenterError = centerLine[Ui_Control_Line] - IMG_RGB_W / 2;
	//if (thisCenterError > (SERVO_INTERVAL_PWM_VAL / Ui_Cam_P))
		//thisCenterError = SERVO_INTERVAL_PWM_VAL / Ui_Cam_P;
	//return thisCenterError;
}

//������
void camProcessAndGetValue(void) {
	//����ͼ��
	//getImageRGB();
	//��������
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
	
	//���������ʱ����P
	short currentSpeed = (currentSpeedLeft + currentSpeedRight) / 2;
	short moreSpeed = 0;
	if (currentSpeed > Ui_Min_Speed[Ui_Speed_Mode]) moreSpeed = currentSpeed - Ui_Min_Speed[Ui_Speed_Mode]; 
	
	//Cam_Process_Result = (Ui_Cam_P + moreSpeed * Ui_Cam_P_More) * nowError + Ui_Cam_D* (nowError - preCamError);
	preCamError = nowError;
}

//Բ���ô��(����ͷ����)
void updateCamAndAdcThenTurnWithFixed(int fixedPwm, bool fixedSpeed, int setSpeed, int turnMode) {
	//����/������
	if (fixedSpeed)
		setToFixedSpeed(setSpeed, setSpeed);
	else
		cancelSetToFixedSpeed();
	
	//����ͼ�����ߡ�����ͷ���
	camProcessAndGetValue();
	//�̶����
	LQ_SetServoDty(fixedPwm);
	//���µ��ֵ����д��
	//adcProcessAndGetValue();
}

//Բ���ô��(����ͷ����)
void updateCamAndAdcThenTurnWithMin(int minPwm, bool fixedSpeed, int setSpeed, int turnMode) {
	//����/������
	if (fixedSpeed)
		setToFixedSpeed(setSpeed, setSpeed);
	else
		cancelSetToFixedSpeed();
	
	//����ͼ�����ߡ�����ͷ���
	camProcessAndGetValue();
	//��С���
	int servoPwm = Cam_Process_Result + SERVO_MID_PWM_VAL;
	if (minPwm < SERVO_MID_PWM_VAL) { //��ת
		if (servoPwm > minPwm)
			servoPwm = minPwm;
	}
	else { //��ת
		if (servoPwm < minPwm)
			servoPwm = minPwm;
	}
	LQ_SetServoDty(servoPwm);
	//���µ��ֵ����д��
	//adcProcessAndGetValue();
}

//float getAdcError(void) {
//	//�������ҵ��ֵ
//	updateLeftAdcVal();
//	updateRightAdcVal();
//	
//	//һ�ι�һ��
//	float leftAdcPercentage = leftAdcVal / Ui_Max_Left_ADC_VAL;
//	float rightAdcPercentage = rightAdcVal / Ui_Max_Right_ADC_VAL;
//	
//	//���ι�һ��
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
