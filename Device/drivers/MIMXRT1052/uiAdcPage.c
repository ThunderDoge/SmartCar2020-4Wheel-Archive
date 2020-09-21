#include "UIControl.h"
#include "UIVar.h"
#include "adcprocess.h"
#include "pitimer.h"

Page uiAdcPage;
extern Queue queue;
static void uiAdcPageDoOKorESC(int OP_TYPE);
static void uiAdcPageDoUPorDOWN(int OP_TYPE);
static void uiAdcPageDowADDorSUB(int OP_TYPE);
static void uiAdcPageDoDisplay(void);

void uiAdcPageInit(void) {
	uiAdcPage.currentChosenPos = 0;
	uiAdcPage.maxChosenPos = 17;

	uiAdcPage.doOKorESC = uiAdcPageDoOKorESC;
	uiAdcPage.doUPorDOWN = uiAdcPageDoUPorDOWN;
	uiAdcPage.doADDorSUB = uiAdcPageDowADDorSUB;
	uiAdcPage.doDisplay = uiAdcPageDoDisplay;
}

static void uiAdcPageDoOKorESC(int OP_TYPE) {
	if (OP_TYPE == OK) {
		changeCurrentPageTo(UI_RUN_PAGE);
	}
	if (OP_TYPE == ESC) {
		changeCurrentPageTo(UI_MAIN_PAGE);
	}
}

static void uiAdcPageDoUPorDOWN(int OP_TYPE) {
	if (OP_TYPE == DOWN && uiAdcPage.currentChosenPos < uiAdcPage.maxChosenPos) {
		uiAdcPage.currentChosenPos++;
	}
	if (OP_TYPE == UP && uiAdcPage.currentChosenPos > 0) {
		uiAdcPage.currentChosenPos--;
	}
}

