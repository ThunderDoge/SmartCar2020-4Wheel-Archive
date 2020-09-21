/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨������i.MX RT1052���İ�-���ܳ���
����    д������
����    ����V1.0
��ע    �⡿���ļ�Ϊ���������ֲ��Ϊͼ�����ļ���������������
�������¡�2020��1��12��
------------------------------------------------
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "szx_image.h"
#include "szx_color.h"
#include "myUtil.h"
//ͼ�����֣�����ÿһ���Ĳ���
volatile uint8 imageRGB[IMG_RGB_H][IMG_RGB_W][3]; //0-31 0-31 0-31 R G B 

/******************************/
/************����**************/
/******************************/
volatile uint8 roadPointStackX[MAX_LINE_POINT_NUM];//volatile ��ʾ�ö�������ܿ��ܷ����仯�������������жϳ����У�
volatile uint8 roadPointStackY[MAX_LINE_POINT_NUM];//�ñ�����ֵҪ���ڴ��ж�ȡ
volatile short stackTopPos;                     //ջ��
volatile bool isVisited[IMG_RGB_H][IMG_RGB_W];  //ij���Ƿ񱻷��ʹ�

volatile short leftLine[IMG_RGB_H]; //�����i�е�����ߵ��ǵ�j����j����������
volatile short rightLine[IMG_RGB_H];
volatile short centerLine[IMG_RGB_H];
volatile bool leftValid[IMG_RGB_H]; //�����i���Ƿ�����Ч����ߵ�
volatile bool rightValid[IMG_RGB_H];
volatile bool centerValid[IMG_RGB_H];

volatile bool leftIsLost;  
volatile bool rightIsLost;

volatile bool CENTER_UNFOUND;

short myMax(short a, short b);
inline short myMax(short a, short b) {
	return (a > b ? a : b);
}
//ͼ�����������ʼ��
void imageProcessParamsInit(void) {
	colorSegmentationInit();

	stackTopPos = -1;
	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			isVisited[i][j] = false;//stdbool.h�н�false����Ϊbool�ͱ���0��trueΪbool�ͱ���1
		}                         //�Ƿ���ʹ�ij����ʼ��Ϊ0
	}

	for (uint8 i = 0; i < IMG_RGB_H; i++) {
		leftValid[i] = rightValid[i] = centerValid[i] = false;//�������Ч���ұ�����Ч��������Ч��ʼ��Ϊ0
	}
}

//brokenAngleJudge�еõ��Ľ��
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
//��Ҫimage,����isLeftPoint,isRightPoint Ѱ������
void findCenterLine(void) {
	//DFS ��������������ұ���
	roadPointDfs();
	//�����������,δ����ʱ����
	doFliter();
	//�޸���ʼ����
	repairStartOfEdge();
	//TODO �����߱仯����Ϊһ������/���߱仯����һ��Ϊ����ʱ,�����ĩ����SobelYΪ��������
	//ǰ����DFS��Ѱ����δ�����˵ı��߲�����ֱ��
	if (originLineResValid && !originLeftLineStraight && !originRightLineStraight)
		repairEndOfEdge();

	if (leftIsLost && rightIsLost) {
		CENTER_UNFOUND = true;

		return;
	}
	CENTER_UNFOUND = false;
	if (leftIsLost) {
		//�������߲���
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
		//�����㹻,��Error����
		float centerLineFix = 0;
		if (validCount >= 5 && sumError > 0) {
			float aveError = sumError / validCount;
			if (aveError > roadMoreMaxAveErrorLostLine) aveError = roadMoreMaxAveErrorLostLine;
			centerLineFix = aveError / roadMoreMaxAveErrorLostLine * roadMoreLostLine;
		}

		//����
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (rightValid[i]) {
				centerLimitFilter(i, rightLine[i] - roadLenK / 2 * i - roadLenB / 2 - centerLineFix);
			}
		}


		repairWholeLine();
	}
	else if (rightIsLost) {
		//�������߲���
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
		//�����㹻,��Error����
		float centerLineFix = 0;
		if (validCount >= 5 && sumError > 0) {
			float aveError = sumError / validCount;
			if (aveError > roadMoreMaxAveErrorLostLine) aveError = roadMoreMaxAveErrorLostLine;
			centerLineFix = aveError / roadMoreMaxAveErrorLostLine * roadMoreLostLine;
		}

		//����
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (leftValid[i]) {
				centerLimitFilter(i, leftLine[i] + roadLenK / 2 * i + roadLenB / 2 + centerLineFix);
			}
		}

		repairWholeLine();
	}
	else if (!leftIsLost && !rightIsLost) {
		//��·��Ч���ȹ���
		uint8 minRoadLen = IMG_RGB_W / 6;

		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (leftValid[i] && rightValid[i]) {
				//��·��Ч���ȹ���
				if (rightLine[i] - leftLine[i] < minRoadLen) continue;

				//��·���Ȳ��������ֵ,����ȡ����
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

		//�޸�����
		repairWholeLine();

		//�޸����߲��ɹ�
		for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
			if (!centerValid[i])
				centerValid[i] = true;
		}
	}
}

