/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】坤哥
【版    本】V1.0
【注    意】本文件为坤哥代码移植，为图像处理文件，包括处理中线
【最后更新】2020年1月12日
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "szx_image.h"
#include "szx_color.h"
#include "myUtil.h"
//图像处理部分，看懂每一步的操作
volatile uint8 imageRGB[IMG_RGB_H][IMG_RGB_W][3]; //0-31 0-31 0-31 R G B 

/******************************/
/************中线**************/
/******************************/
volatile uint8 roadPointStackX[MAX_LINE_POINT_NUM];//volatile 表示该定义变量很可能发生变化（比如在其他中断程序中）
volatile uint8 roadPointStackY[MAX_LINE_POINT_NUM];//该变量的值要从内存中读取
volatile short stackTopPos;                     //栈顶
volatile bool isVisited[IMG_RGB_H][IMG_RGB_W];  //ij点是否被访问过

volatile short leftLine[IMG_RGB_H]; //储存第i行的左边线点是第j个（j代表列数）
volatile short rightLine[IMG_RGB_H];
volatile short centerLine[IMG_RGB_H];
volatile bool leftValid[IMG_RGB_H]; //储存第i行是否有有效左边线点
volatile bool rightValid[IMG_RGB_H];
volatile bool centerValid[IMG_RGB_H];

volatile bool leftIsLost;  
volatile bool rightIsLost;

volatile bool CENTER_UNFOUND;

short myMax(short a, short b);
inline short myMax(short a, short b) {
	return (a > b ? a : b);
}
//图像处理各参数初始化
void imageProcessParamsInit(void) {
	colorSegmentationInit();

	stackTopPos = -1;
	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			isVisited[i][j] = false;//stdbool.h中将false定义为bool型变量0，true为bool型变量1
		}                         //是否访问过ij，初始化为0
	}

	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		leftValid[i] = rightValid[i] = centerValid[i] = false;//左边线有效，右边线有效，中线有效初始化为0
	}
}

//brokenAngleJudge中得到的结果
volatile bool originLineResValid = false;
volatile bool originLeftLineStraight = false;
volatile float originLeftLineK = 0;
volatile float originLeftLineB = 0;
volatile bool originRightLineStraight = false;
volatile float originRightLineK = 0;
volatile float originRightLineB = 0;

void roadPointDfs(void);
bool isLeftPoint(int i, int j);
bool isRightPoint(int i, int j);
bool isEdgePoint(int i, int j);
void doFliter(void);
void repairStartOfEdge(void);
void repairEndOfEdge(void);
void centerLimitFilter(int i, short value);
void repairWholeLine(void);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//需要image,依赖isLeftPoint,isRightPoint 寻找中线
void findCenterLine(void) {
	//DFS 深度优先搜索左右边线
	roadPointDfs();
	//鉴定丢线与否,未丢线时过滤
	doFliter();
	//修复起始丢线
	repairStartOfEdge();
	//TODO 当左线变化趋势为一致向右/右线变化趋势一致为向左时,令该线末端以SobelY为依据生长
	//前提是DFS中寻到的未经过滤的边线不能是直线
	if (originLineResValid && !originLeftLineStraight && !originRightLineStraight)
		repairEndOfEdge();

	if (leftIsLost && rightIsLost) {
		CENTER_UNFOUND = true;

		return;
	}
	CENTER_UNFOUND = false;
	if (leftIsLost) {
		//计算中线补偿
		float validCount = 0;
		short sumError = 0;
		float stdPoint;
		float actualPoint;
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (rightValid[i]) {
				stdPoint = (IMG_RGB_W / 2 + roadLenK / 2 * i + roadLenB / 2);
				actualPoint = rightLine[i];
				if (stdPoint < IMG_RGB_W && actualPoint < IMG_RGB_W) {
					validCount++;
					sumError += (stdPoint - actualPoint);
				}
			}
		}
		//个数足够,总Error在左
		float centerLineFix = 0;
		if (validCount >= 5 && sumError > 0) {
			float aveError = sumError / validCount;
			if (aveError > roadMoreMaxAveErrorLostLine) aveError = roadMoreMaxAveErrorLostLine;
			centerLineFix = aveError / roadMoreMaxAveErrorLostLine * roadMoreLostLine;
		}

		//补线
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (rightValid[i]) {
				centerLimitFilter(i, rightLine[i] - roadLenK / 2 * i - roadLenB / 2 - centerLineFix);
			}
		}


		repairWholeLine();
	}
	else if (rightIsLost) {
		//计算中线补偿
		float validCount = 0;
		short sumError = 0;
		float stdPoint;
		float actualPoint;
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (leftValid[i]) {
				stdPoint = (IMG_RGB_W / 2 - roadLenK / 2 * i - roadLenB / 2);
				actualPoint = leftLine[i];
				if (stdPoint > 0 && actualPoint > 0) {
					validCount++;
					sumError += (actualPoint - stdPoint);
				}
			}
		}
		//个数足够,总Error向右
		float centerLineFix = 0;
		if (validCount >= 5 && sumError > 0) {
			float aveError = sumError / validCount;
			if (aveError > roadMoreMaxAveErrorLostLine) aveError = roadMoreMaxAveErrorLostLine;
			centerLineFix = aveError / roadMoreMaxAveErrorLostLine * roadMoreLostLine;
		}

		//补线
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (leftValid[i]) {
				centerLimitFilter(i, leftLine[i] + roadLenK / 2 * i + roadLenB / 2 + centerLineFix);
			}
		}

		repairWholeLine();
	}
	else if (!leftIsLost && !rightIsLost) {
		//道路有效长度过滤
		uint8 minRoadLen = IMG_RGB_W / 6;

		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (leftValid[i] && rightValid[i]) {
				//道路有效长度过滤
				if (rightLine[i] - leftLine[i] < minRoadLen) continue;

				//道路长度不过长则均值,否则取界内
				//				if (rightLine[i] - leftLine[i] > (roadLenK * i + roadLenB)) {
				//					if (leftLine[i] > 0 && leftLine[i] < IMG_RGB_W)
				//						centerLimitFilter(i, leftLine[i] + roadLenK / 2 * i + roadLenB / 2);
				//					else if (rightLine[i] > 0 && rightLine[i] < IMG_RGB_W)
				//						centerLimitFilter(i, rightLine[i] - roadLenK / 2 * i - roadLenB / 2);
				//				}
				//				else
				centerLimitFilter(i, (leftLine[i] + rightLine[i]) / 2);
			}
			else if (leftValid[i] && i < IMG_RGB_H / 2) {
				centerLimitFilter(i, leftLine[i] + roadLenK / 2 * i + roadLenB / 2);
			}
			else if (rightValid[i] && i < IMG_RGB_H / 2) {
				centerLimitFilter(i, rightLine[i] - roadLenK / 2 * i - roadLenB / 2);
			}
		}

		//修复中线
		repairWholeLine();

		//修复中线不成功
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (!centerValid[i])
				centerValid[i] = true;
		}
	}
}

