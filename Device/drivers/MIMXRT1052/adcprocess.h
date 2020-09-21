#include "main.h"
#include "pitimer.h"
#ifndef __ADC_PROCESS_H__
#define __ADC_PROCESS_H__

extern volatile uint16_t leftAdcVal;
extern volatile uint16_t rightAdcVal;
extern volatile uint16_t MidleftAdcVal;
extern volatile uint16_t MidrightAdcVal;
extern volatile uint16_t LMidAdcVal;
extern volatile uint16_t RMidAdcVal;
extern volatile short Adc_Process_Result;
extern volatile short MAdc_Process_Result;
extern volatile float purpostAdcError;
extern void updateLeftAdcVal(void);

extern void updateRightAdcVal(void);

extern void updateMidLeftAdcVal(void);

extern void updateMidRightAdcVal(void);

extern void updateLMidAdcVal(void);

extern void updateRMidAdcVal(void);

extern float getAdcError(void);
extern float getMAdcError(void);
extern void adcProcessAndGetValue(void);
extern void MAdcProcessAndGetValue(void);
extern uint8_t Adc_Max_Position(void);

#define Left		1	//Adc最大返回值
#define MidLeft	        2       
#define Mid		3	
#define MidRight        4	
#define Right		5	

extern volatile bool Normal_Flag;
extern volatile bool Abnormal_Flag;
#endif