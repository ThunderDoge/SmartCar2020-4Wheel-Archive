#include "UIControl.h"
#include "UIVar.h"

Page uiSpeedPage;

static void uiSpeedPageDoOKorESC(int OP_TYPE);
static void uiSpeedPageDoUPorDOWN(int OP_TYPE);
static void uiSpeedPageDowADDorSUB(int OP_TYPE);
static void uiSpeedPageDoDisplay(void);

void uiSpeedPageInit(void) {
	uiSpeedPage.currentChosenPos = 0;
	uiSpeedPage.maxChosenPos = 20;

	uiSpeedPage.doOKorESC = uiSpeedPageDoOKorESC;
	uiSpeedPage.doUPorDOWN = uiSpeedPageDoUPorDOWN;
	uiSpeedPage.doADDorSUB = uiSpeedPageDowADDorSUB;
	uiSpeedPage.doDisplay = uiSpeedPageDoDisplay;
}

static void uiSpeedPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
		changeCurrentPageTo(UI_RUN_PAGE);
	}
	if (OP_TYPE == ESC) {
		changeCurrentPageTo(UI_MAIN_PAGE);
	}
}

static void uiSpeedPageDoUPorDOWN(int OP_TYPE) {
	if (OP_TYPE == DOWN && uiSpeedPage.currentChosenPos < uiSpeedPage.maxChosenPos) {
		uiSpeedPage.currentChosenPos++;
	}
	if (OP_TYPE == UP && uiSpeedPage.currentChosenPos > 0) {
		uiSpeedPage.currentChosenPos--;
	}
}