void roadPointDfs(void) {//�����������
	//��ʼ�㣨28��20��
	if (isWhite(IMG_RGB_H - 2, IMG_RGB_W / 2)) { //�ж�Խ�籣�� ���ҿ��Ƿ��ǰ�ɫ������ǰ�ɫ�����صİ�ɫ�����Ϊ1����������²���
		                                           //�������Ϊ����죬����else����
		stackTopPos++;                             //ջ��++����һ�α�Ϊ0���Ʋ�����ʼ���rgb������ջ                           
		roadPointStackX[stackTopPos] = IMG_RGB_H - 2;      //��·��Ķ�ջ
		roadPointStackY[stackTopPos] = IMG_RGB_W / 2;

		isVisited[IMG_RGB_H - 2][IMG_RGB_W / 2] = true;    //��һ����Ѿ������ʹ���
	}
	else {                                        //������ǰ�ɫ
		for (uint8 j = 2; j < IMG_RGB_W - 2; j++) {                         //ע�� ÿһ�дӵ�0�п�ʼ ����28�д�2��ʼ�ۼӵ�19 //�ж�28�еĵ�2�е����Ҹ������Ƿ�ȫ�ǰ�ɫ������ǣ���28��2����ջ
			if (isWhite(IMG_RGB_H - 2, j - 2) && isWhite(IMG_RGB_H - 2, j - 1)&& isWhite(IMG_RGB_H - 2, j) && isWhite(IMG_RGB_H - 2, j + 1) && isWhite(IMG_RGB_H - 2, j + 2)) 
                        {
				
				stackTopPos++;                                                  //ջ��++
				roadPointStackX[stackTopPos] = IMG_RGB_H - 2;
				roadPointStackY[stackTopPos] = j;

				isVisited[IMG_RGB_H - 2][j] = true;                             //�ѷ���

				break;
			}
		}
	}

	uint8 i, j;
	while (stackTopPos >= 0) {
		i = roadPointStackX[stackTopPos];                                    //iΪ������jΪ���� ��ջ��Ϊ��ɫ��
		j = roadPointStackY[stackTopPos];
		stackTopPos--;                                                       //��ջ

		//�������
		if (!(i >= EFFECTIVE_ROW && i <= IMG_RGB_H - 2 && j >= 1 && j <= IMG_RGB_W - 2)) continue;//�������ֱ����������whileѭ��
		//����Dfs,���ڼ��߽��

		//��
		if (!isVisited[i][j - 1]) {                  //û������ij��ǰһ����
			if (isWhite(i, j - 1)) {                   //�����i��j-1�ǰ�ɫ��
				stackTopPos++;                           //��ջ
				roadPointStackX[stackTopPos] = i;
				roadPointStackY[stackTopPos] = j - 1;

				//����ѷ���
				isVisited[i][j - 1] = true;
			}
			else {                                      //�����i��j-1���ǰ�ɫ�� ���i��j�Ƿ��Ǳ��ߵ�
				if (isLeftPoint(i, j)) {                  //�ж������
					leftLine[i] = j;                        //��i�е�����ߵ�Ϊj
					leftValid[i] = true;                    //��i�����������
				}
			}
		}

		//��
		if (!isVisited[i][j + 1]) {
			if (isWhite(i, j + 1)) {
				stackTopPos++;
				roadPointStackX[stackTopPos] = i;
				roadPointStackY[stackTopPos] = j + 1;

				//����ѷ���
				isVisited[i][j + 1] = true;
			}
			else {
				if (isRightPoint(i, j)) {
					rightLine[i] = j;
					rightValid[i] = true;
				}
			}
		}

		//��
		if (!isVisited[i - 1][j]) {
			if (isWhite(i - 1, j)) {
				stackTopPos++;
				roadPointStackX[stackTopPos] = i - 1;
				roadPointStackY[stackTopPos] = j;

				//����ѷ���
				isVisited[i - 1][j] = true;
			}
		}
	}
}

