#include "UIControl.h"
#include "UIVar.h"
#include "main.h"

Page uiMainConPage;

static void uiMainConPageDoOKorESC(int OP_TYPE);
static void uiMainConPageDoUPorDOWN(int OP_TYPE);
static void uiMainConPageDowADDorSUB(int OP_TYPE);
static void uiMainConPageDoDisplay(void);

int currentRingN = 0;
int currentRingRadio = 0;

void uiMainConPageInit(void) {
	uiMainConPage.currentChosenPos = 0;
	uiMainConPage.maxChosenPos = 15;

	uiMainConPage.doOKorESC = uiMainConPageDoOKorESC;
	uiMainConPage.doUPorDOWN = uiMainConPageDoUPorDOWN;
	uiMainConPage.doADDorSUB = uiMainConPageDowADDorSUB;
	uiMainConPage.doDisplay = uiMainConPageDoDisplay;
}

static void uiMainConPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
		changeCurrentPageTo(UI_RUN_PAGE);
	}
	if (OP_TYPE == ESC) {
		changeCurrentPageTo(UI_MAIN_PAGE);
	}
}

static void uiMainConPageDoUPorDOWN(int OP_TYPE) {
	if (OP_TYPE == DOWN && uiMainConPage.currentChosenPos < uiMainConPage.maxChosenPos) {
		uiMainConPage.currentChosenPos++;
	}
	if (OP_TYPE == UP && uiMainConPage.currentChosenPos > 0) {
		uiMainConPage.currentChosenPos--;
	}
}

static void uiMainConPageDowADDorSUB(int OP_TYPE) {
	if (uiMainConPage.currentChosenPos == 0) {
		if (OP_TYPE == ADD)
			Ui_Ring_Shield_Time[Ui_Speed_Mode][currentRingN] += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_Shield_Time[Ui_Speed_Mode][currentRingN] > 0)
			Ui_Ring_Shield_Time[Ui_Speed_Mode][currentRingN] -= 0.01f;
	}
	else if (uiMainConPage.currentChosenPos == 1) {
		if (OP_TYPE == ADD)
			Ui_Ring_In_Servo_Time[Ui_Speed_Mode][currentRingN] += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_In_Servo_Time[Ui_Speed_Mode][currentRingN] > 0)
			Ui_Ring_In_Servo_Time[Ui_Speed_Mode][currentRingN] -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 2) {
		if (OP_TYPE == ADD && Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][currentRingN] < ADC_INTERVAL_PWM_VAL)
			Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][currentRingN] += 1;
		if (OP_TYPE == SUB && Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][currentRingN] > -ADC_INTERVAL_PWM_VAL)
			Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][currentRingN] -= 1;
	}
	else if (uiMainConPage.currentChosenPos == 3) {
		if (OP_TYPE == ADD)
			Ui_Ring_Directions[Ui_Speed_Mode][currentRingN] = true;
		if (OP_TYPE == SUB)
			Ui_Ring_Directions[Ui_Speed_Mode][currentRingN] = false;
	}
	else if (uiMainConPage.currentChosenPos == 4) {
		if (OP_TYPE == ADD && currentRingN < 5)
			currentRingN++;
		if (OP_TYPE == SUB && currentRingN > 0)
			currentRingN--;
	}
        else if (uiMainConPage.currentChosenPos == 5) {
		if (OP_TYPE == ADD)
			Ui_Ring_Max_Speed[Ui_Speed_Mode][currentRingN]++;
		if (OP_TYPE == SUB && Ui_Ring_Max_Speed[Ui_Speed_Mode][currentRingN] > 0)
			Ui_Ring_Max_Speed[Ui_Speed_Mode][currentRingN]--;
	}
        else if (uiMainConPage.currentChosenPos == 6) {
		if (OP_TYPE == ADD)
			Ui_Ring_Min_Speed[Ui_Speed_Mode][currentRingN]++;
		if (OP_TYPE == SUB && Ui_Ring_Min_Speed[Ui_Speed_Mode][currentRingN] > 0)
			Ui_Ring_Min_Speed[Ui_Speed_Mode][currentRingN]--;
	}
        else if (uiMainConPage.currentChosenPos == 7) {
		if (OP_TYPE == ADD)
			Ui_Ring_In_Delay_Time[Ui_Speed_Mode][currentRingN] += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_In_Delay_Time[Ui_Speed_Mode][currentRingN] > 0)
			Ui_Ring_In_Delay_Time[Ui_Speed_Mode][currentRingN] -= 0.01f;
	}
	else if (uiMainConPage.currentChosenPos == 8) {
		if (OP_TYPE == ADD)
			Ui_Ramp_Time += 0.01f;
		if (OP_TYPE == SUB && Ui_Ramp_Time > 0)
			Ui_Ramp_Time -= 0.01f;
	}
	else if (uiMainConPage.currentChosenPos == 9) {
		if (OP_TYPE == ADD)
			Ui_Ramp_Up_Time += 0.01f;
		if (OP_TYPE == SUB && Ui_Ramp_Up_Time > 0)
			Ui_Ramp_Up_Time -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 10) {
		if (OP_TYPE == ADD)
			Ui_Stop_Time[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Stop_Time[Ui_Speed_Mode] - 0.01f > 0)
			Ui_Stop_Time[Ui_Speed_Mode] -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 11) {
		if (OP_TYPE == ADD)
			Ui_Finish_Shield_Time[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Finish_Shield_Time[Ui_Speed_Mode] > 0)
			Ui_Finish_Shield_Time[Ui_Speed_Mode] -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 12) {
		if (OP_TYPE == ADD)
			Ui_Out_Garage_Time += 0.01f;
		if (OP_TYPE == SUB && Ui_Out_Garage_Time > 0)
			Ui_Out_Garage_Time -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 13) {
		if (OP_TYPE == ADD)
			Ui_Out_Garage_Servo_Duty = ADC_INTERVAL_PWM_VAL;
		if (OP_TYPE == SUB)
			Ui_Out_Garage_Servo_Duty = -ADC_INTERVAL_PWM_VAL;
	}
        else if (uiMainConPage.currentChosenPos == 14) {
		if (OP_TYPE == ADD)
			Ui_Back_Time[Ui_Speed_Mode] += 0.01f ;
		if (OP_TYPE == SUB && Ui_Back_Time[Ui_Speed_Mode] > 0.0f)
			Ui_Back_Time[Ui_Speed_Mode] -= 0.01f;
	}
        else if (uiMainConPage.currentChosenPos == 15) {
		if (OP_TYPE == ADD)
			Ui_Stop_Delay_Time[Ui_Speed_Mode] += 0.01f ;
		if (OP_TYPE == SUB && Ui_Stop_Delay_Time[Ui_Speed_Mode] > 0.0f)
			Ui_Stop_Delay_Time[Ui_Speed_Mode] -= 0.01f;
	}
}

