#include "main.h"
#include "adcprocess.h"
#include "ControlElement.h"
#include "angleProcess.h"

static void presetMode(int ringDirectionN);
static void beforeEnterRingPhase(int beforeEnterRingCnt, int setSpeed);
static void ringOutPhase(int ringInsideMinPwm, float ringOutEndRadio, int setSpeed);
static void ringTurnInPhase(int ringTurnInPwm, float ringTurnInEndRadio,int setSpeed);



bool ringTryIn(void) {
	static int isRingCnt = 0;
	//进环电感阈值
	if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) > Ui_Ring_In_Adc)
		isRingCnt++;
	else
		isRingCnt = 0;
	
	return isRingCnt >= 3;
}

void ringControl(int ringDirectionN) {
	presetMode(ringDirectionN);
}

static void presetMode(int ringDirectionN) {
	//确定圆环类型
	int radioType = Ui_Ring_Radii[ringDirectionN];
	//确定圆环速度
	int setSpeed = Ui_Ring_Set_Speeds[radioType];

	/***********************************************/
	/****************准备进环阶段*******************/
	/***********************************************/
	//准备进环阶段的延时
	int beforeEnterRingCnt = Ui_Before_Enter_Ring_Cnts[radioType];
	//准备进环阶段,一段延时后进环
	beforeEnterRingPhase(beforeEnterRingCnt, setSpeed);

	/***********************************************/
	/****************进环打死阶段*******************/
	/***********************************************/
	//进环打死阶段的打角
	int ringTurnInPwm;
	if (Ui_Ring_Directions[ringDirectionN])
		ringTurnInPwm = SERVO_MID_PWM_VAL - Ui_Ring_Turn_In_Pwms[radioType];
	else
		ringTurnInPwm = SERVO_MID_PWM_VAL + Ui_Ring_Turn_In_Pwms[radioType];
	//进环打死阶段结束的电感阈值
	float ringTurnInEndRadio = Ui_Ring_Turn_In_End_Radios;
	//进环打死阶段
	ringTurnInPhase(ringTurnInPwm, ringTurnInEndRadio, setSpeed);

	/***********************************************/
	/****************环中+出环阶段******************/
	/***********************************************/
	//环中+出环阶段的最小打角
	int ringInsideMinPwm;
	if (Ui_Ring_Directions[ringDirectionN])
		ringInsideMinPwm = SERVO_MID_PWM_VAL - Ui_Ring_Inside_Min_Pwms[radioType];
	else
		ringInsideMinPwm = SERVO_MID_PWM_VAL + Ui_Ring_Inside_Min_Pwms[radioType];
	//环中+出环阶段结束的电感阈值
	//float ringOutEndRadio = Ui_Ring_Out_End_Radios;
	//环中+出环阶段
	//ringOutPhase(ringInsideMinPwm, ringOutEndRadio, setSpeed);
}

static void beforeEnterRingPhase(int beforeEnterRingCnt, int setSpeed) {
	while (beforeEnterRingCnt > 0) {
		beforeEnterRingCnt--;

		UiSaveimgAdcstopOled();
		//摄像头优先打角,固定速度,模式为圆环准备阶段
		updateCamAndAdcThenTurn(true, true, setSpeed, setSpeed, TURN_BEFORE_RING);
	}
}

static void ringOutPhase(int ringInsideMinPwm, float ringOutEndRadio, int setSpeed) {
	while (true) {
		UiSaveimgAdcstopOled();
		//最小打角
		updateCamAndAdcThenTurnWithMin(ringInsideMinPwm, true, setSpeed, TURN_RING_OUT);
		//试图结束该阶段
		if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) > ringOutEndRadio)
			return;
	}
}

static void ringTurnInPhase(int ringTurnInPwm, float ringTurnInEndRadio, int setSpeed) {	
	while (true) {
		UiSaveimgAdcstopOled();
		//给定打角
		updateCamAndAdcThenTurnWithFixed(ringTurnInPwm, true, setSpeed, TURN_RING_TURN_IN);
		//TODO 试图结束该阶段
		if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) < ringTurnInEndRadio)
				return;
	}
}