bool isLeftPoint(int i, int j) { //��������Ӧ�����к�ɫ�㣬�ұ�Ӧ��û�а�ɫ�� ij�Ӷ�ջ��������������ǰ�ɫ
	//Խ�籣��
	if (j < 3 || j >= IMG_RGB_W - 3) return false;
	//�ǰ� �õ㼰���ұ�Ӧ�ö��ǰ�ɫ
	if ((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2))) return false;
	//���޺�
	if ((!isBlueOrBlack(i, j - 3)) && (!isBlueOrBlack(i, j - 2)) && (!isBlueOrBlack(i, j - 1))) return false;
	return true;
}

bool isRightPoint(int i, int j) { //�ұ����ұ߻��к�ɫ�㣬����ް�ɫ�㣬ij�������ǰ�ɫ ��ȡ���ı��ߵ㶼�ǰ�ɫ��
	//Խ�籣��
	if (j < 3 || j >= IMG_RGB_W - 3) return false;
	//�ǰ�
	if ((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2))) return false;
	//���޺�
	if ((!isBlueOrBlack(i, j + 3)) && (!isBlueOrBlack(i, j + 2)) && (!isBlueOrBlack(i, j + 1))) return false;
	return true;
}

bool isEdgePoint(int i, int j) {
	//Խ�籣��
	if (i < 3 || i >= IMG_RGB_H - 3 || j < 3 || j >= IMG_RGB_W - 3) return false;
	//�ǰ�
	if (!isWhite(i, j)) return false;
	//�������¶��Ǻ�
	if ((!isBlueOrBlack(i, j + 2))
		&& (!isBlueOrBlack(i, j - 2))
		&& (!isBlueOrBlack(i - 2, j))
		&& (!isBlueOrBlack(i + 2, j))
		)
		return false;

	return true;
}