void roadPointDfs(void) {//深度优先搜索
	//起始点（28，20）
	if (isWhite(IMG_RGB_H - 2, IMG_RGB_W / 2)) { //判断越界保护 并且看是否是白色，如果是白色，返回的白色缓冲记为1，则进行以下步骤
		                                           //否则可能为蓝或红，进行else操作
		stackTopPos++;                             //栈顶++，第一次变为0，推测是起始点的rgb数据入栈                           
		roadPointStackX[stackTopPos] = IMG_RGB_H - 2;      //道路点的堆栈
		roadPointStackY[stackTopPos] = IMG_RGB_W / 2;

		isVisited[IMG_RGB_H - 2][IMG_RGB_W / 2] = true;    //这一点就已经被访问过了
	}
	else {                                        //如果不是白色
		for (uint8 j = 2; j < IMG_RGB_W - 2; j++) {                         //注意 每一行从第0列开始 这里28行从2开始累加到19 //判断28行的第2列的左右各两列是否全是白色，如果是，（28，2）入栈
			if (isWhite(IMG_RGB_H - 2, j - 2) && isWhite(IMG_RGB_H - 2, j - 1)&& isWhite(IMG_RGB_H - 2, j) && isWhite(IMG_RGB_H - 2, j + 1) && isWhite(IMG_RGB_H - 2, j + 2)) 
                        {
				
				stackTopPos++;                                                  //栈顶++
				roadPointStackX[stackTopPos] = IMG_RGB_H - 2;
				roadPointStackY[stackTopPos] = j;

				isVisited[IMG_RGB_H - 2][j] = true;                             //已访问

				break;
			}
		}
	}

	uint8 i, j;
	while (stackTopPos >= 0) {
		i = roadPointStackX[stackTopPos];                                    //i为行数，j为列数 堆栈中为白色点
		j = roadPointStackY[stackTopPos];
		stackTopPos--;                                                       //出栈

		//处理出界
		if (!(i >= EFFECTIVE_ROW && i <= IMG_RGB_H - 2 && j >= 1 && j <= IMG_RGB_W - 2)) continue;//如果出界直接跳过本轮while循环
		//遇白Dfs,遇黑检查边界点

		//左
		if (!isVisited[i][j - 1]) {                  //没搜索过ij的前一个点
			if (isWhite(i, j - 1)) {                   //如果是i，j-1是白色的
				stackTopPos++;                           //入栈
				roadPointStackX[stackTopPos] = i;
				roadPointStackY[stackTopPos] = j - 1;

				//标记已访问
				isVisited[i][j - 1] = true;
			}
			else {                                      //如果是i，j-1不是白色点 检查i，j是否是边线点
				if (isLeftPoint(i, j)) {                  //判断左边线
					leftLine[i] = j;                        //第i行的左边线点为j
					leftValid[i] = true;                    //第i行有左边线了
				}
			}
		}

		//右
		if (!isVisited[i][j + 1]) {
			if (isWhite(i, j + 1)) {
				stackTopPos++;
				roadPointStackX[stackTopPos] = i;
				roadPointStackY[stackTopPos] = j + 1;

				//标记已访问
				isVisited[i][j + 1] = true;
			}
			else {
				if (isRightPoint(i, j)) {
					rightLine[i] = j;
					rightValid[i] = true;
				}
			}
		}

		//上
		if (!isVisited[i - 1][j]) {
			if (isWhite(i - 1, j)) {
				stackTopPos++;
				roadPointStackX[stackTopPos] = i - 1;
				roadPointStackY[stackTopPos] = j;

				//标记已访问
				isVisited[i - 1][j] = true;
			}
		}
	}
}

bool isLeftPoint(int i, int j) { //左边线左边应当还有黑色点，右边应该没有白色点 ij从堆栈中提出，本来就是白色
	//越界保护
	if (j < 3 || j >= IMG_RGB_W - 3) return false;
	//非白 该点及其右边应该都是白色
	if ((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2))) return false;
	//左无黑
	if ((!isBlueOrBlack(i, j - 3)) && (!isBlueOrBlack(i, j - 2)) && (!isBlueOrBlack(i, j - 1))) return false;
	return true;
}

bool isRightPoint(int i, int j) { //右边线右边还有黑色点，左边无白色点，ij本来就是白色 提取出的边线点都是白色的
	//越界保护
	if (j < 3 || j >= IMG_RGB_W - 3) return false;
	//非白
	if ((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2))) return false;
	//右无黑
	if ((!isBlueOrBlack(i, j + 3)) && (!isBlueOrBlack(i, j + 2)) && (!isBlueOrBlack(i, j + 1))) return false;
	return true;
}

bool isEdgePoint(int i, int j) {
	//越界保护
	if (i < 3 || i >= IMG_RGB_H - 3 || j < 3 || j >= IMG_RGB_W - 3) return false;
	//非白
	if (!isWhite(i, j)) return false;
	//上左右下都非黑
	if ((!isBlueOrBlack(i, j + 2))
		&& (!isBlueOrBlack(i, j - 2))
		&& (!isBlueOrBlack(i - 2, j))
		&& (!isBlueOrBlack(i + 2, j))
		)
		return false;

	return true;
}

