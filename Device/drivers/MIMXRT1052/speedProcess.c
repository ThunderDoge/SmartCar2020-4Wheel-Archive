#include "speedProcess.h"
#include "angleProcess.h"
#include "adcprocess.h"
#include "pitimer.h"

#include "UIControl.h"

#include "sys.h"
#include "LQ_PWM.h"
#include "UIVar.h"

//extern bool Finish_Stop_Flag;
extern bool Out_Garage_Flag;
extern bool LeftRingFlag;
extern bool RightRingFlag;
extern bool Ramp_Flag;
extern bool Lost_Line_Flag;
extern bool Stop_Flag;
extern bool Laser_Finish_Flag;
extern bool Hall_Finish_Flag;

extern short Straightcnt;
extern uint16 temp3;
extern uint16 temp2;

extern Queue queue;
//left记忆值
volatile short leftUk_1 = 0, leftDeltaU;
volatile short leftEk, leftEk_1 = 0, leftEk_2 = 0;
//right记忆值
volatile short rightUk_1 = 0, rightDeltaU;
volatile short rightEk, rightEk_1 = 0, rightEk_2 = 0;

volatile uint16 purpostEncVal = 100;
volatile uint16 purpostEncValLeft = 100;
volatile uint16 purpostEncValRight = 100;

//车实际运行信息
volatile short currentSpeedLeft = 0;
volatile short currentSpeedRight = 0;
volatile int currentMileageLeft = 0;
volatile int currentMileageRight = 0;

//速度不可被updatePurpostEncVal更新标志位
bool fixedPurpostEncVal = false;
volatile short minFixedSpeed = 0;
volatile short maxFixedSpeed = 0;

void updateDifferentialSpeed(void);

int16_t getLeftUk() {
	currentSpeedLeft = queue.currentEncValLeft;
	currentMileageLeft += queue.currentEncValLeft;
	
	leftEk = purpostEncValLeft - queue.currentEncValLeft;
        //leftUk_1 = Ui_Motor_P * leftEk;
	//更新DeltaU
	leftDeltaU = Ui_Motor_P * (leftEk - leftEk_1) + Ui_Motor_I * leftEk + Ui_Motor_D * (leftEk - 2 * leftEk_1 + leftEk_2);
        /*------------------------遇限削弱抗积分饱和等处理-----------------------*/
//        if(leftUk_1 >= 200)
//          if(leftDeltaU > 0)
//            leftDeltaU = 0.0;//当前一时刻控制量(占空比)已经达到最大时，若现在增量为正则不累加
//        else if(leftUk_1 < -100)
//          if(leftDeltaU < 0)
//            leftDeltaU = 0.0;//当前一时刻控制量(占空比)已经达到最大时，若现在增量为正则不累加
	//更新Uk_1, Ek_1, Ek_2
	leftUk_1 = leftUk_1 + leftDeltaU;
	leftEk_2 = leftEk_1;
	leftEk_1 = leftEk;
	//Uk限幅
	if (leftUk_1 > 200) leftUk_1 = 200;//todo 可以考虑一下限幅的值的大小关系
	if (leftUk_1 < -200) leftUk_1 = -200;
	
	//bang-bang 或 PID
	if (leftEk < -Ui_MotorD_Threshold)
		return -200;
	else if (leftEk > Ui_Motor_Threshold)
		return 200;
	else
		return leftUk_1;
}
int16_t getRightUk() {
	currentSpeedRight = queue.currentEncValRight;
	currentMileageRight += queue.currentEncValRight;
	
	rightEk = purpostEncValRight - queue.currentEncValRight;
        //rightUk_1 = Ui_Motor_P * rightEk;
	//更新DeltaU
	rightDeltaU = Ui_Motor_P * (rightEk - rightEk_1) + Ui_Motor_I * rightEk + Ui_Motor_D * (rightEk - 2 * rightEk_1 + rightEk_2);
        /*------------------------遇限削弱抗积分饱和等处理-----------------------*/
//        if(rightUk_1 >= 200)
//          if(rightDeltaU > 0)
//            rightDeltaU = 0.0;//当前一时刻控制量(占空比)已经达到最大时，若现在增量为正则不累加
//        else if(rightUk_1 < -100)
//          if(rightDeltaU < 0)
//            rightDeltaU = 0.0;//当前一时刻控制量(占空比)已经达到最大时，若现在增量为正则不累加
	//更新Uk_1, Ek_1, Ek_2
	rightUk_1 = rightUk_1 + rightDeltaU;
	rightEk_2 = rightEk_1;
	rightEk_1 = rightEk;
	//Uk限幅
	if (rightUk_1 > 200) rightUk_1 = 200;
	if (rightUk_1 < -200) rightUk_1 = -200;
	
	//bang-bang 或 PID
	if (rightEk < -Ui_MotorD_Threshold)
		return -200;
	else if (rightEk > Ui_Motor_Threshold)
		return 200;
	else
		return rightUk_1; 
}

