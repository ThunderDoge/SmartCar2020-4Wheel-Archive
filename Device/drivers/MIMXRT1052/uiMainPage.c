#include "UIControl.h"
#include "UIVar.h"
#include "adcprocess.h"

Page uiMainPage;

static void uiMainPageDoOKorESC(int OP_TYPE);
static void uiMainPageDoUPorDOWN(int OP_TYPE);
static void uiMainPageDowADDorSUB(int OP_TYPE);
static void uiMainPageDoDisplay(void);

void uiMainPageInit(void) {
	uiMainPage.currentChosenPos = 0;
	uiMainPage.maxChosenPos = 7;

	uiMainPage.doOKorESC = uiMainPageDoOKorESC;
	uiMainPage.doUPorDOWN = uiMainPageDoUPorDOWN;
	uiMainPage.doADDorSUB = uiMainPageDowADDorSUB;
	uiMainPage.doDisplay = uiMainPageDoDisplay;
}

static void uiMainPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
	if (uiMainPage.currentChosenPos == 0) {
			changeCurrentPageTo(UI_CAM_PAGE);
		}
		else if (uiMainPage.currentChosenPos == 1) {
			changeCurrentPageTo(UI_ADC_PAGE);
			//Ui_Is_Debug_Page = true;
		}
		else if (uiMainPage.currentChosenPos == 2) {
			changeCurrentPageTo(UI_SPEED_PAGE);
		}
                else if (uiMainPage.currentChosenPos == 3) {
			changeCurrentPageTo(UI_MAINCON_PAGE);
		}
		else if (uiMainPage.currentChosenPos == 4) {
			changeCurrentPageTo(UI_RUN_PAGE);
		}
                else if (uiMainPage.currentChosenPos == 5) {
			KeyMode = 1;
		}
	}
	if (OP_TYPE == ESC) {
                KeyMode = 0;
		return;
	}
}

static void uiMainPageDoUPorDOWN(int OP_TYPE) {
	if (OP_TYPE == DOWN && uiMainPage.currentChosenPos < uiMainPage.maxChosenPos) {
		uiMainPage.currentChosenPos++;
	}
	if (OP_TYPE == UP && uiMainPage.currentChosenPos > 0) {
		uiMainPage.currentChosenPos--;
	}
}

static void uiMainPageDowADDorSUB(int OP_TYPE) {
	 if (uiMainPage.currentChosenPos == 6) {
		if (OP_TYPE == ADD && Ui_Speed_Mode < 5)
			Ui_Speed_Mode++;
		if (OP_TYPE == SUB && Ui_Speed_Mode > 0)
			Ui_Speed_Mode--;
	}
}

static void uiMainPageDoDisplay(void) {
	if (uiMainPage.currentChosenPos < 8) {
		sprintf(getOledBuf(0), "CAM SET");
		sprintf(getOledBuf(1), "ADC SET");
		sprintf(getOledBuf(2), "SPEED SET");
                sprintf(getOledBuf(3), "MainCon SET");
		sprintf(getOledBuf(4), "CAR RUN");
                sprintf(getOledBuf(5), "KEY MODE");
                sprintf(getOledBuf(6), "SpeedMode: %d",Ui_Speed_Mode);
                sprintf(getOledBuf(7), "BatV: %.2f",BatV);
	}
	else if (uiMainPage.currentChosenPos < 16) {
		for (int i = 0; i < 8; i++) {
			sprintf(getOledBuf(i), " ");
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			sprintf(getOledBuf(i), " ");
		}
	}

	for (int i = 0; i < 8; i++) {
		if (uiMainPage.currentChosenPos % 8 == i)
			LCD_refreshOneLine(i, true);
		else
			LCD_refreshOneLine(i, false);
	}
}