void lostLineJudge(void) {
	//少于LOST_LINE_THRESHOLD个直接认为丢线  有边线的行数至少大于4
	uint8 validCount = 0;//有效计数
	//检验左边线
	for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) { //i=1
		if (leftValid[i])
			validCount++;
	}
	if (validCount < LOST_LINE_THRESHOLD) {
		leftIsLost = true;

		for (uint8 i = 0; i < IMG_RGB_H; i++)
			leftValid[i] = false;
	}
	else {
		leftIsLost = false; //左边线没丢
	}
  //检验右边线
	validCount = 0;
	for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (rightValid[i])
			validCount++;
	}
	if (validCount < LOST_LINE_THRESHOLD) {
		rightIsLost = true;

		for (uint8 i = 0; i < IMG_RGB_H; i++)
			rightValid[i] = false;
	}
	else {
		rightIsLost = false;
	}
}

volatile short isBrokenRoadAngleCount = 0;
void brokenRoadAngleJudge(void) {
	/***************防断路角**************/
	float kLeft, bLeft, kRight, bRight;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//拟合左 最小二乘法 左边线 y=kx+b
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (leftValid[i]) {          //如果第i行有有效左边线点，则执行以下操作
			sumXY += i * leftLine[i];  //X,Y求和就是左边线点横纵坐标乘积
			sumX += i;
			sumY += leftLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= LOST_LINE_THRESHOLD) {  //丢线点阈值4
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kLeft = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bLeft = averageY - kLeft * averageX;
	}
	else {
		leftIsLost = true;
	}

	//拟合右
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (rightValid[i]) {
			sumXY += i * rightLine[i];
			sumX += i;
			sumY += rightLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= LOST_LINE_THRESHOLD) {
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kRight = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bRight = averageY - kRight * averageX;
	}
	else {
		rightIsLost = true;
	}

	//如果丢一边,先降低寻线条件,出函数时还原原状态，就不走前面的判断是否访问的程序，直接从右边线点向左走，看左右三个点的情况
	bool setLeftIsLost = false, setRightIsLost = false;
	if (leftIsLost && !rightIsLost) {
		setLeftIsLost = true;  //左线丢了

		//从右线找左线
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H - 2; i++) {
			if (rightValid[i]) {
				for (short j = rightLine[i] - 1; j >= 0; j--) {
					if (isLeftPoint(i, j)) {     //重新找一遍左边线点
						leftValid[i] = true;
						leftLine[i] = j;
						break;
					}
				}
			}
		}

		leftIsLost = false;

		//拟合左
		sumXY = sumX = sumY = sumX2 = 0;
		validCount = 0;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			if (leftValid[i]) {
				sumXY += i * leftLine[i];
				sumX += i;
				sumY += leftLine[i];
				sumX2 += i * i;

				validCount++;
			}
		}

		if (validCount >= LOST_LINE_THRESHOLD) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			kLeft = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bLeft = averageY - kLeft * averageX;
		}
		else {
			leftIsLost = true;
		}
	}
	//右线丢了
	else if (rightIsLost && !leftIsLost) {
		setRightIsLost = true;

		//从左线找右线
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H - 2; i++) {
			if (leftValid[i]) {
				for (short j = leftLine[i] + 1; j < IMG_RGB_W; j++) {
					if (isRightPoint(i, j)) {
						rightValid[i] = true;
						rightLine[i] = j;
						break;
					}
				}
			}
		}

		rightIsLost = false;

		//拟合右
		sumXY = sumX = sumY = sumX2 = 0;
		validCount = 0;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			if (rightValid[i]) {
				sumXY += i * rightLine[i];
				sumX += i;
				sumY += rightLine[i];
				sumX2 += i * i;

				validCount++;
			}
		}

		if (validCount >= LOST_LINE_THRESHOLD) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			kRight = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bRight = averageY - kRight * averageX;
		}
		else {
			rightIsLost = true;
		}
	}

	//如果左右未丢失
	if (!leftIsLost && !rightIsLost) {
		//拟合误差判据
		float averageFitErrorLeft = 0, averageFitErrorRight = 0;
		short leftCount = 0, rightCount = 0;
		float fitValue;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) { //拟合出的边线与实际边线的误差计算
			if (leftValid[i]) {
				fitValue = kLeft * i + bLeft;
				averageFitErrorLeft += ((leftLine[i] - fitValue) * (leftLine[i] - fitValue));
				leftCount++;
			}
			if (rightValid[i]) {
				fitValue = kRight * i + bRight;
				averageFitErrorRight += ((rightLine[i] - fitValue) * (rightLine[i] - fitValue));
				rightCount++;
			}
		}
		averageFitErrorLeft /= leftCount;//左边线每点平均拟合误差
		averageFitErrorRight /= rightCount;

		//将拟合误差结果保存
		originLineResValid = true;
		if (averageFitErrorLeft <= (1 * 1)) {
			originLeftLineStraight = true; //拟合成功
			originLeftLineK = kLeft;
			originLeftLineB = bLeft;
		}
		else
			originLeftLineStraight = false;//拟合失败
		if (averageFitErrorRight <= (1 * 1)) {
			originRightLineStraight = true;
			originRightLineK = kRight;
			originRightLineB = bRight;
		}
		else
			originRightLineStraight = false;

		//任一拟合误差大于给定值,认为不是直线 //actualCenterLineDfs 1325
		if ((averageFitErrorLeft <= (1 * 1) && averageFitErrorRight <= (5 * 5))
			|| (averageFitErrorLeft <= (5 * 5) && averageFitErrorRight <= (1 * 1))
			) {
			//都是直线,则算交叉点  由于为广角镜头 左右两条边线不断延长，最终交于极远处
			float xPoint = (bRight - bLeft) / (kLeft - kRight);
			float yPoint = kLeft * xPoint + bLeft;

			//如果交叉点x坐标太小 找错边线重新找 坐标原点在左上角
			if (xPoint > IMG_RGB_H / 8) {
				isBrokenRoadAngleCount++;

				for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
					leftValid[i] = rightValid[i] = false;
				}
				leftIsLost = rightIsLost = true;

				return;
			}
		}
	}
	else if (!leftIsLost) {
		//只计算左线误差并保存
		float averageFitErrorLeft = 0;
		short leftCount = 0;
		float fitValue;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			if (leftValid[i]) {
				fitValue = kLeft * i + bLeft;
				averageFitErrorLeft += ((leftLine[i] - fitValue) * (leftLine[i] - fitValue));
				leftCount++;
			}
		}
		averageFitErrorLeft /= leftCount;

		//将拟合误差结果保存
		originLineResValid = true;
		if (averageFitErrorLeft <= (1 * 1)) {
			originLeftLineStraight = true;
			originLeftLineK = kLeft;
			originLeftLineB = bLeft;
		}
		else
			originLeftLineStraight = false;
		originRightLineStraight = false;
	}
	else if (!rightIsLost) {
		//只计算右线误差并保存
		float averageFitErrorRight = 0;
		short rightCount = 0;
		float fitValue;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			if (rightValid[i]) {
				fitValue = kRight * i + bRight;
				averageFitErrorRight += ((rightLine[i] - fitValue) * (rightLine[i] - fitValue));
				rightCount++;
			}
		}
		averageFitErrorRight /= rightCount;

		//将拟合误差结果保存
		originLineResValid = true;
		originLeftLineStraight = false;
		if (averageFitErrorRight <= (1 * 1)) {
			originRightLineStraight = true;
			originRightLineK = kRight;
			originRightLineB = bRight;
		}
		else
			originRightLineStraight = false;
	}
	else {
		originLineResValid = false;
	}

	if (setLeftIsLost) {
		leftIsLost = true;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			leftValid[i] = false;
		}
	}

	if (setRightIsLost) {
		rightIsLost = true;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
			rightValid[i] = false;
		}
	}

	//前面未return,说明不是断路角
	isBrokenRoadAngleCount = 0;
}