static void uiSpeedPageDowADDorSUB(int OP_TYPE) {
	if (uiSpeedPage.currentChosenPos == 0) {
		if (OP_TYPE == ADD)
			Ui_Motor_P += 0.1f;
		if (OP_TYPE == SUB && Ui_Motor_P > 0)
			Ui_Motor_P -= 0.1f;
	}
	else if (uiSpeedPage.currentChosenPos == 1) {
		if (OP_TYPE == ADD)
			Ui_Motor_I += 0.1f;
		if (OP_TYPE == SUB && Ui_Motor_I > 0)
			Ui_Motor_I -= 0.1f;
	}
        else if (uiSpeedPage.currentChosenPos == 2) {
		if (OP_TYPE == ADD)
			Ui_Motor_D += 0.1f;
		if (OP_TYPE == SUB && Ui_Motor_D > 0)
			Ui_Motor_D -= 0.1f;
	}
	else if (uiSpeedPage.currentChosenPos == 3) {
		if (OP_TYPE == ADD && Ui_Uk_To_Pwm < 50)
			Ui_Uk_To_Pwm++;
		if (OP_TYPE == SUB && Ui_Uk_To_Pwm > 1)
			Ui_Uk_To_Pwm--;
	}
	else if (uiSpeedPage.currentChosenPos == 4) {
		if (OP_TYPE == ADD)
			Ui_Max_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB && Ui_Max_Speed[Ui_Speed_Mode] > 0 && Ui_Max_Speed[Ui_Speed_Mode] > Ui_Min_Speed[Ui_Speed_Mode])
			Ui_Max_Speed[Ui_Speed_Mode]--;
	}
	else if (uiSpeedPage.currentChosenPos == 5) {
		if (OP_TYPE == ADD && Ui_Max_Speed[Ui_Speed_Mode] > Ui_Min_Speed[Ui_Speed_Mode])
			Ui_Min_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB && Ui_Min_Speed[Ui_Speed_Mode] > 0)
			Ui_Min_Speed[Ui_Speed_Mode]--;
	}
	else if (uiSpeedPage.currentChosenPos == 6) {
		if (OP_TYPE == ADD)
			Ui_Ramp_Up_Speed++;
		if (OP_TYPE == SUB && Ui_Ramp_Up_Speed > 0)
			Ui_Ramp_Up_Speed--;
	}
	else if (uiSpeedPage.currentChosenPos == 7) {
		if (OP_TYPE == ADD)
			Ui_Ramp_Dowm_Speed++;
		if (OP_TYPE == SUB && Ui_Ramp_Dowm_Speed > 0)
			Ui_Ramp_Dowm_Speed--;
	}
	else if (uiSpeedPage.currentChosenPos == 8) {
		if (OP_TYPE == ADD)
			Ui_Straight_Diff_K[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Straight_Diff_K[Ui_Speed_Mode] > 0)
			Ui_Straight_Diff_K[Ui_Speed_Mode] -= 0.01f;
	}
	else if (uiSpeedPage.currentChosenPos == 9) {
		if (OP_TYPE == ADD)
			Ui_Bend_Diff_K[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Bend_Diff_K[Ui_Speed_Mode] - 0.01f >= 0)
			Ui_Bend_Diff_K[Ui_Speed_Mode] -= 0.01f;
	}
	else if (uiSpeedPage.currentChosenPos == 10) {
		if (OP_TYPE == ADD)
			Ui_Motor_Threshold++;
		if (OP_TYPE == SUB && Ui_Motor_Threshold > 0)
			Ui_Motor_Threshold--;
	}
        else if (uiSpeedPage.currentChosenPos == 11) {
		if (OP_TYPE == ADD)
			Ui_Ring_In_Max_Speed++;
		if (OP_TYPE == SUB && Ui_Ring_In_Max_Speed > 0 && Ui_Ring_In_Max_Speed > Ui_Ring_In_Min_Speed)
			Ui_Ring_In_Max_Speed--;
	}
        else if (uiSpeedPage.currentChosenPos == 12) {
		if (OP_TYPE == ADD && Ui_Ring_In_Max_Speed > Ui_Ring_In_Min_Speed)
			Ui_Ring_In_Min_Speed++;
		if (OP_TYPE == SUB && Ui_Ring_In_Min_Speed > 0 )
			Ui_Ring_In_Min_Speed--;
	}
        else if (uiSpeedPage.currentChosenPos == 13) {
		if (OP_TYPE == ADD)
			Ui_Out_Garage_Max_Speed++;
		if (OP_TYPE == SUB && Ui_Out_Garage_Max_Speed > 0 && Ui_Out_Garage_Max_Speed > Ui_Out_Garage_Min_Speed)
			Ui_Out_Garage_Max_Speed--;
	}
        else if (uiSpeedPage.currentChosenPos == 14) {
		if (OP_TYPE == ADD && Ui_Out_Garage_Max_Speed > Ui_Out_Garage_Min_Speed)
			Ui_Out_Garage_Min_Speed++;
		if (OP_TYPE == SUB && Ui_Out_Garage_Min_Speed > 0 )
			Ui_Out_Garage_Min_Speed--;
	}
        else if (uiSpeedPage.currentChosenPos == 15) {
		if (OP_TYPE == ADD && Ui_Inner_Max_Speed[Ui_Speed_Mode] < Ui_Min_Speed[Ui_Speed_Mode])
			Ui_Inner_Max_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB && Ui_Inner_Max_Speed[Ui_Speed_Mode] > 35)
			Ui_Inner_Max_Speed[Ui_Speed_Mode]--;
	}
        else if (uiSpeedPage.currentChosenPos == 16) {
		if (OP_TYPE == ADD)
			Ui_MotorD_Threshold++;
		if (OP_TYPE == SUB && Ui_MotorD_Threshold > 0)
			Ui_MotorD_Threshold--;
	}
        else if (uiSpeedPage.currentChosenPos == 17) {
		if (OP_TYPE == ADD)
			Ui_SMin_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB && Ui_SMin_Speed[Ui_Speed_Mode] > 0)
			Ui_SMin_Speed[Ui_Speed_Mode]--;
	}
        else if (uiSpeedPage.currentChosenPos == 18) {
		if (OP_TYPE == ADD)
			Ui_Cur_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB && Ui_Cur_Speed[Ui_Speed_Mode] > 0)
			Ui_Cur_Speed[Ui_Speed_Mode]--;
	}
        else if (uiSpeedPage.currentChosenPos == 19) {
		if (OP_TYPE == ADD)
			Ui_Stop_Left_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB)
			Ui_Stop_Left_Speed[Ui_Speed_Mode]--;
	}
        else if (uiSpeedPage.currentChosenPos == 20) {
		if (OP_TYPE == ADD)
			Ui_Stop_Right_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB)
			Ui_Stop_Right_Speed[Ui_Speed_Mode]--;
	}
        else if (uiSpeedPage.currentChosenPos == 20) {
		if (OP_TYPE == ADD)
			Ui_Hall_Stop_Speed[Ui_Speed_Mode]++;
		if (OP_TYPE == SUB)
			Ui_Hall_Stop_Speed[Ui_Speed_Mode]--;
	}
}