static void uiAdcPageDowADDorSUB(int OP_TYPE) {
	if (uiAdcPage.currentChosenPos == 0) {
		if (OP_TYPE == ADD)
			Ui_Adc_SP[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Adc_SP[Ui_Speed_Mode] > 0)
			Ui_Adc_SP[Ui_Speed_Mode] -= 0.01f;
	}
	else if (uiAdcPage.currentChosenPos == 1) {
		if (OP_TYPE == ADD)
			Ui_Adc_SD[Ui_Speed_Mode] += 1;
		if (OP_TYPE == SUB && Ui_Adc_SD[Ui_Speed_Mode] > 0)
			Ui_Adc_SD[Ui_Speed_Mode] -= 1;
	}
        else if (uiAdcPage.currentChosenPos == 2) {
		if (OP_TYPE == ADD)
			Ui_Adc_BP[Ui_Speed_Mode] += 0.01f;
		if (OP_TYPE == SUB && Ui_Adc_BP[Ui_Speed_Mode] > 0)
			Ui_Adc_BP[Ui_Speed_Mode] -= 0.01f;
	}
        else if (uiAdcPage.currentChosenPos == 3) {
		if (OP_TYPE == ADD)
			Ui_Adc_BD[Ui_Speed_Mode] += 1;
		if (OP_TYPE == SUB && Ui_Adc_BD[Ui_Speed_Mode] > 0)
			Ui_Adc_BD[Ui_Speed_Mode] -= 1;
	}
        else if (uiAdcPage.currentChosenPos == 4) {
		if (OP_TYPE == ADD)
			Ui_Ramp_Up_Adc += 0.01f;
		if (OP_TYPE == SUB && Ui_Ramp_Up_Adc > 0)
			Ui_Ramp_Up_Adc -= 0.01f;
	}
        else if (uiAdcPage.currentChosenPos == 5) {
		if (OP_TYPE == ADD)
			Ui_Adc_D_Max += 0.01f;
		if (OP_TYPE == SUB && Ui_Adc_D_Max > 0)
			Ui_Adc_D_Max -= 0.01f;
	}
//	else if (uiAdcPage.currentChosenPos == 6) {
//		if (OP_TYPE == ADD)
//			Ui_Max_Left_ADC_VAL += 10;
//		if (OP_TYPE == SUB && Ui_Max_Left_ADC_VAL > 10)
//			Ui_Max_Left_ADC_VAL -= 10;
//	}
//	else if (uiAdcPage.currentChosenPos == 7) {
//		 if (OP_TYPE == ADD)
//			Ui_Max_Right_ADC_VAL += 10;
//		if (OP_TYPE == SUB && Ui_Max_Right_ADC_VAL > 10)
//			Ui_Max_Right_ADC_VAL -= 10;
//	}
	else if (uiAdcPage.currentChosenPos == 6) {
		if (OP_TYPE == ADD)
			Ui_Ring_In_Adc += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_In_Adc > Ui_Ring_Out_Adc)
			Ui_Ring_In_Adc -= 0.01f;
	}
	else if (uiAdcPage.currentChosenPos == 7) {
		if (OP_TYPE == ADD && Ui_Ring_Out_Adc < 2.0f)
			Ui_Ring_Out_Adc += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_Out_Adc > 0.6f)
			Ui_Ring_Out_Adc -= 0.01f;
	}
        else if (uiAdcPage.currentChosenPos == 8) {
		if (OP_TYPE == ADD && Ui_Turn_Adc)
			Ui_Turn_Adc += 0.01f;
		if (OP_TYPE == SUB && Ui_Turn_Adc)
			Ui_Turn_Adc -= 0.01f;
	}
        else if (uiAdcPage.currentChosenPos == 9) {
		if (OP_TYPE == ADD && Ui_Ring_Turn_In_End_Radios < Ui_Ring_Out_Adc)
			Ui_Ring_Turn_In_End_Radios += 0.01f;
		if (OP_TYPE == SUB && Ui_Ring_Turn_In_End_Radios > 0.0f)
			Ui_Ring_Turn_In_End_Radios -= 0.01f;
			
	}
        else if (uiAdcPage.currentChosenPos == 10) {
		if (OP_TYPE == ADD)
			Ui_Max_MLeft_ADC_VAL += 10;
		if (OP_TYPE == SUB && Ui_Max_MLeft_ADC_VAL > 10)
			Ui_Max_MLeft_ADC_VAL -= 10;
	}
        else if (uiAdcPage.currentChosenPos == 11) {
		if (OP_TYPE == ADD)
			Ui_Max_MRight_ADC_VAL += 10;
		if (OP_TYPE == SUB && Ui_Max_MRight_ADC_VAL > 10)
			Ui_Max_MRight_ADC_VAL -= 10;
	}
        else if (uiAdcPage.currentChosenPos == 12) {
		if (OP_TYPE == ADD)
			Ui_Max_LMid_ADC_VAL += 10;
		if (OP_TYPE == SUB && Ui_Max_LMid_ADC_VAL > 10)
			Ui_Max_LMid_ADC_VAL -= 10;
	}
        else if (uiAdcPage.currentChosenPos == 14) {
		if (OP_TYPE == ADD)
			Ui_Adc_Thr += 0.01f;
		if (OP_TYPE == SUB && Ui_Adc_Thr > 0)
			Ui_Adc_Thr -= 0.01f;
	}
        else if (uiAdcPage.currentChosenPos == 15) {
          if (OP_TYPE == ADD){
			Ui_Save_Adc_Flag = true;
                        LCD_CLS();
                        Ui_Max_Left_ADC_VAL = 0;
                        Ui_Max_Right_ADC_VAL = 0;
                        Ui_Max_MLeft_ADC_VAL = 0;
                        Ui_Max_MRight_ADC_VAL = 0;
                        Ui_Max_LMid_ADC_VAL = 0;
                        Ui_Max_RMid_ADC_VAL = 0;
                        LCD_P8x16Str(4,3,"ADC SAVE MODE");
                        delayms(1000);
                        LCD_CLS();
          }
		if (OP_TYPE == SUB)
			Ui_Save_Adc_Flag = false;
	}