void crossLeftLineJudge(void) {
	//求左线最右点，若最右点未超过1/3则不检查
	short leftLineMaxPoint = 0;
	for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (leftValid[i] && leftLine[i] > leftLineMaxPoint)
			leftLineMaxPoint = leftLine[i];
	}
	if (leftLineMaxPoint < IMG_RGB_W / 3)
		return;

	//找左线拐点1
	short leftInflectionPoint = EFFECTIVE_ROW;
	short leftInflectionPoint2 = EFFECTIVE_ROW;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {//从下往上找
		if (leftValid[i]) {
			//往下找最近有效点 这里的j不表示列数
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//找到最近有效点
				if (leftValid[j]) {
					//找到拐点
					if (leftLine[i] < leftLine[j])//根据广角镜头成像原理，上面一行左边线点的列数必大于下一行，故当小于时发生拐弯
						leftInflectionPoint = i;    //拐点所在行
					break;
				}
			}
		}

		//如果已找到拐点,break
		if (leftInflectionPoint != EFFECTIVE_ROW)
			break;
	}

	//找左线拐点2  从第一拐点往上找
	for (short i = leftInflectionPoint; i >= EFFECTIVE_ROW; i--) {//////////////////////？？？？？？
		if (leftValid[i]) {
			//向上找最近有效点
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//往下找到最近有效点
				if (leftValid[j]) {
					//找到拐点2
					if (leftLine[i] > leftLine[j])
						leftInflectionPoint2 = i;
					break;
				}
			}
		}

		//如果已找到拐点2,break
		if (leftInflectionPoint2 != EFFECTIVE_ROW)
			break;
	}

	//分别对两段做直线拟合 拐点以下的点进行直线拟合
	float kLeft1, bLeft1, kLeft2, bLeft2;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//拟合[leftInflectionPoint + 1, IMG_RGB_H - 1]  第一拐点下一行到末尾
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (short i = leftInflectionPoint + 1; i < IMG_RGB_H; i++) {
		if (leftValid[i]) {
			sumXY += i * leftLine[i];
			sumX += i;
			sumY += leftLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= CROSS_EDGE_LEN) {
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kLeft1 = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bLeft1 = averageY - kLeft1 * averageX;
	}
	else
		return;

	//拟合[leftInflectionPoint2 + 1, leftInflectionPoint] //从第二拐点下一行到第一拐点
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (short i = leftInflectionPoint2 + 1; i <= leftInflectionPoint; i++) {//第二拐点在第一拐点上面
		if (leftValid[i]) {
			sumXY += i * leftLine[i];
			sumX += i;
			sumY += leftLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= CROSS_EDGE_LEN) { //第二拐点在第一拐点上面超过4行
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kLeft2 = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bLeft2 = averageY - kLeft2 * averageX;
	}
	else
		return;

	//前面未return，说明上下段都拟合成功，计算拟合误差
	float averageFitError1 = 0, averageFitError2 = 0;
	short count1 = 0, count2 = 0;
	float fitValue;
	for (uint8 i = leftInflectionPoint2 + 1; i < IMG_RGB_H; i++) {
		if (leftValid[i]) {
			if (i > leftInflectionPoint) {
				fitValue = kLeft1 * i + bLeft1;
				averageFitError1 += ((leftLine[i] - fitValue) * (leftLine[i] - fitValue));
				count1++;
			}
			else {
				fitValue = kLeft2 * i + bLeft2;
				averageFitError2 += ((leftLine[i] - fitValue) * (leftLine[i] - fitValue));
				count2++;
			}
		}
	}
	averageFitError1 /= count1;
	averageFitError2 /= count2;

	//两段都是直线，且段1向右段2向左
	if (
		((averageFitError1 <= 4 && averageFitError2 <= 1) || (averageFitError1 <= 1 && averageFitError2 <= 4))
		&& ((kLeft1 < -2 && kLeft2 > 1) || (kLeft1 < 0 && kLeft2 > 2.5))) {
		for (short i = EFFECTIVE_ROW; i <= leftInflectionPoint; i++) //有十字串路，故第一拐点以上部分的边线均丢
			leftValid[i] = false;
		for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++)
			rightValid[i] = false;
		rightIsLost = true;
	}
}

