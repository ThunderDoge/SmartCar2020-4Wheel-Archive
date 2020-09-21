#include "UIControl.h"
#include "UIVar.h"
#include "pitimer.h"

Page uiCamPage;
extern Threshold threshold;
extern uint16_t tfMiniDistance;
extern uint16_t tfMiniStrength;

static void uiCamPageDoOKorESC(int OP_TYPE);
static void uiCamPageDoUPorDOWN(int OP_TYPE);
static void uiCamPageDowADDorSUB(int OP_TYPE);
static void uiCamPageDoDisplay(void);

void uiCamPageInit(void) {
	uiCamPage.currentChosenPos = 0;
	uiCamPage.maxChosenPos = 10;

	uiCamPage.doOKorESC = uiCamPageDoOKorESC;
	uiCamPage.doUPorDOWN = uiCamPageDoUPorDOWN;
	uiCamPage.doADDorSUB = uiCamPageDowADDorSUB;
	uiCamPage.doDisplay = uiCamPageDoDisplay;
}

static void uiCamPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
		changeCurrentPageTo(UI_RUN_PAGE);
	}
	if (OP_TYPE == ESC) {
		changeCurrentPageTo(UI_MAIN_PAGE);
	}
}

static void uiCamPageDoUPorDOWN(int OP_TYPE) {
	if (OP_TYPE == DOWN && uiCamPage.currentChosenPos < uiCamPage.maxChosenPos) {
		uiCamPage.currentChosenPos++;
	}
	if (OP_TYPE == UP && uiCamPage.currentChosenPos > 0) {
		uiCamPage.currentChosenPos--;
	}
}

static void uiCamPageDowADDorSUB(int OP_TYPE) {
	if (uiCamPage.currentChosenPos == 0) {
		if (OP_TYPE == ADD)
			threshold.LOSE_LINE_SUM += 0.01f;
		if (OP_TYPE == SUB && threshold.LOSE_LINE_SUM > 0)
			threshold.LOSE_LINE_SUM -= 0.01f;
	}
	else if (uiCamPage.currentChosenPos == 1) {
		if (OP_TYPE == ADD)
			Ui_Control_Point += 1;
		if (OP_TYPE == SUB && Ui_Control_Point > 0)
			Ui_Control_Point -= 1;
	}
	else if (uiCamPage.currentChosenPos == 2) {
		if (OP_TYPE == ADD)
			Fixed_Finish_Flag = true;
		if (OP_TYPE == SUB)
			Fixed_Finish_Flag = false;
	}
	else if (uiCamPage.currentChosenPos == 3) {
		 if (OP_TYPE == ADD && Ui_Dis_Err_Min < 0.0f)
			 Ui_Dis_Err_Min += 0.1f;
		 if (OP_TYPE == SUB && Ui_Dis_Err_Min > -12.0f)
			 Ui_Dis_Err_Min -= 0.1f;
	}
	else if (uiCamPage.currentChosenPos == 4) {
		if (OP_TYPE == ADD)
			Ui_Fifo_Sum++;
		if (OP_TYPE == SUB && Ui_Fifo_Sum > 0)
			Ui_Fifo_Sum--;
	}
	else if (uiCamPage.currentChosenPos == 5) {
		if (OP_TYPE == ADD)
			Ui_Find_Range++;
		if (OP_TYPE == SUB && Ui_Find_Range > 0)
			Ui_Find_Range--;
	}
	else if (uiCamPage.currentChosenPos == 6) {
		if (OP_TYPE == ADD)
			Ui_Servo_Mid_Pwm++;
		if (OP_TYPE == SUB)
			Ui_Servo_Mid_Pwm--;
	}
        else if (uiCamPage.currentChosenPos == 7) {
		if (OP_TYPE == ADD)
			threshold.CROSS_VALUE += 0.01f;
		if (OP_TYPE == SUB && threshold.CROSS_VALUE > 0.0f)
			threshold.CROSS_VALUE -= 0.01f;
	}
        else if (uiCamPage.currentChosenPos == 8) {
		if (OP_TYPE == ADD)
			Ui_Cross_Time++;
		if (OP_TYPE == SUB && Ui_Cross_Time > 0)
			Ui_Cross_Time--;
	}
        else if (uiCamPage.currentChosenPos == 9) {
		if (OP_TYPE == ADD)
			Ui_Laser_Threshold++;
		if (OP_TYPE == SUB && Ui_Laser_Threshold > 0)
			Ui_Laser_Threshold--;
	}
        else if (uiCamPage.currentChosenPos == 10) {
		if (OP_TYPE == ADD)
                {
                  LCD_CLS();
                  while(1)
                  {
                    unsigned char txt[16];
                    //获得按键扫描结果,支持连按
                    unsigned char keyRes = KEY_Read(0);
                    
                    sprintf(txt,"Distance:%d ",tfMiniDistance);
                    LCD_P6x8Str(4,0,txt);
                    
                    sprintf(txt,"Strength:%d ",tfMiniStrength);
                    LCD_P6x8Str(4,1,txt);
                    
                    if (keyRes == SUB)
                      break;
                  }
                }
		if (OP_TYPE == SUB && Ui_Laser_Threshold > 0)
			Ui_Laser_Threshold--;
	}
}

static void uiCamPageDoDisplay(void) {
	if (uiCamPage.currentChosenPos < 8) {
		sprintf(getOledBuf(0), "lostthr %.2f", threshold.LOSE_LINE_SUM);
		sprintf(getOledBuf(1), "controlp %d", Ui_Control_Point);
		if(Fixed_Finish_Flag)
                  sprintf(getOledBuf(2), "FixedFinish");
                else
                  sprintf(getOledBuf(2), "LaserFinish" );
		sprintf(getOledBuf(3), "minerr %.1f", Ui_Dis_Err_Min);
                sprintf(getOledBuf(4), "Fifosum %d", Ui_Fifo_Sum);
		sprintf(getOledBuf(5), "FindRange %d", Ui_Find_Range);
                sprintf(getOledBuf(6), "ServoMid %d", Ui_Servo_Mid_Pwm);
                sprintf(getOledBuf(7), "CrossThr %.2f", threshold.CROSS_VALUE);
	}
        else if(uiCamPage.currentChosenPos < 16){
                sprintf(getOledBuf(0), "CrossTime %d", Ui_Cross_Time);
                sprintf(getOledBuf(1), "LaserThr %d", Ui_Laser_Threshold);
                sprintf(getOledBuf(2), "LaserTest");
                for (int i = 3; i < 8; i++) {
			sprintf(getOledBuf(i), " ");
		}
        }

	for (int i = 0; i < 8; i++) {
		if (uiCamPage.currentChosenPos % 8 == i)
			LCD_refreshOneLine(i, true);
		else
			LCD_refreshOneLine(i, false);
	}
}