static void uiSpeedPageDoDisplay(void) {
	if (uiSpeedPage.currentChosenPos < 8) {
		sprintf(getOledBuf(0), "motorP %.2f", Ui_Motor_P);
		sprintf(getOledBuf(1), "motorI %.2f", Ui_Motor_I);
		sprintf(getOledBuf(2), "motorD %.2f", Ui_Motor_D);
		sprintf(getOledBuf(3), "UkToPwm %d", Ui_Uk_To_Pwm);
		sprintf(getOledBuf(4), "maxSpeed %d", Ui_Max_Speed[Ui_Speed_Mode]);
		sprintf(getOledBuf(5), "minSpeed %d", Ui_Min_Speed[Ui_Speed_Mode]);
		sprintf(getOledBuf(6), "RampUpS %d", Ui_Ramp_Up_Speed);
		sprintf(getOledBuf(7), "RampDowmS %d", Ui_Ramp_Dowm_Speed);
	}
	else if(uiSpeedPage.currentChosenPos < 16){
                sprintf(getOledBuf(0), "sDiffK %.2f", Ui_Straight_Diff_K[Ui_Speed_Mode]);
		sprintf(getOledBuf(1), "bDiffK %.2f", Ui_Bend_Diff_K[Ui_Speed_Mode]);
		sprintf(getOledBuf(2), "motorThr %.2f", Ui_Motor_Threshold);
		sprintf(getOledBuf(3), "RingmaxSpeed %d", Ui_Ring_In_Max_Speed);
		sprintf(getOledBuf(4), "RingminSpeed %d", Ui_Ring_In_Min_Speed);
                sprintf(getOledBuf(5), "OutmaxSpeed %d", Ui_Out_Garage_Max_Speed);
		sprintf(getOledBuf(6), "OutminSpeed %d", Ui_Out_Garage_Min_Speed);
		sprintf(getOledBuf(7), "InnerSpeed %d", Ui_Inner_Max_Speed[Ui_Speed_Mode]);
	}
        else{
                sprintf(getOledBuf(0), "motorDThr %.2f", Ui_MotorD_Threshold);
                sprintf(getOledBuf(1), "SnakeSpeed %d", Ui_SMin_Speed[Ui_Speed_Mode]);
                sprintf(getOledBuf(2), "CurSpeed %d", Ui_Cur_Speed[Ui_Speed_Mode]);
                sprintf(getOledBuf(3), "StopLSpeed %d", Ui_Stop_Left_Speed[Ui_Speed_Mode]);
                sprintf(getOledBuf(4), "StopRSpeed %d", Ui_Stop_Right_Speed[Ui_Speed_Mode]);
                sprintf(getOledBuf(5), "HallSpeed %d", Ui_Hall_Stop_Speed[Ui_Speed_Mode]);
                for (int i = 6; i < 8; i++) {
			sprintf(getOledBuf(i), " ");
		}
        }

	for (int i = 0; i < 8; i++) {
		if (uiSpeedPage.currentChosenPos % 8 == i)
			LCD_refreshOneLine(i, true);
		else
			LCD_refreshOneLine(i, false);
	}
}