void crossRightLineJudge(void) {
	//求右线最左点，若最左点未超过1/3则不检查
	short rightLineMinPoint = IMG_RGB_W - 1;
	for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (rightValid[i] && rightLine[i] < rightLineMinPoint)
			rightLineMinPoint = rightLine[i];
	}
	if (rightLineMinPoint > (IMG_RGB_W * 2 / 3))
		return;

	//找右线拐点1
	short rightInflectionPoint = EFFECTIVE_ROW;
	short rightInflectionPoint2 = EFFECTIVE_ROW;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
		if (rightValid[i]) {
			//向下找最近有效点
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//找到最近有效点
				if (rightValid[j]) {
					//找到拐点
					if (rightLine[i] > rightLine[j])
						rightInflectionPoint = i;
					break;
				}
			}
		}

		//如果已找到拐点,break
		if (rightInflectionPoint != EFFECTIVE_ROW)
			break;
	}

	//找右线拐点2
	for (short i = rightInflectionPoint; i >= EFFECTIVE_ROW; i--) {
		if (rightValid[i]) {
			//向下找最近有效点
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//找到最近有效点
				if (rightValid[j]) {
					//找到拐点2
					if (rightLine[i] < rightLine[j])
						rightInflectionPoint2 = i;
					break;
				}
			}
		}

		//如果已找到拐点2,break
		if (rightInflectionPoint2 != EFFECTIVE_ROW)
			break;
	}

	//分别对两段做直线拟合
	float kRight1, bRight1, kRight2, bRight2;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//拟合[rightInflectionPoint + 1, IMG_RGB_H - 1]
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (short i = rightInflectionPoint + 1; i < IMG_RGB_H; i++) {
		if (rightValid[i]) {
			sumXY += i * rightLine[i];
			sumX += i;
			sumY += rightLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= CROSS_EDGE_LEN) {
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kRight1 = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bRight1 = averageY - kRight1 * averageX;
	}
	else
		return;

	//拟合[rightInflectionPoint2 + 1, rightInflectionPoint]
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (short i = rightInflectionPoint2 + 1; i <= rightInflectionPoint; i++) {
		if (rightValid[i]) {
			sumXY += i * rightLine[i];
			sumX += i;
			sumY += rightLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= CROSS_EDGE_LEN) {
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kRight2 = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bRight2 = averageY - kRight2 * averageX;
	}
	else
		return;

	//前面未return，说明上下段都拟合成功，计算拟合误差
	float averageFitError1 = 0, averageFitError2 = 0;
	short count1 = 0, count2 = 0;
	float fitValue;
	for (uint8 i = rightInflectionPoint2 + 1; i < IMG_RGB_H; i++) {
		if (rightValid[i]) {
			if (i > rightInflectionPoint) {
				fitValue = kRight1 * i + bRight1;
				averageFitError1 += ((rightLine[i] - fitValue) * (rightLine[i] - fitValue));
				count1++;
			}
			else {
				fitValue = kRight2 * i + bRight2;
				averageFitError2 += ((rightLine[i] - fitValue) * (rightLine[i] - fitValue));
				count2++;
			}
		}
	}
	averageFitError1 /= count1;
	averageFitError2 /= count2;

	//两段都是直线，且段1向左段2向右
	if (
		((averageFitError1 <= 4 && averageFitError2 <= 1) || (averageFitError1 <= 1 && averageFitError2 <= 4))
		&& ((kRight1 > 2 && kRight2 < -1) || (kRight1 > 0 && kRight2 < -2.5))) {
		for (short i = EFFECTIVE_ROW; i <= rightInflectionPoint; i++)
			rightValid[i] = false;
		for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++)
			leftValid[i] = false;
		leftIsLost = true;
	}
}

void crossJudge(void) {
	/***************防十字串道**************/ //TODO check
										 //检查左线
	crossLeftLineJudge();
	//检查右线
	crossRightLineJudge();
}


//鉴定丢线与否,未丢线时过滤
void doFliter(void) {
	lostLineJudge();//判断是否丢线
//	brokenRoadAngleJudge();//断路 判断
//	crossJudge();  //防十字串道
	////从IMG_RGB_H - 2开始
	/*******如果左右不丢线,则过滤掉越过中间的点****/
	if (!leftIsLost && !rightIsLost) {
		for (short i = IMG_RGB_H - 2; i >= 0; i--) {
			if (leftValid[i] && leftLine[i] > IMG_RGB_W / 2)//说明边线点越过中线，不可能
				leftValid[i] = false;
			if (rightValid[i] && rightLine[i] < IMG_RGB_W / 2)
				rightValid[i] = false;
		}
	}

	//if (leftIsLost || rightIsLost) return;
	short k;

	for (short i = IMG_RGB_H - 2; i >= 0; i--) {
		if (leftValid[i]) {
			//向下找第一个有效点
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				if (leftValid[j]) {
					//找到有效点,计算斜率（其实不是斜率）
					k = (leftLine[i] - leftLine[j]) / (i - j);
					//左减小不保留该点
					if (k > 0) {
						//两点一起去掉?
						leftValid[i] = false;
						leftValid[j] = false;
					}
					//差距 （斜率）过大不保留该点
					if (k < 0) k = -k;//将负数变为正数
					if (k > FILTER_K_THRESHOLD) { //TODO 影响了大弯边线的识别 过滤阈值6
												  //两点一起去掉?
						leftValid[i] = false;
						leftValid[j] = false;
					}
					break;
				}
			}
		}
	}

	for (short i = IMG_RGB_H - 2; i >= 0; i--) {
		if (rightValid[i]) {
			//向下找第一个有效点
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				if (rightValid[j]) {
					//找到有效点,计算斜率
					k = (rightLine[i] - rightLine[j]) / (i - j);
					//右增大不保留该点
					if (k < 0) {
						//两点一起去掉?
						rightValid[i] = false;
						rightValid[j] = false;
					}
					//差距过大不保留该点
					if (k < 0) k = -k;
					if (k > FILTER_K_THRESHOLD) {
						//两点一起去掉?
						rightValid[i] = false;
						rightValid[j] = false;
					}
					break;
				}
			}
		}
	}

	lostLineJudge();
}

