#include "UIControl.h"
#include "UIVar.h"

Page* currentPagePointer = &uiMainPage;
Page* prePagePointer = &uiMainPage;
extern void delayms(uint16_t ms);

void UI_initialize(void);

void UI_initialize(void) {
  
  uiMainPageInit();
  uiCamPageInit();
  uiAdcPageInit();
  uiSpeedPageInit();
  uiMainConPageInit();
  uiRunPageInit();
  
  currentPagePointer->doDisplay();
}

void changeCurrentPageTo(int pageIndex) {
	prePagePointer = currentPagePointer;

	switch (pageIndex) {
	case UI_MAIN_PAGE:
		currentPagePointer = &uiMainPage;
		break;
	case UI_CAM_PAGE:
		currentPagePointer = &uiCamPage;
		break;
	case UI_ADC_PAGE:
		currentPagePointer = &uiAdcPage;
		break;
	case UI_SPEED_PAGE:
		currentPagePointer = &uiSpeedPage;
		break;
        case UI_MAINCON_PAGE:
		currentPagePointer = &uiMainConPage;
		break;
	case UI_RUN_PAGE:
		currentPagePointer = &uiRunPage;
		break;
	}
}

void changeCurrentPageToPrePage(void) {
	currentPagePointer = prePagePointer;
}

void keyScanForUI(void) {
	//��ð���ɨ����,֧������
	unsigned char keyRes = KEY_Read(KeyMode);

	//���ݰ������;������õ�ǰҳ����ĸ���Ӧ����
	//������ɺ����ʱ��ֹ����ÿ�ΰ�����Ӧ����
	if (keyRes == OK || keyRes == ESC) {
		currentPagePointer->doOKorESC(keyRes);
		delayms(300);
	}
	else if (keyRes == UP || keyRes == DOWN) {
		currentPagePointer->doUPorDOWN(keyRes);
		delayms(100);
	}
	else if (keyRes == ADD || keyRes == SUB) {
		currentPagePointer->doADDorSUB(keyRes);
		delayms(50);
	}
	else {
		return;
	}

	//������Ӧ��ɺ�ˢ�µ�ǰҳ�棬���û�а��������򲻻�ˢ��ҳ��(������return)
	currentPagePointer->doDisplay();
}