void lostLineJudge(void) {
	//����LOST_LINE_THRESHOLD��ֱ����Ϊ����  �б��ߵ��������ٴ���4
	uint8 validCount = 0;//��Ч����
	//���������
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
		leftIsLost = false; //�����û��
	}
  //�����ұ���
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
	/***************����·��**************/
	float kLeft, bLeft, kRight, bRight;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//����� ��С���˷� ����� y=kx+b
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (leftValid[i]) {          //�����i������Ч����ߵ㣬��ִ�����²���
			sumXY += i * leftLine[i];  //X,Y��;�������ߵ��������˻�
			sumX += i;
			sumY += leftLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= LOST_LINE_THRESHOLD) {  //���ߵ���ֵ4
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kLeft = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bLeft = averageY - kLeft * averageX;
	}
	else {
		leftIsLost = true;
	}

	//�����
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

	//�����һ��,�Ƚ���Ѱ������,������ʱ��ԭԭ״̬���Ͳ���ǰ����ж��Ƿ���ʵĳ���ֱ�Ӵ��ұ��ߵ������ߣ�����������������
	bool setLeftIsLost = false, setRightIsLost = false;
	if (leftIsLost && !rightIsLost) {
		setLeftIsLost = true;  //���߶���

		//������������
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H - 2; i++) {
			if (rightValid[i]) {
				for (short j = rightLine[i] - 1; j >= 0; j--) {
					if (isLeftPoint(i, j)) {     //������һ������ߵ�
						leftValid[i] = true;
						leftLine[i] = j;
						break;
					}
				}
			}
		}

		leftIsLost = false;

		//�����
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
	//���߶���
	else if (rightIsLost && !leftIsLost) {
		setRightIsLost = true;

		//������������
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

		//�����
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

	//�������δ��ʧ
	if (!leftIsLost && !rightIsLost) {
		//�������о�
		float averageFitErrorLeft = 0, averageFitErrorRight = 0;
		short leftCount = 0, rightCount = 0;
		float fitValue;
		for (uint8 i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) { //��ϳ��ı�����ʵ�ʱ��ߵ�������
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
		averageFitErrorLeft /= leftCount;//�����ÿ��ƽ��������
		averageFitErrorRight /= rightCount;

		//��������������
		originLineResValid = true;
		if (averageFitErrorLeft <= (1 * 1)) {
			originLeftLineStraight = true; //��ϳɹ�
			originLeftLineK = kLeft;
			originLeftLineB = bLeft;
		}
		else
			originLeftLineStraight = false;//���ʧ��
		if (averageFitErrorRight <= (1 * 1)) {
			originRightLineStraight = true;
			originRightLineK = kRight;
			originRightLineB = bRight;
		}
		else
			originRightLineStraight = false;

		//��һ��������ڸ���ֵ,��Ϊ����ֱ�� //actualCenterLineDfs 1325
		if ((averageFitErrorLeft <= (1 * 1) && averageFitErrorRight <= (5 * 5))
			|| (averageFitErrorLeft <= (5 * 5) && averageFitErrorRight <= (1 * 1))
			) {
			//����ֱ��,���㽻���  ����Ϊ��Ǿ�ͷ �����������߲����ӳ������ս��ڼ�Զ��
			float xPoint = (bRight - bLeft) / (kLeft - kRight);
			float yPoint = kLeft * xPoint + bLeft;

			//��������x����̫С �Ҵ���������� ����ԭ�������Ͻ�
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
		//ֻ��������������
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

		//��������������
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
		//ֻ��������������
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

		//��������������
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

	//ǰ��δreturn,˵�����Ƕ�·��
	isBrokenRoadAngleCount = 0;
}

void crossLeftLineJudge(void) {
	//���������ҵ㣬�����ҵ�δ����1/3�򲻼��
	short leftLineMaxPoint = 0;
	for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (leftValid[i] && leftLine[i] > leftLineMaxPoint)
			leftLineMaxPoint = leftLine[i];
	}
	if (leftLineMaxPoint < IMG_RGB_W / 3)
		return;

	//�����߹յ�1
	short leftInflectionPoint = EFFECTIVE_ROW;
	short leftInflectionPoint2 = EFFECTIVE_ROW;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {//����������
		if (leftValid[i]) {
			//�����������Ч�� �����j����ʾ����
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//�ҵ������Ч��
				if (leftValid[j]) {
					//�ҵ��յ�
					if (leftLine[i] < leftLine[j])//���ݹ�Ǿ�ͷ����ԭ������һ������ߵ�������ش�����һ�У��ʵ�С��ʱ��������
						leftInflectionPoint = i;    //�յ�������
					break;
				}
			}
		}

		//������ҵ��յ�,break
		if (leftInflectionPoint != EFFECTIVE_ROW)
			break;
	}

	//�����߹յ�2  �ӵ�һ�յ�������
	for (short i = leftInflectionPoint; i >= EFFECTIVE_ROW; i--) {//////////////////////������������
		if (leftValid[i]) {
			//�����������Ч��
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//�����ҵ������Ч��
				if (leftValid[j]) {
					//�ҵ��յ�2
					if (leftLine[i] > leftLine[j])
						leftInflectionPoint2 = i;
					break;
				}
			}
		}

		//������ҵ��յ�2,break
		if (leftInflectionPoint2 != EFFECTIVE_ROW)
			break;
	}

	//�ֱ��������ֱ����� �յ����µĵ����ֱ�����
	float kLeft1, bLeft1, kLeft2, bLeft2;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//���[leftInflectionPoint + 1, IMG_RGB_H - 1]  ��һ�յ���һ�е�ĩβ
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

	//���[leftInflectionPoint2 + 1, leftInflectionPoint] //�ӵڶ��յ���һ�е���һ�յ�
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	for (short i = leftInflectionPoint2 + 1; i <= leftInflectionPoint; i++) {//�ڶ��յ��ڵ�һ�յ�����
		if (leftValid[i]) {
			sumXY += i * leftLine[i];
			sumX += i;
			sumY += leftLine[i];
			sumX2 += i * i;

			validCount++;
		}
	}

	if (validCount >= CROSS_EDGE_LEN) { //�ڶ��յ��ڵ�һ�յ����泬��4��
		averageX = sumX / validCount;
		averageY = sumY / validCount;

		kLeft2 = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
		bLeft2 = averageY - kLeft2 * averageX;
	}
	else
		return;

	//ǰ��δreturn��˵�����¶ζ���ϳɹ�������������
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

	//���ζ���ֱ�ߣ��Ҷ�1���Ҷ�2����
	if (
		((averageFitError1 <= 4 && averageFitError2 <= 1) || (averageFitError1 <= 1 && averageFitError2 <= 4))
		&& ((kLeft1 < -2 && kLeft2 > 1) || (kLeft1 < 0 && kLeft2 > 2.5))) {
		for (short i = EFFECTIVE_ROW; i <= leftInflectionPoint; i++) //��ʮ�ִ�·���ʵ�һ�յ����ϲ��ֵı��߾���
			leftValid[i] = false;
		for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++)
			rightValid[i] = false;
		rightIsLost = true;
	}
}