//        else if (uiAdcPage.currentChosenPos == 14) {
//		if (OP_TYPE == ADD)
//			Ui_Adc_Valid_Threshold += 0.01f;
//		if (OP_TYPE == SUB && Ui_Adc_Valid_Threshold - 0.01f > 0)
//			Ui_Adc_Valid_Threshold -= 0.01f;
//	}
        else if (uiAdcPage.currentChosenPos == 16) {
          if (OP_TYPE == ADD)
          {
                  LQ_UART_Init(LPUART1, 115200);
                  LCD_CLS();
                  while(1){
                    unsigned char txt[16];
                    static int cnt = 0;
                    //uint32_t adcerr1 = 0;
                    //uint32_t Madcerr1 = 0;
                    //获得按键扫描结果,支持连按
                    unsigned char keyRes = KEY_Read(0);
                    Ui_Test_Adc_Flag = true;
                    
                    //deal_adc(&queue);
//                    updateLeftAdcVal();    
//                    updateRightAdcVal();
//                    updateMidLeftAdcVal();
//                    updateMidRightAdcVal();
//                    updateLMidAdcVal();
//                    updateRMidAdcVal();
                    
                    float sum34 = queue.once_uni_AD[2] + queue.once_uni_AD[3];
                    
                    float LM = queue.once_uni_AD[2] / sum34;
                    float RM = queue.once_uni_AD[3] / sum34;
                    
                    float sum16_34 = queue.once_uni_AD[0] + queue.once_uni_AD[2] + queue.once_uni_AD[3] + queue.once_uni_AD[5];
                    queue.twice_uni_AD[0] = queue.once_uni_AD[0] / sum16_34;
                    queue.twice_uni_AD[2] = queue.once_uni_AD[2] / sum16_34;
                    queue.twice_uni_AD[3] = queue.once_uni_AD[3] / sum16_34;
                    queue.twice_uni_AD[5] = queue.once_uni_AD[5] / sum16_34;
                    
                    float OutAdc = queue.twice_uni_AD[0] - queue.twice_uni_AD[5];
                    float InnerAdc = queue.twice_uni_AD[2] - queue.twice_uni_AD[3];
                    
                    //LCD_P6x8Str(4,0,"LQ ADC Test Bat"); 
                    sprintf(txt,"leftAdcVal:%d ",leftAdcVal);
                    LCD_P6x8Str(4,0,txt);
                    if(leftAdcVal > Ui_Max_Left_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_Left_ADC_VAL = leftAdcVal;
                  
                    sprintf(txt,"rightAdcVal:%d ",rightAdcVal);
                    LCD_P6x8Str(4,1,txt);
                    if(rightAdcVal > Ui_Max_Right_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_Right_ADC_VAL = rightAdcVal;
                  
                    sprintf(txt,"MidleftAdcVal:%d ",MidleftAdcVal);
                    LCD_P6x8Str(4,2,txt);
                    if(MidleftAdcVal > Ui_Max_MLeft_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_MLeft_ADC_VAL = MidleftAdcVal;
      
                    sprintf(txt,"MidrightAdcVal:%d ",MidrightAdcVal);
                    LCD_P6x8Str(4,3,txt);
                    if(MidrightAdcVal > Ui_Max_MRight_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_MRight_ADC_VAL = MidrightAdcVal;
                  
                    sprintf(txt,"LMidAdcVal:%d ",LMidAdcVal);
                    LCD_P6x8Str(4,4,txt);
                    if(LMidAdcVal > Ui_Max_LMid_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_LMid_ADC_VAL = LMidAdcVal;
                  
                    sprintf(txt,"RMidAdcVal:%d ",RMidAdcVal);
                    LCD_P6x8Str(4,5,txt);
                    if(RMidAdcVal > Ui_Max_RMid_ADC_VAL && Ui_Save_Adc_Flag)
                      Ui_Max_RMid_ADC_VAL = RMidAdcVal;
                    
                    float adcerr = getAdcError();
                    float LRAdcRate = queue.twice_uni_AD[2] + queue.twice_uni_AD[3] - queue.twice_uni_AD[0] - queue.twice_uni_AD[5];
                    float CrossVal = queue.once_uni_AD[1] * queue.once_uni_AD[4];
                    sprintf(txt,"CrossVal:%4.3f ",CrossVal);
                    LCD_P6x8Str(4,6,txt);
                    
                    float Madcerr = getMAdcError();
                    sprintf(txt,"DisErr:%4.3f ",queue.dis_err);
                    LCD_P6x8Str(4,7,txt);
                    
                    //printf("\n%f\n",LRAdcRate);
                    
                    //adcerr1 = adcerr1|adcerr;
                    //Madcerr1 = Madcerr1|Madcerr;
                    cnt++;
                    
                    if(cnt <=100){
                      printf("\n%f",InnerAdc);
                      printf("\n%f",OutAdc);
                      printf("\n%f",LM);
                      printf("\n%f",RM);

                      //printf("\r\n%f\n",queue.dis_err);
                      //UART_PutChar(LPUART1,adcerr1>>24);
                      //UART_PutChar(LPUART1,adcerr1>>16);
                      //UART_PutChar(LPUART1,adcerr1>>8);
                      //UART_PutChar(LPUART1,adcerr1&0xFF);
                      //UART_PutChar(LPUART1,Madcerr1>>24);
                      //UART_PutChar(LPUART1,Madcerr1>>16);
                      //UART_PutChar(LPUART1,Madcerr1>>8);
                      //UART_PutChar(LPUART1,Madcerr1&0xFF);
                    }
                    
                    //W25QXX_Write((uint8_t *)&data,8*1024*1024-1000,sizeof(data));
                    adcProcessAndGetValue();
//                    float adcr = Adc_Process_Result;
//                    sprintf(txt,"AdcServoVal:%4.3f ",adcr);
//                    LCD_P6x8Str(4,5,txt);
//                    
                    LQ_SetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL);
                    
                    if (keyRes == SUB){
                      cnt=0;
                      Ui_Test_Adc_Flag = false;
                      break;
                    }
                  }
          }
		if (OP_TYPE == SUB)
			Ui_Test_Adc_Flag = false;
	}
        else if (uiAdcPage.currentChosenPos == 17) {
          if (OP_TYPE == ADD)
          {
                  LCD_CLS();
                  while(1){
                    unsigned char txt[16];
                    //获得按键扫描结果,支持连按
                    unsigned char keyRes = KEY_Read(0);
                    Ui_Test_Adc_Flag = true;
                    
                    float sum34 = queue.once_uni_AD[2] + queue.once_uni_AD[3];
                    
                    float LM = queue.once_uni_AD[2] / sum34;
                    float RM = queue.once_uni_AD[3] / sum34;
                    
                    float sum16_34 = queue.once_uni_AD[0] + queue.once_uni_AD[2] + queue.once_uni_AD[3] + queue.once_uni_AD[5];
                    queue.twice_uni_AD[0] = queue.once_uni_AD[0] / sum16_34;
                    queue.twice_uni_AD[2] = queue.once_uni_AD[2] / sum16_34;
                    queue.twice_uni_AD[3] = queue.once_uni_AD[3] / sum16_34;
                    queue.twice_uni_AD[5] = queue.once_uni_AD[5] / sum16_34;
                    
                    float OutAdc = queue.twice_uni_AD[0] - queue.twice_uni_AD[5];
                    float InnerAdc = queue.twice_uni_AD[2] - queue.twice_uni_AD[3]; 
                    
                    float LRAdcRate = queue.twice_uni_AD[2] + queue.twice_uni_AD[3] - queue.twice_uni_AD[0] - queue.twice_uni_AD[5];
                    
                    float Mdis_Err = LM * Ui_fit_LM + Ui_fit_RM * RM;
                    sprintf(txt,"MdisErr:%4.3f ",Mdis_Err);
                    LCD_P6x8Str(4,0,txt);
                    
                    float CrossVal = queue.once_uni_AD[1] * queue.once_uni_AD[4];
                    sprintf(txt,"CrossVal:%4.3f ",CrossVal);
                    LCD_P6x8Str(4,1,txt);
                    
                    float Madcerr = getMAdcError();
                    sprintf(txt,"DisErr:%4.3f ",queue.dis_err);
                    LCD_P6x8Str(4,2,txt);
                    
                    sprintf(txt,"LRAdcRate:%4.3f ",LRAdcRate);
                    LCD_P6x8Str(4,3,txt);
                    
                    float sum25_34 = queue.once_uni_AD[1] + queue.once_uni_AD[4] + queue.once_uni_AD[2] + queue.once_uni_AD[3];
                    sprintf(txt,"sum2534:%4.3f ",sum25_34);
                    LCD_P6x8Str(4,4,txt);
                    
                    sprintf(txt,"sum34:%4.3f ",sum34);
                    LCD_P6x8Str(4,5,txt);
                    
                    float sum25 = queue.once_uni_AD[1] + queue.once_uni_AD[4];
                    sprintf(txt,"sum25:%4.3f ",sum25);
                    LCD_P6x8Str(4,6,txt);
                    
                    sprintf(txt,"sum1634:%4.3f ",sum16_34);
                    LCD_P6x8Str(4,7,txt);
                    
                    adcProcessAndGetValue();
                    LQ_SetServoDty(Adc_Process_Result + SERVO_MID_PWM_VAL);
                    
                    if (keyRes == SUB){
                      Ui_Test_Adc_Flag = false;
                      break;
                    }
                  }
          }
		if (OP_TYPE == SUB)
			Ui_Test_Adc_Flag = false;
	}
}

static void uiAdcPageDoDisplay(void) {
	if (uiAdcPage.currentChosenPos < 8) {
		sprintf(getOledBuf(0), "AdcSP %.2f", Ui_Adc_SP[Ui_Speed_Mode]);
		sprintf(getOledBuf(1), "AdcSD %.2f", Ui_Adc_SD[Ui_Speed_Mode]);
		sprintf(getOledBuf(2), "AdcBP %.2f", Ui_Adc_BP[Ui_Speed_Mode]);
		sprintf(getOledBuf(3), "AdcBD %.2f", Ui_Adc_BD[Ui_Speed_Mode]);
		sprintf(getOledBuf(4), "RampUpAdc %.2f", Ui_Ramp_Up_Adc);
		sprintf(getOledBuf(5), "AdcDMax %.2f", Ui_Adc_D_Max);
                sprintf(getOledBuf(6), "RingInAdc %.2f", Ui_Ring_In_Adc);
		sprintf(getOledBuf(7), "RingoutAdc %.2f", Ui_Ring_Out_Adc);
	}
        else if(uiAdcPage.currentChosenPos < 16)
        {
                sprintf(getOledBuf(0), "maxAdcL %.0f", Ui_Max_Left_ADC_VAL);
                sprintf(getOledBuf(1), "maxAdcR %.0f", Ui_Max_Right_ADC_VAL);
                sprintf(getOledBuf(2), "maxAdcML %.0f", Ui_Max_MLeft_ADC_VAL);
                sprintf(getOledBuf(3), "maxAdcMR %.0f", Ui_Max_MRight_ADC_VAL);
                sprintf(getOledBuf(4), "maxAdcLM %.0f", Ui_Max_LMid_ADC_VAL);
                sprintf(getOledBuf(5), "maxAdcRM %.0f", Ui_Max_RMid_ADC_VAL);
                sprintf(getOledBuf(6), "AdcThr %.2f", Ui_Adc_Thr);
                sprintf(getOledBuf(7), "AdcSaveMode");
		//sprintf(getOledBuf(8), " ");
		
        }
        else
        {
                //sprintf(getOledBuf(0), "AdcSaveMode");
                sprintf(getOledBuf(0), "TestAdc");
                sprintf(getOledBuf(1), "TestErr");
                for (int i = 2; i < 8; i++) {
			sprintf(getOledBuf(i), " ");
		}
        }

	for (int i = 0; i < 8; i++) {
		if (uiAdcPage.currentChosenPos % 8 == i)
			LCD_refreshOneLine(i, true);
		else
			LCD_refreshOneLine(i, false);
	}
}