void repairStartOfEdge(void) {
	float aLeft, bLeft, aRight, bRight;
	bool leftExist = false, rightExist = false;

	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;
	/************拟合左***********/
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//找到左起始
	uint8 leftStart = 0;
	for (short i = IMG_RGB_H - 1; i >= 0; i--) {
		if (leftValid[i]) {
			leftStart = i;
			break;//跳出整个for循环
		}
	}
	//左起始超过1/6,不补线
	if (leftStart > IMG_RGB_H / 6) {
		//在左起始距离为15以内找5个点
		for (short i = leftStart; i >= myMax(leftStart - 15, 0); i--) {  //取二者中大的值
			if (leftValid[i]) {
				sumXY += i * leftLine[i];
				sumX += i;
				sumY += leftLine[i];
				sumX2 += i * i;

				validCount++;
			}
			if (validCount == 5) break;
		}

		//如果找得到5个点
		if (validCount == 5) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			aLeft = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bLeft = averageY - aLeft * averageX;
			leftExist = true;
		}
	}

	/************拟合右***********/
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//找到右起始
	uint8 rightStart = 0;
	for (short i = IMG_RGB_H - 1; i >= 0; i--) {
		if (rightValid[i]) {
			rightStart = i;
			break;
		}
	}
	//右起始超过1/6不补线
	if (rightStart > IMG_RGB_H / 6) {
		//在右起始距离为15以内找5个点
		for (short i = rightStart; i >= myMax(rightStart - 15, 0); i--) {
			if (rightValid[i]) {
				sumXY += i * rightLine[i];
				sumX += i;
				sumY += rightLine[i];
				sumX2 += i * i;

				validCount++;
			}
			if (validCount == 5) break;
		}

		//如果找得到5个点
		if (validCount == 5) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			aRight = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bRight = averageY - aRight * averageX;
			rightExist = true;
		}
	}

	/************填入左右预测值***********/ 
	if (leftExist) {                       //根据曲线拟合补边线
		for (short i = IMG_RGB_H - 1; i >= leftStart; i--) {
			if (!leftValid[i]) {
				leftValid[i] = true;
				leftLine[i] = aLeft * i + bLeft;
			}
		}
	}
	if (rightExist) {
		for (short i = IMG_RGB_H - 1; i >= rightStart; i--) {
			if (!rightValid[i]) {
				rightValid[i] = true;
				rightLine[i] = aRight * i + bRight;
			}
		}
	}
}

void repairEndOfEdge(void) {
	//左线是否一直向右?数量是否足够?记录最末行
	bool isLeftLineAllToRight = true; uint8 leftValidCount = 0; uint8 minLeftRow = IMG_RGB_H - 1;
	//右线是否一直向左?数量是否足够?记录最末行
	bool isRightLineAllToLeft = true; uint8 rightValidCount = 0; uint8 minRightRow = IMG_RGB_H - 1;
	for (short i = IMG_RGB_H - 2; i >= EFFECTIVE_ROW; i--) {
		if (leftValid[i]) {
			leftValidCount++;
			if (leftValid[i + 1] && leftLine[i] - leftLine[i + 1] < 0)
				isLeftLineAllToRight = false;
			minLeftRow = i;
		}
		if (rightValid[i]) {
			rightValidCount++;
			if (rightValid[i + 1] && rightLine[i] - rightLine[i + 1] > 0)
				isRightLineAllToLeft = false;
			minRightRow = i;
		}
	}

	if (isLeftLineAllToRight && leftValidCount >= (IMG_RGB_H - EFFECTIVE_ROW) * 0.5) {
		//从最末点开始右上(包括右上)dfs,标记为左边点
		//右上>上>右
		short currentPointI = minLeftRow;
		short currentPointJ = leftLine[minLeftRow];

		while (true) {
			if (isEdgePoint(currentPointI - 1, currentPointJ + 1)) {
				currentPointI = currentPointI - 1;
				currentPointJ = currentPointJ + 1;

				leftLine[currentPointI] = currentPointJ;
				leftValid[currentPointI] = true;
			}
			else if (isEdgePoint(currentPointI, currentPointJ + 1)) {
				currentPointI = currentPointI;
				currentPointJ = currentPointJ + 1;

				leftLine[currentPointI] = currentPointJ;
				leftValid[currentPointI] = true;
			}
			else if (isEdgePoint(currentPointI - 1, currentPointJ)) {
				currentPointI = currentPointI - 1;
				currentPointJ = currentPointJ;

				leftLine[currentPointI] = currentPointJ;
				leftValid[currentPointI] = true;
			}
			else {
				break;
			}
		}
	}

	if (isRightLineAllToLeft && rightValidCount >= (IMG_RGB_H - EFFECTIVE_ROW) * 0.5) {
		//从最末点开始左上(包括左上)dfs,标记为右边点
		//左上>上>左
		short currentPointI = minRightRow;
		short currentPointJ = rightLine[minRightRow];

		while (true) {
			if (isEdgePoint(currentPointI - 1, currentPointJ - 1)) {
				currentPointI = currentPointI - 1;
				currentPointJ = currentPointJ - 1;

				rightLine[currentPointI] = currentPointJ;
				rightValid[currentPointI] = true;
			}
			else if (isEdgePoint(currentPointI, currentPointJ - 1)) {
				currentPointI = currentPointI;
				currentPointJ = currentPointJ - 1;

				rightLine[currentPointI] = currentPointJ;
				rightValid[currentPointI] = true;
			}
			else if (isEdgePoint(currentPointI - 1, currentPointJ)) {
				currentPointI = currentPointI - 1;
				currentPointJ = currentPointJ;

				rightLine[currentPointI] = currentPointJ;
				rightValid[currentPointI] = true;
			}
			else {
				break;
			}
		}
	}
}


