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
	//���������ֵ
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
	//ȷ��Բ������
	int radioType = Ui_Ring_Radii[ringDirectionN];
	//ȷ��Բ���ٶ�
	int setSpeed = Ui_Ring_Set_Speeds[radioType];

	/***********************************************/
	/****************׼�������׶�*******************/
	/***********************************************/
	//׼�������׶ε���ʱ
	int beforeEnterRingCnt = Ui_Before_Enter_Ring_Cnts[radioType];
	//׼�������׶�,һ����ʱ�����
	beforeEnterRingPhase(beforeEnterRingCnt, setSpeed);

	/***********************************************/
	/****************���������׶�*******************/
	/***********************************************/
	//���������׶εĴ��
	int ringTurnInPwm;
	if (Ui_Ring_Directions[ringDirectionN])
		ringTurnInPwm = SERVO_MID_PWM_VAL - Ui_Ring_Turn_In_Pwms[radioType];
	else
		ringTurnInPwm = SERVO_MID_PWM_VAL + Ui_Ring_Turn_In_Pwms[radioType];
	//���������׶ν����ĵ����ֵ
	float ringTurnInEndRadio = Ui_Ring_Turn_In_End_Radios;
	//���������׶�
	ringTurnInPhase(ringTurnInPwm, ringTurnInEndRadio, setSpeed);

	/***********************************************/
	/****************����+�����׶�******************/
	/***********************************************/
	//����+�����׶ε���С���
	int ringInsideMinPwm;
	if (Ui_Ring_Directions[ringDirectionN])
		ringInsideMinPwm = SERVO_MID_PWM_VAL - Ui_Ring_Inside_Min_Pwms[radioType];
	else
		ringInsideMinPwm = SERVO_MID_PWM_VAL + Ui_Ring_Inside_Min_Pwms[radioType];
	//����+�����׶ν����ĵ����ֵ
	//float ringOutEndRadio = Ui_Ring_Out_End_Radios;
	//����+�����׶�
	//ringOutPhase(ringInsideMinPwm, ringOutEndRadio, setSpeed);
}

static void beforeEnterRingPhase(int beforeEnterRingCnt, int setSpeed) {
	while (beforeEnterRingCnt > 0) {
		beforeEnterRingCnt--;

		UiSaveimgAdcstopOled();
		//����ͷ���ȴ��,�̶��ٶ�,ģʽΪԲ��׼���׶�
		updateCamAndAdcThenTurn(true, true, setSpeed, setSpeed, TURN_BEFORE_RING);
	}
}

static void ringOutPhase(int ringInsideMinPwm, float ringOutEndRadio, int setSpeed) {
	while (true) {
		UiSaveimgAdcstopOled();
		//��С���
		updateCamAndAdcThenTurnWithMin(ringInsideMinPwm, true, setSpeed, TURN_RING_OUT);
		//��ͼ�����ý׶�
		if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) > ringOutEndRadio)
			return;
	}
}

static void ringTurnInPhase(int ringTurnInPwm, float ringTurnInEndRadio, int setSpeed) {	
	while (true) {
		UiSaveimgAdcstopOled();
		//�������
		updateCamAndAdcThenTurnWithFixed(ringTurnInPwm, true, setSpeed, TURN_RING_TURN_IN);
		//TODO ��ͼ�����ý׶�
		if (((leftAdcVal + rightAdcVal) / (Ui_Max_Left_ADC_VAL + Ui_Max_Right_ADC_VAL)) < ringTurnInEndRadio)
				return;
	}
}