static void uiMainConPageDoDisplay(void) {
	if (uiMainConPage.currentChosenPos < 8) {
		sprintf(getOledBuf(0), "RingShieldT %.2f", Ui_Ring_Shield_Time[Ui_Speed_Mode][currentRingN]);
		sprintf(getOledBuf(1), "RingServoT %.2f", Ui_Ring_In_Servo_Time[Ui_Speed_Mode][currentRingN]);
		sprintf(getOledBuf(2), "RinginServo %d", Ui_Ring_In_Servo_Duty[Ui_Speed_Mode][currentRingN]);
                if(Ui_Ring_Directions[Ui_Speed_Mode][currentRingN])
                  sprintf(getOledBuf(3), "RingDir: Right");
                else
                  sprintf(getOledBuf(3), "RingDir: Left" );
		sprintf(getOledBuf(4), "RingNth %d", currentRingN);
                sprintf(getOledBuf(5), "RingMaxSpeed %d", Ui_Ring_Max_Speed[Ui_Speed_Mode][currentRingN]);
                sprintf(getOledBuf(6), "RingMinSpeed %d", Ui_Ring_Min_Speed[Ui_Speed_Mode][currentRingN]);
		sprintf(getOledBuf(7), "RinginDealy %.2f", Ui_Ring_In_Delay_Time[Ui_Speed_Mode][currentRingN]);
	}
	else {
                sprintf(getOledBuf(0), "RampTime %.2f", Ui_Ramp_Time);
                sprintf(getOledBuf(1), "RampUpT %.2f", Ui_Ramp_Up_Time);
                sprintf(getOledBuf(2), "StopT %.2f", Ui_Stop_Time[Ui_Speed_Mode]);
		sprintf(getOledBuf(3), "FShieldT %.2f", Ui_Finish_Shield_Time[Ui_Speed_Mode]);
		sprintf(getOledBuf(4), "OutTime %.2f", Ui_Out_Garage_Time);
		sprintf(getOledBuf(5), "OutServo %d", Ui_Out_Garage_Servo_Duty);
                sprintf(getOledBuf(6), "HallBT %.2f", Ui_Back_Time[Ui_Speed_Mode]);
                sprintf(getOledBuf(7), "FDelayT %.2f", Ui_Stop_Delay_Time[Ui_Speed_Mode]);
	}

	for (int i = 0; i < 8; i++) {
		if (uiMainConPage.currentChosenPos % 8 == i)
			LCD_refreshOneLine(i, true);
		else
			LCD_refreshOneLine(i, false);
	}
}