void updatePurpostEncVal() {
        if (Ui_Stop_Flag) {
		purpostEncVal = 0;
		purpostEncValLeft = 0;
		purpostEncValRight = 0;
                queue.Cal_Speed_Flag = true;

		return;
	}
        else if(Stop_Flag && !Laser_Finish_Flag && !Hall_Finish_Flag){
                purpostEncVal = 0;
		purpostEncValLeft = 0;
		purpostEncValRight = 0;
                return;
        }
        else if(Laser_Finish_Flag && (pitIsrCnt2 * 0.003 - temp3 * 0.003) < Ui_Stop_Time[Ui_Speed_Mode] && !Hall_Finish_Flag && (pitIsrCnt2 * 0.003 - temp3 * 0.003) > Ui_Stop_Delay_Time[Ui_Speed_Mode]){
                //purpostEncVal = 0;
		purpostEncValLeft = Ui_Stop_Left_Speed[Ui_Speed_Mode];//10
		purpostEncValRight = Ui_Stop_Right_Speed[Ui_Speed_Mode];//20
                
                return;
        }
        else if((pitIsrCnt2 * 0.003 - temp3 * 0.003) > Ui_Stop_Time[Ui_Speed_Mode] && !Hall_Finish_Flag)
        {
          purpostEncValLeft = 0;
          purpostEncValRight = 0;
          
          return;
        }
        else if(Hall_Finish_Flag && !Laser_Finish_Flag)
        {
          if(queue.currentEncValLeft > 5 && queue.currentEncValRight > 5)
          {
            temp3 = pitIsrCnt2;
            purpostEncValLeft = -100;
            purpostEncValRight = -100;
            return;
          }
          if((pitIsrCnt2 * 0.003 - temp3 * 0.003) > Ui_Back_Time[Ui_Speed_Mode])
          {
            purpostEncValLeft = Ui_Hall_Stop_Speed[Ui_Speed_Mode];
            purpostEncValRight = Ui_Hall_Stop_Speed[Ui_Speed_Mode];
            return;
          }
        }
        else if(Out_Garage_Flag){
                purpostEncValLeft = 60;
		purpostEncValRight = 60;
                
                return;
        }
        else if(Ramp_Flag && !LeftRingFlag && !RightRingFlag)
        {
                if(pitIsrCnt2 * 0.003 - temp2 * 0.003 < Ui_Ramp_Up_Time)
                {
                  purpostEncValLeft = Ui_Ramp_Up_Speed;
                  purpostEncValRight = Ui_Ramp_Up_Speed;
                  Straightcnt = 0;
                }
                else
                {
                  purpostEncValLeft = Ui_Ramp_Dowm_Speed;
                  purpostEncValRight = Ui_Ramp_Dowm_Speed;
                  Straightcnt = 0;
                }
                return;
        }
//        else if(!queue.Cal_Speed_Flag){
//          purpostEncValLeft = purpostEncVal;
//          purpostEncValRight = purpostEncVal;
//          
//          return;
//        }
        
      if(!Ui_Stop_Flag)// && (queue.Cal_Speed_Flag))
      {
	if (fixedPurpostEncVal) {
		//根据当前打角减速
		float decelerateK = (maxFixedSpeed - minFixedSpeed) / (float)SERVO_INTERVAL_PWM_VAL;
		//当前打角绝对值
		int16_t absServoDty = currentServoDty - SERVO_MID_PWM_VAL;
		absServoDty = (absServoDty > 0 ? absServoDty : (-absServoDty)); //SERVO_INTERVAL_PWM_VAL = 120
		short tmpPurpostEncVal = maxFixedSpeed - absServoDty * decelerateK;
	
		if (tmpPurpostEncVal < minFixedSpeed)
			tmpPurpostEncVal = minFixedSpeed;
	
		purpostEncVal = tmpPurpostEncVal;
                //purpostEncValLeft = tmpPurpostEncVal;
		//purpostEncValRight = tmpPurpostEncVal;
		//根据舵机打角差速
		updateDifferentialSpeed();

		return;
	}
        
        uint16  final_motor_value = 0;//记录均值滤波后的电机目标值
        float   maxErr = 0.0,maxErrD = 0.0;
        uint8 i;
        //static uint32 start_high_speed_cnt = 0;//计算距离，用于启动时高速模式计时用
        
        //if((pcb.car_state == Go_on) && (motor_info.en_speed_cal))
        //{ 
                /*-----------------------------------求400T内的最大偏差----------------------------------*/
            for(i=0;i<Ui_Find_Range;i++)  // 找出最近最大偏差 pre 15
              if((fabsf(queue.dis_err_fifo[i]))> (fabsf(maxErr)))
                maxErr=fabsf(queue.dis_err_fifo[i]);
            if(maxErr > Ui_Dis_Err_Max)
                maxErr =Ui_Dis_Err_Max;
            /*---------------------------------找最大偏差变化率，步长为130T,取10个当中的最大的---------------*/
            for(i=0;i<Ui_Find_Range;i++) // 找出最近最大偏差变化率 pre 15
              if((fabsf(queue.dis_err_fifo[i]-queue.dis_err_fifo[i+Ui_Find_Range]))> (fabsf(maxErrD)))//pre 12
                maxErrD=fabsf(queue.dis_err_fifo[i]-queue.dis_err_fifo[i+Ui_Find_Range]);
        
	//float decelerateK = (Ui_Max_Speed - Ui_Min_Speed) / Ui_Dis_Err_Max;//todo 速度范围乘以舵机移动百分比
        //short tmpPurpostEncVal = Ui_Max_Speed - maxErr * decelerateK;
            
//            float decelerateK = (Ui_Max_Speed - Ui_Min_Speed) / (float)SERVO_INTERVAL_PWM_VAL;
//            float decelerateK_1 = (Ui_Max_Speed - Ui_Min_Speed) / Ui_Dis_Err_Max;
//            int16_t absServoDty = currentServoDty - SERVO_MID_PWM_VAL;
//            float absErr = (queue.dis_err_fifo[0] > 0 ? queue.dis_err_fifo[0] : (-queue.dis_err_fifo[0]));
//            absServoDty = (absServoDty > 0 ? absServoDty : (-absServoDty)); //SERVO_INTERVAL_PWM_VAL = 120
//            short tmpPurpostEncVal = Ui_Max_Speed - absServoDty * decelerateK;
//            
//            if(absErr * 15 > absServoDty && !Lost_Line_Flag)
//              tmpPurpostEncVal = Ui_Max_Speed - absErr * decelerateK_1 * 1.25;
//	
//	if (tmpPurpostEncVal < Ui_Min_Speed)
//		tmpPurpostEncVal = Ui_Min_Speed;
//	
//	purpostEncVal = tmpPurpostEncVal;
            final_motor_value =(uint16)(
            (float)Ui_Max_Speed[Ui_Speed_Mode]
          - (float)(Ui_Max_Speed[Ui_Speed_Mode] - Ui_Cur_Speed[Ui_Speed_Mode]) * maxErr/Ui_Dis_Err_Max
          - (float)(Ui_Max_Speed[Ui_Speed_Mode] - Ui_SMin_Speed[Ui_Speed_Mode]) * maxErrD/3.0         //old_car4.0 new_car2.0
                                   );
            
            if(final_motor_value > Ui_Max_Speed[Ui_Speed_Mode])
              final_motor_value = Ui_Max_Speed[Ui_Speed_Mode];
            else if(final_motor_value < Ui_Min_Speed[Ui_Speed_Mode])
              final_motor_value = Ui_Min_Speed[Ui_Speed_Mode];
            
            purpostEncVal = final_motor_value;
        //purpostEncValLeft =tmpPurpostEncVal;
	//purpostEncValRight = tmpPurpostEncVal;
	/***************************/
	/***********差速************/
	/***************************/

	//根据舵机打角差速
	updateDifferentialSpeed();
      }
  
}

