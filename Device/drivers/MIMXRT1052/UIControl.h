#ifndef __UI_CONTROL_H__
#define __UI_CONTROL_H__

#include "LQ_KEY.h"
#include "stdio.h"
#include "LQ_12864.h"

typedef struct {
	//该页面行数
	int currentChosenPos;
	int maxChosenPos;

	//该页面对操作的响应
	void(*doOKorESC)(int);
	void(*doUPorDOWN)(int);
	void(*doADDorSUB)(int);
	
	//该页面如何展示
	void(*doDisplay)(void);
} Page;

extern void UI_initialize(void);
extern void keyScanForUI(void);
extern void changeCurrentPageTo(int pageIndex); // changeCurrentPageTo(UI_MAIN_PAGE);
extern void changeCurrentPageToPrePage(void);

extern Page uiMainPage;
extern Page uiCamPage;
extern Page uiSpeedPage;
extern Page uiAdcPage;
extern Page uiMainConPage;
extern Page uiRunPage;

extern void uiMainPageInit(void);
extern void uiCamPageInit(void);
extern void uiSpeedPageInit(void);
extern void uiAdcPageInit(void);
extern void uiMainConPageInit(void);
extern void uiRunPageInit(void);

#define UI_MAIN_PAGE 0
#define UI_CAM_PAGE 1
#define UI_ADC_PAGE 2
#define UI_SPEED_PAGE 3
#define UI_MAINCON_PAGE 4
#define UI_RUN_PAGE 5



#endif