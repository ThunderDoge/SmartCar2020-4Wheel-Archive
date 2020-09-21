#include "UIControl.h"
#include "UIVar.h"
#include "LQ_LED.h"
#include "main.h"
#include "speedProcess.h"
#include "adcprocess.h"
#include "ControlElement.h"
Page uiRunPage;

static void uiRunPageDoOKorESC(int OP_TYPE);
static void uiRunPageDoUPorDOWN(int OP_TYPE);
static void uiRunPageDowADDorSUB(int OP_TYPE);
static void uiRunPageDoDisplay(void);

void uiRunPageInit(void) {
	uiRunPage.currentChosenPos = 0;
	uiRunPage.maxChosenPos = 0;

	uiRunPage.doOKorESC = uiRunPageDoOKorESC;
	uiRunPage.doUPorDOWN = uiRunPageDoUPorDOWN;
	uiRunPage.doADDorSUB = uiRunPageDowADDorSUB;
	uiRunPage.doDisplay = uiRunPageDoDisplay;
}

static void uiRunPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
		writeUiVarsToFlash();
		buzzer(1);
		delayms(500);
		buzzer(0);
		Ui_Stop_Flag = false;
		changeCurrentPageToPrePage();
                //enableAdcstop();
	}
	if (OP_TYPE == ESC) {
		changeCurrentPageToPrePage();
	}
}

static void uiRunPageDoUPorDOWN(int OP_TYPE) {
	return;
}

static void uiRunPageDowADDorSUB(int OP_TYPE) {
	return;
}

static void uiRunPageDoDisplay(void) {
	for (int i = 0; i < 8; i++) {
		sprintf(getOledBuf(i), " ");
	}

	if (Ui_Stop_Flag) {
		sprintf(getOledBuf(2), "  Let the Car Run ?");
		sprintf(getOledBuf(5), "    OK  /  ESC");
	}
	else {
		sprintf(getOledBuf(2), "  Car is Running ");
	}

	for (int i = 0; i < 8; i++) {
		LCD_refreshOneLine(i, false);
	}
}