void updateDifferentialSpeed(void) {
  
         //固定圆环差速
         if (fixedPurpostEncVal) {
		short alpha = currentServoDty - SERVO_MID_PWM_VAL;
		
		short tmpPurpostEncValLeft;
		short tmpPurpostEncValRight;
		
		//差速的基准速度
//		short baseSpeed;
//		if (purpostEncVal <= 48)
//			baseSpeed = 48;
//		else
//			baseSpeed = purpostEncVal;
		
		if (alpha < 0) {//左转
			alpha = -alpha;
			tmpPurpostEncValLeft = purpostEncVal - 0.3 * alpha;
			tmpPurpostEncValRight = purpostEncVal;
		}
		else {
			tmpPurpostEncValLeft = purpostEncVal;
			tmpPurpostEncValRight = purpostEncVal - 0.3 * alpha;
		}
		
//		if (tmpPurpostEncValLeft > purpostEncVal)
//			tmpPurpostEncValLeft = purpostEncVal;
//		if (tmpPurpostEncValRight > purpostEncVal)
//				tmpPurpostEncValRight = purpostEncVal;
		
		purpostEncValLeft = tmpPurpostEncValLeft;
		purpostEncValRight = tmpPurpostEncValRight;
		
		return;
	} 
         
        //直道差速
	if (purpostEncVal > Ui_Min_Speed[Ui_Speed_Mode]) {
        //if(Bend_Diff_Flag){
		int alpha = currentServoDty - SERVO_MID_PWM_VAL;
//                float beta = queue.dis_err_fifo[0] * 12;
//                if(abs(alpha) < fabsf(beta))
//                  alpha = -fabsf(beta);
		if (alpha < 0) {//左转
			alpha = -alpha;
			purpostEncValLeft = purpostEncVal - Ui_Straight_Diff_K[Ui_Speed_Mode] * alpha;
			purpostEncValRight = purpostEncVal;
		}
		else {
			purpostEncValLeft = purpostEncVal;
			purpostEncValRight = purpostEncVal - Ui_Straight_Diff_K[Ui_Speed_Mode] * alpha;
		}
                
                return;
        }
        
//        if(Bend_Diff_Flag){
//		short alpha = currentServoDty - SERVO_MID_PWM_VAL;
//                //static float C =15.5 / (45*45);
//		if (alpha < 0) {//右转
//			alpha = -alpha;
//			purpostEncValLeft = Ui_Min_Speed - Ui_Straight_Diff_K * alpha; //* C;
//			purpostEncValRight = Ui_Min_Speed; //* C;
//		}
//		else {
//			purpostEncValLeft = Ui_Min_Speed; //* C;
//			purpostEncValRight = Ui_Min_Speed - Ui_Straight_Diff_K * alpha; //* C;
//		}
//                
//                return;
//        }
        
//        purpostEncValLeft = purpostEncVal;
//        purpostEncValRight = purpostEncVal;
        //}
            
//                short alpha = currentServoDty - SERVO_MID_PWM_VAL;;
//                //static float C =15.5 / (45*45);
//		if (alpha < 0) {//右转
//			alpha = -alpha;
//			purpostEncValLeft = purpostEncVal + Ui_Straight_Diff_K * alpha; //* C;
//			purpostEncValRight = purpostEncVal - Ui_Straight_Diff_K * alpha; //* C;
//		}
//		else {
//			purpostEncValLeft = purpostEncVal - Ui_Straight_Diff_K * alpha; //* C;
//			purpostEncValRight = purpostEncVal + Ui_Straight_Diff_K * alpha; //* C;
//		}

          
		//return;
	//}
        //}
        
        //purpostEncValLeft = purpostEncVal;
        //purpostEncValRight = purpostEncVal;
        
        //弯道差速(最大Error决定)
	int alpha = currentServoDty - SERVO_MID_PWM_VAL;
//        float beta = queue.dis_err_fifo[0] * 12;
//        if(abs(alpha) < fabsf(beta))
//          alpha = -fabsf(beta);
        
	if (alpha < 0) {//左转
		alpha = -alpha;
		if (Ui_Min_Speed[Ui_Speed_Mode] >= Ui_Inner_Max_Speed[Ui_Speed_Mode]) {
			purpostEncValLeft = Ui_Inner_Max_Speed[Ui_Speed_Mode] - Ui_Bend_Diff_K[Ui_Speed_Mode] * alpha;
			purpostEncValRight = Ui_Min_Speed[Ui_Speed_Mode];
		}
		else {
			purpostEncValLeft = Ui_Min_Speed[Ui_Speed_Mode] - Ui_Bend_Diff_K[Ui_Speed_Mode] * alpha;
			purpostEncValRight = Ui_Min_Speed[Ui_Speed_Mode];
		}
	}
	else {
		if (Ui_Min_Speed[Ui_Speed_Mode] >= Ui_Inner_Max_Speed[Ui_Speed_Mode]) {
			purpostEncValLeft = Ui_Min_Speed[Ui_Speed_Mode];
			purpostEncValRight = Ui_Inner_Max_Speed[Ui_Speed_Mode] - Ui_Bend_Diff_K[Ui_Speed_Mode] * alpha;
		}
		else {
			purpostEncValLeft = Ui_Min_Speed[Ui_Speed_Mode];
			purpostEncValRight = Ui_Min_Speed[Ui_Speed_Mode] - Ui_Bend_Diff_K[Ui_Speed_Mode] * alpha;
		}
	}
}

void setToFixedSpeed(short minSpeed, short maxSpeed) {
	//更新最大最小速度并置固定速度位为true
	minFixedSpeed = minSpeed;
	maxFixedSpeed = maxSpeed;
	fixedPurpostEncVal = true;
}

void cancelSetToFixedSpeed(void) {
	fixedPurpostEncVal = false;
}