void crossRightLineJudge(void) {
	//����������㣬�������δ����1/3�򲻼��
	short rightLineMinPoint = IMG_RGB_W - 1;
	for (short i = EFFECTIVE_ROW; i < IMG_RGB_H; i++) {
		if (rightValid[i] && rightLine[i] < rightLineMinPoint)
			rightLineMinPoint = rightLine[i];
	}
	if (rightLineMinPoint > (IMG_RGB_W * 2 / 3))
		return;

	//�����߹յ�1
	short rightInflectionPoint = EFFECTIVE_ROW;
	short rightInflectionPoint2 = EFFECTIVE_ROW;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
		if (rightValid[i]) {
			//�����������Ч��
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//�ҵ������Ч��
				if (rightValid[j]) {
					//�ҵ��յ�
					if (rightLine[i] > rightLine[j])
						rightInflectionPoint = i;
					break;
				}
			}
		}

		//������ҵ��յ�,break
		if (rightInflectionPoint != EFFECTIVE_ROW)
			break;
	}

	//�����߹յ�2
	for (short i = rightInflectionPoint; i >= EFFECTIVE_ROW; i--) {
		if (rightValid[i]) {
			//�����������Ч��
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				//�ҵ������Ч��
				if (rightValid[j]) {
					//�ҵ��յ�2
					if (rightLine[i] < rightLine[j])
						rightInflectionPoint2 = i;
					break;
				}
			}
		}

		//������ҵ��յ�2,break
		if (rightInflectionPoint2 != EFFECTIVE_ROW)
			break;
	}

	//�ֱ��������ֱ�����
	float kRight1, bRight1, kRight2, bRight2;
	int sumXY, sumX, sumY, sumX2;
	float averageX, averageY;
	float validCount;

	//���[rightInflectionPoint + 1, IMG_RGB_H - 1]
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

	//���[rightInflectionPoint2 + 1, rightInflectionPoint]
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

	//ǰ��δreturn��˵�����¶ζ���ϳɹ�������������
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

	//���ζ���ֱ�ߣ��Ҷ�1�����2����
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
	/***************��ʮ�ִ���**************/ //TODO check
										 //�������
	crossLeftLineJudge();
	//�������
	crossRightLineJudge();
}