void centerLimitFilter(int i, short value) {
	centerValid[i] = true;

	//左
	if (value < centerLine[i]) {
		if (centerLine[i] - value < AMPLITUDE_LIMIT)
			centerLine[i] = value;
		else
			centerLine[i] = centerLine[i] - AMPLITUDE_LIMIT;
	}
	//右
	else {
		if (value - centerLine[i] < AMPLITUDE_LIMIT)
			centerLine[i] = value;
		else
			centerLine[i] = centerLine[i] + AMPLITUDE_LIMIT;
	}
}

void repairWholeLine(void) {
	volatile bool *validPointer = centerValid;
	volatile short *linePointer = centerLine;

	//找centerStart
	uint8 centerStart = 0;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
		if (validPointer[i]) {
			centerStart = i;
			break;
		}
	}

	//从centerStart连接前面的部分,最多到EFFECTIVE_ROW
	for (short i = centerStart; i >= EFFECTIVE_ROW; i--) {
		//缺点
		if (!validPointer[i]) {
			//后面有效点
			float x1 = i + 1, y1 = linePointer[i + 1];
			//找前面有效点
			float x2, y2;
			for (short j = i - 1; j >= EFFECTIVE_ROW; j--) {
				//如果找到
				if (validPointer[j]) {
					x2 = j;
					y2 = linePointer[j];

					for (short k = j + 1; k <= i; k++) {
						linePointer[k] = (y2 - y1) / (x2 - x1) * (k - x1) + y1;
						validPointer[k] = true;
					}

					break;
				}
			}
		}
	}

	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;
	float aLine, bLine;

	//拟合前面的部分
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	if (!(centerStart <= EFFECTIVE_ROW + 5)) {
		for (short i = centerStart; i >= centerStart - 5; i--) {
			if (centerValid[i]) {
				sumXY += i * linePointer[i];
				sumX += i;
				sumY += linePointer[i];
				sumX2 += i * i;

				validCount++;
			}
		}

		if (validCount > 1) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;
			aLine = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bLine = averageY - aLine * averageX;

			for (short i = IMG_RGB_H - 1; i > centerStart; i--) {
				linePointer[i] = aLine * i + bLine;
				validPointer[i] = true;
			}
		}
	}

	//拟合后面的部分
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//找到终止点
	short lineEnd = -1;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
		if (validPointer[i])
			lineEnd = i;
	}

	if (!(lineEnd == -1 || lineEnd <= EFFECTIVE_ROW)) {

		for (short i = lineEnd + 5; i >= lineEnd; i--) {
			if (centerValid[i]) {
				sumXY += i * linePointer[i];
				sumX += i;
				sumY += linePointer[i];
				sumX2 += i * i;

				validCount++;
			}
		}

		if (validCount > 1) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;
			aLine = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bLine = averageY - aLine * averageX;

			for (short i = lineEnd; i >= EFFECTIVE_ROW; i--) {
				linePointer[i] = aLine * i + bLine;
				validPointer[i] = true;
			}
		}
	}
}

volatile unsigned int isFinishLineCount = 0;
volatile unsigned int isNotFinishLineCount = 0;
void updateFinishLineCount(void) {
	//从底向上1/2找占0.9IMG_RGB_W的白行
	bool findAlmostWhiteRow = false;
	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
		uint8 whitePointCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				whitePointCnt++;
		}

		if (whitePointCnt > IMG_RGB_W * 0.9) {
			findAlmostWhiteRow = true;
			break;
		}
	}
	//如果找不到占0.9IMG_RGB_W的白行,直接认为找不到终点线
	if (!findAlmostWhiteRow) {
		isFinishLineCount = 0;
		isNotFinishLineCount++;
		return;
	}

	uint8 startPos, endPos;
	bool currentSegmentIsWhite;
	uint8 segmentCount;

	uint8 segmentLen;

	//从底向上1/2
	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
          //从下往上扫描到1/2高度
		//初始化startPos、currentSegmentIsWhite、segmentCount
		startPos = 0;
		if (isWhite(i, 0))
			currentSegmentIsWhite = true;//第i行第一个元素是白色
		else
			currentSegmentIsWhite = false;
		segmentCount = 0;

		while (startPos < IMG_RGB_W) {
			//从startPos开始根据currentSegmentIsWhite向右边找连续白段/连续黑段
			endPos = startPos;
			if (currentSegmentIsWhite) {
				while (endPos < IMG_RGB_W && isWhite(i, endPos))
					endPos++;
			}
			else {
				while (endPos < IMG_RGB_W && (!isWhite(i, endPos)))
					endPos++;
			}

			//根据segmentLen是否在范围内更新segmentCount
			//68 - 50 半段路8段 8.5 - 6.25 (3)
			segmentLen = endPos - startPos;
			if (segmentLen >= 2 && segmentLen <= 5) {
				segmentCount++;

				if (segmentCount >= 4) { //TODO 16段
					isFinishLineCount++;//这一行确定是起跑线的行
					isNotFinishLineCount = 0;
					return;
				}
			}
			else
				segmentCount = 0;

			//根据currentSegmentIsWhite更新下一次循环的startPos、currentSegmentIsWhite
			startPos = endPos;
			if (currentSegmentIsWhite)
				currentSegmentIsWhite = false;
			else
				currentSegmentIsWhite = true;
		}
	}

	//如果上面未return
	isFinishLineCount = 0;
	isNotFinishLineCount++;
}
//void updateFinishLineCount(void) {
//	//从底向上1/2找占0.9IMG_RGB_W的白行
//	bool findAlmostWhiteRow = false;
//	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
//		uint8 whitePointCnt = 0;
//		for (uint8 j = 0; j < IMG_RGB_W; j++) {
//			if (isWhite(i, j))
//				whitePointCnt++;
//		}
//
//		if (whitePointCnt > IMG_RGB_W * 0.9) {
//			findAlmostWhiteRow = true;
//			break;
//		}
//	}
//	//如果找不到占0.9IMG_RGB_W的白行,直接认为找不到终点线
//	if (!findAlmostWhiteRow) {
//		isFinishLineCount = 0;
//		isNotFinishLineCount++;
//		return;
//	}
//
//	uint8 startPos, endPos;
//	bool currentSegmentIsWhite;
//	uint8 segmentCount;
//
//	uint8 segmentLen;
//
//	//从底向上1/2
//	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
//		//初始化startPos、currentSegmentIsWhite、segmentCount
//		startPos = 0;
//		if (isWhite(i, 0))
//			currentSegmentIsWhite = true;
//		else
//			currentSegmentIsWhite = false;
//		segmentCount = 0;
//
//		while (startPos < IMG_RGB_W) {
//			//从startPos开始根据currentSegmentIsWhite找连续白段/连续黑段
//			endPos = startPos;
//			if (currentSegmentIsWhite) {
//				while (endPos < IMG_RGB_W && isWhite(i, endPos))
//					endPos++;
//			}
//			else {
//				while (endPos < IMG_RGB_W && (!isWhite(i, endPos)))
//					endPos++;
//			}
//
//			//根据segmentLen是否在范围内更新segmentCount
//			//68 - 50 半段路8段 8.5 - 6.25 (3)
//			segmentLen = endPos - startPos;
//			if (segmentLen >= 2 && segmentLen <= 5) {
//				segmentCount++;
//
//				if (segmentCount >= 10) { //TODO 16段
//					isFinishLineCount++;
//					isNotFinishLineCount = 0;
//					return;
//				}
//			}
//			else
//				segmentCount = 0;
//
//			//根据currentSegmentIsWhite更新下一次循环的startPos、currentSegmentIsWhite
//			startPos = endPos;
//			if (currentSegmentIsWhite)
//				currentSegmentIsWhite = false;
//			else
//				currentSegmentIsWhite = true;
//		}
//	}
//
//	//如果上面未return
//	isFinishLineCount = 0;
//	isNotFinishLineCount++;
//}