//�����������,δ����ʱ����
void doFliter(void) {
	lostLineJudge();//�ж��Ƿ���
//	brokenRoadAngleJudge();//��· �ж�
//	crossJudge();  //��ʮ�ִ���
	////��IMG_RGB_H - 2��ʼ
	/*******������Ҳ�����,����˵�Խ���м�ĵ�****/
	if (!leftIsLost && !rightIsLost) {
		for (short i = IMG_RGB_H - 2; i >= 0; i--) {
			if (leftValid[i] && leftLine[i] > IMG_RGB_W / 2)//˵�����ߵ�Խ�����ߣ�������
				leftValid[i] = false;
			if (rightValid[i] && rightLine[i] < IMG_RGB_W / 2)
				rightValid[i] = false;
		}
	}

	//if (leftIsLost || rightIsLost) return;
	short k;

	for (short i = IMG_RGB_H - 2; i >= 0; i--) {
		if (leftValid[i]) {
			//�����ҵ�һ����Ч��
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				if (leftValid[j]) {
					//�ҵ���Ч��,����б�ʣ���ʵ����б�ʣ�
					k = (leftLine[i] - leftLine[j]) / (i - j);
					//���С�������õ�
					if (k > 0) {
						//����һ��ȥ��?
						leftValid[i] = false;
						leftValid[j] = false;
					}
					//��� ��б�ʣ����󲻱����õ�
					if (k < 0) k = -k;//��������Ϊ����
					if (k > FILTER_K_THRESHOLD) { //TODO Ӱ���˴�����ߵ�ʶ�� ������ֵ6
												  //����һ��ȥ��?
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
			//�����ҵ�һ����Ч��
			for (short j = i + 1; j < IMG_RGB_H; j++) {
				if (rightValid[j]) {
					//�ҵ���Ч��,����б��
					k = (rightLine[i] - rightLine[j]) / (i - j);
					//�����󲻱����õ�
					if (k < 0) {
						//����һ��ȥ��?
						rightValid[i] = false;
						rightValid[j] = false;
					}
					//�����󲻱����õ�
					if (k < 0) k = -k;
					if (k > FILTER_K_THRESHOLD) {
						//����һ��ȥ��?
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
	/************�����***********/
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//�ҵ�����ʼ
	uint8 leftStart = 0;
	for (short i = IMG_RGB_H - 1; i >= 0; i--) {
		if (leftValid[i]) {
			leftStart = i;
			break;//��������forѭ��
		}
	}
	//����ʼ����1/6,������
	if (leftStart > IMG_RGB_H / 6) {
		//������ʼ����Ϊ15������5����
		for (short i = leftStart; i >= myMax(leftStart - 15, 0); i--) {  //ȡ�����д��ֵ
			if (leftValid[i]) {
				sumXY += i * leftLine[i];
				sumX += i;
				sumY += leftLine[i];
				sumX2 += i * i;

				validCount++;
			}
			if (validCount == 5) break;
		}

		//����ҵõ�5����
		if (validCount == 5) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			aLeft = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bLeft = averageY - aLeft * averageX;
			leftExist = true;
		}
	}

	/************�����***********/
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//�ҵ�����ʼ
	uint8 rightStart = 0;
	for (short i = IMG_RGB_H - 1; i >= 0; i--) {
		if (rightValid[i]) {
			rightStart = i;
			break;
		}
	}
	//����ʼ����1/6������
	if (rightStart > IMG_RGB_H / 6) {
		//������ʼ����Ϊ15������5����
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

		//����ҵõ�5����
		if (validCount == 5) {
			averageX = sumX / validCount;
			averageY = sumY / validCount;

			aRight = (sumXY - sumX * sumY / validCount) / (sumX2 - sumX * sumX / validCount);
			bRight = averageY - aRight * averageX;
			rightExist = true;
		}
	}

	/************��������Ԥ��ֵ***********/ 
	if (leftExist) {                       //����������ϲ�����
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
	//�����Ƿ�һֱ����?�����Ƿ��㹻?��¼��ĩ��
	bool isLeftLineAllToRight = true; uint8 leftValidCount = 0; uint8 minLeftRow = IMG_RGB_H - 1;
	//�����Ƿ�һֱ����?�����Ƿ��㹻?��¼��ĩ��
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
		//����ĩ�㿪ʼ����(��������)dfs,���Ϊ��ߵ�
		//����>��>��
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
		//����ĩ�㿪ʼ����(��������)dfs,���Ϊ�ұߵ�
		//����>��>��
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

	//��
	if (value < centerLine[i]) {
		if (centerLine[i] - value < AMPLITUDE_LIMIT)
			centerLine[i] = value;
		else
			centerLine[i] = centerLine[i] - AMPLITUDE_LIMIT;
	}
	//��
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

	//��centerStart
	uint8 centerStart = 0;
	for (short i = IMG_RGB_H - 1; i >= EFFECTIVE_ROW; i--) {
		if (validPointer[i]) {
			centerStart = i;
			break;
		}
	}

	//��centerStart����ǰ��Ĳ���,��ൽEFFECTIVE_ROW
	for (short i = centerStart; i >= EFFECTIVE_ROW; i--) {
		//ȱ��
		if (!validPointer[i]) {
			//������Ч��
			float x1 = i + 1, y1 = linePointer[i + 1];
			//��ǰ����Ч��
			float x2, y2;
			for (short j = i - 1; j >= EFFECTIVE_ROW; j--) {
				//����ҵ�
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

	//���ǰ��Ĳ���
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

	//��Ϻ���Ĳ���
	sumXY = sumX = sumY = sumX2 = 0;
	validCount = 0;
	//�ҵ���ֹ��
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
	//�ӵ�����1/2��ռ0.9IMG_RGB_W�İ���
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
	//����Ҳ���ռ0.9IMG_RGB_W�İ���,ֱ����Ϊ�Ҳ����յ���
	if (!findAlmostWhiteRow) {
		isFinishLineCount = 0;
		isNotFinishLineCount++;
		return;
	}

	uint8 startPos, endPos;
	bool currentSegmentIsWhite;
	uint8 segmentCount;

	uint8 segmentLen;

	//�ӵ�����1/2
	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
          //��������ɨ�赽1/2�߶�
		//��ʼ��startPos��currentSegmentIsWhite��segmentCount
		startPos = 0;
		if (isWhite(i, 0))
			currentSegmentIsWhite = true;//��i�е�һ��Ԫ���ǰ�ɫ
		else
			currentSegmentIsWhite = false;
		segmentCount = 0;

		while (startPos < IMG_RGB_W) {
			//��startPos��ʼ����currentSegmentIsWhite���ұ��������׶�/�����ڶ�
			endPos = startPos;
			if (currentSegmentIsWhite) {
				while (endPos < IMG_RGB_W && isWhite(i, endPos))
					endPos++;
			}
			else {
				while (endPos < IMG_RGB_W && (!isWhite(i, endPos)))
					endPos++;
			}

			//����segmentLen�Ƿ��ڷ�Χ�ڸ���segmentCount
			//68 - 50 ���·8�� 8.5 - 6.25 (3)
			segmentLen = endPos - startPos;
			if (segmentLen >= 2 && segmentLen <= 5) {
				segmentCount++;

				if (segmentCount >= 4) { //TODO 16��
					isFinishLineCount++;//��һ��ȷ���������ߵ���
					isNotFinishLineCount = 0;
					return;
				}
			}
			else
				segmentCount = 0;

			//����currentSegmentIsWhite������һ��ѭ����startPos��currentSegmentIsWhite
			startPos = endPos;
			if (currentSegmentIsWhite)
				currentSegmentIsWhite = false;
			else
				currentSegmentIsWhite = true;
		}
	}

	//�������δreturn
	isFinishLineCount = 0;
	isNotFinishLineCount++;
}
//void updateFinishLineCount(void) {
//	//�ӵ�����1/2��ռ0.9IMG_RGB_W�İ���
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
//	//����Ҳ���ռ0.9IMG_RGB_W�İ���,ֱ����Ϊ�Ҳ����յ���
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
//	//�ӵ�����1/2
//	for (short i = IMG_RGB_H - 1; i >= (IMG_RGB_H / 2); i--) {
//		//��ʼ��startPos��currentSegmentIsWhite��segmentCount
//		startPos = 0;
//		if (isWhite(i, 0))
//			currentSegmentIsWhite = true;
//		else
//			currentSegmentIsWhite = false;
//		segmentCount = 0;
//
//		while (startPos < IMG_RGB_W) {
//			//��startPos��ʼ����currentSegmentIsWhite�������׶�/�����ڶ�
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
//			//����segmentLen�Ƿ��ڷ�Χ�ڸ���segmentCount
//			//68 - 50 ���·8�� 8.5 - 6.25 (3)
//			segmentLen = endPos - startPos;
//			if (segmentLen >= 2 && segmentLen <= 5) {
//				segmentCount++;
//
//				if (segmentCount >= 10) { //TODO 16��
//					isFinishLineCount++;
//					isNotFinishLineCount = 0;
//					return;
//				}
//			}
//			else
//				segmentCount = 0;
//
//			//����currentSegmentIsWhite������һ��ѭ����startPos��currentSegmentIsWhite
//			startPos = endPos;
//			if (currentSegmentIsWhite)
//				currentSegmentIsWhite = false;
//			else
//				currentSegmentIsWhite = true;
//		}
//	}
//
//	//�������δreturn
//	isFinishLineCount = 0;
//	isNotFinishLineCount++;
//}

bool isBeforeBrokenRoad(void) {
	for (short i = IMG_RGB_H - 1; i >= IMG_RGB_H - 3; i--) {
		//��鱾�а׵�����Ƿ�С��һ������
		uint8 rowWhiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				rowWhiteCnt++;
		}

		//��鵽�׵�����Ƿ�С��һ����������, ����true
		if (rowWhiteCnt < (IMG_RGB_W / 3))
			return true;
	}

	//��鲻���׵�����Ƿ�С��һ���������У�����false
	return false;
}

volatile unsigned int outOfBrokenRoadCount = 0;
void updateOutOfBrokenRoadCount(void) {
	//���3�а׵��������(9/10)
	for (short i = IMG_RGB_H - 1; i >= IMG_RGB_H - 3; i--) {
		short rowWhiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				rowWhiteCnt++;
		}

		//��鵽�׵����δ����(9/10)����
		if (rowWhiteCnt < (IMG_RGB_W * 9 / 10)) {
			outOfBrokenRoadCount = 0;
			return;
		}
	}

	//���3��Ϊȫ����
	outOfBrokenRoadCount++;
}

volatile unsigned int isRampCount;
void updateRampCount(void) {
	for (uint8 i = 0; i < RAMP_END_ROW; i++) {
		//��i�а׵����
		uint8 whiteCnt = 0;
		for (uint8 j = 0; j < IMG_RGB_W; j++) {
			if (isWhite(i, j))
				whiteCnt++;
		}

		//���㱾��Ӧ�а׵����
		uint8 roadLen = roadLenK * i + roadLenB;

		//���а׵����Ӧȫ�������roadLen + RAMP_MORE_WHITE_THR
		//�粻���㣬���µ�����Ϊ0��ֱ�ӷ���
		if ((whiteCnt < IMG_RGB_W)
			&& (whiteCnt < roadLen + RAMP_MORE_WHITE_THR)) {
			isRampCount = 0;
			return;
		}
	}

	//����δ���أ�˵��ͨ������
	isRampCount++;
}

/*********************************************************************/
/*************************��������У��********************************/
/*********************************************************************/
void invalidCenterLine(void);
void centerLineValidation(void) {
	//�Ҷ���
	if (rightIsLost) {
		//ͼ���ϰ벿������δ������ֵ������δԽ���м�������Ч
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

	//����
	if (leftIsLost) {
		//ͼ���ϰ벿������δ������ֵ������δԽ���м�������Ч
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

	//������
	if (!leftIsLost && !rightIsLost) {
		//���ͼ���°벿�ֵ�·��
		uint8 actualLen, roadLen;
		for (uint8 i = IMG_RGB_H / 2; i < IMG_RGB_H; i++) {
			//����������ұߵ����
			if (leftValid[i] && rightValid[i]) {
				actualLen = rightLine[i] - leftLine[i];
				roadLen = roadLenK * i + roadLenB;

				//����ʵ��·��Ӧȫ�������roadLen - 10
				//�粻����, ��Ч���߲�����ѭ��
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