bool isBeforeBrokenRoad(void) {
	for (short i = IMG_RGB_H - 1; i >= IMG_RGB_H - 3; i--) {
		//检查本行白点个数是否小于一定比例
		uint8 rowWhiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				rowWhiteCnt++;
		}

		//检查到白点个数是否小于一定比例的行, 返回true
		if (rowWhiteCnt < (IMG_RGB_W / 3))
			return true;
	}

	//检查不到白点个数是否小于一定比例的行，返回false
	return false;
}

volatile unsigned int outOfBrokenRoadCount = 0;
void updateOutOfBrokenRoadCount(void) {
	//最近3行白点个数超过(9/10)
	for (short i = IMG_RGB_H - 1; i >= IMG_RGB_H - 3; i--) {
		short rowWhiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				rowWhiteCnt++;
		}

		//检查到白点个数未超过(9/10)的行
		if (rowWhiteCnt < (IMG_RGB_W * 9 / 10)) {
			outOfBrokenRoadCount = 0;
			return;
		}
	}

	//最近3行为全白行
	outOfBrokenRoadCount++;
}

volatile unsigned int isRampCount;
void updateRampCount(void) {
	for (uint8 i = 0; i < RAMP_END_ROW; i++) {
		//第i行白点个数
		uint8 whiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				whiteCnt++;
		}

		//计算本行应有白点个数
		uint8 roadLen = roadLenK * i + roadLenB;

		//本行白点个数应全满或大于roadLen + RAMP_MORE_WHITE_THR
		//如不满足，置坡道计数为0，直接返回
		if ((whiteCnt < IMG_RGB_W)
			&& (whiteCnt < roadLen + RAMP_MORE_WHITE_THR)) {
			isRampCount = 0;
			return;
		}
	}

	//上面未返回，说明通过检验
	isRampCount++;
}

/*********************************************************************/
/*************************进行中线校验********************************/
/*********************************************************************/
void invalidCenterLine(void);
void centerLineValidation(void) {
	//右丢线
	if (rightIsLost) {
		//图像上半部分左线未超过阈值且左线未越过中间则补线无效
		uint8 leftPointCnt = 0;
		uint8 leftColMax = 0;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H / 2; i++) {
			if (leftValid[i]) {
				leftPointCnt++;
				if (leftLine[i] > leftColMax)
					leftColMax = leftLine[i];
			}
		}

		if (leftPointCnt < (IMG_RGB_H / 4) && leftColMax < (IMG_RGB_W / 2))
			invalidCenterLine();
	}

	//左丢线
	if (leftIsLost) {
		//图像上半部分右线未超过阈值且右线未越过中间则补线无效
		uint8 rightPointCnt = 0;
		uint8 rightColMin = IMG_RGB_W - 1;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H / 2; i++) {
			if (rightValid[i]) {
				rightPointCnt++;
				if (rightLine[i] < rightColMin)
					rightColMin = rightLine[i];
			}
		}

		if (rightPointCnt < (IMG_RGB_H / 4) && rightColMin >(IMG_RGB_W / 2))
			invalidCenterLine();
	}

	//不丢线
	if (!leftIsLost && !rightIsLost) {
		//检查图像下半部分的路宽
		uint8 actualLen, roadLen;
		for (uint8 i = IMG_RGB_H / 2; i < IMG_RGB_H; i++) {
			//如果本行左右边点存在
			if (leftValid[i] && rightValid[i]) {
				actualLen = rightLine[i] - leftLine[i];
				roadLen = roadLenK * i + roadLenB;

				//本行实际路宽应全满或大于roadLen - 10
				//如不满足, 无效中线并跳出循环
				if ((actualLen < IMG_RGB_W)
					&& (actualLen < roadLen - 10)) {
					invalidCenterLine();
					break;
				}
			}
		}
	}
}
void invalidCenterLine(void) {
	CENTER_UNFOUND = true;
}
