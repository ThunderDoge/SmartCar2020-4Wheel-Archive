/*-----------------------------------------------------------------
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月12日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【dev.env.】IAR8.30.1及以上版本
--------------------------------------------------------------------*/
#ifndef _GPIO_CFG_H_
#define _GPIO_CFG_H_

//定义PT1_的端口  
#define PT1_0     GPIO1->DRs.DR0
#define PT1_1     GPIO1->DRs.DR1
#define PT1_2     GPIO1->DRs.DR2
#define PT1_3     GPIO1->DRs.DR3
#define PT1_4     GPIO1->DRs.DR4
#define PT1_5     GPIO1->DRs.DR5
#define PT1_6     GPIO1->DRs.DR6
#define PT1_7     GPIO1->DRs.DR7
#define PT1_8     GPIO1->DRs.DR8
#define PT1_9     GPIO1->DRs.DR9
#define PT1_10    GPIO1->DRs.DR10
#define PT1_11    GPIO1->DRs.DR11
#define PT1_12    GPIO1->DRs.DR12
#define PT1_13    GPIO1->DRs.DR13
#define PT1_14    GPIO1->DRs.DR14
#define PT1_15    GPIO1->DRs.DR15
#define PT1_16    GPIO1->DRs.DR16
#define PT1_17    GPIO1->DRs.DR17
#define PT1_18    GPIO1->DRs.DR18
#define PT1_19    GPIO1->DRs.DR19
#define PT1_20    GPIO1->DRs.DR20
#define PT1_21    GPIO1->DRs.DR21
#define PT1_22    GPIO1->DRs.DR22
#define PT1_23    GPIO1->DRs.DR23
#define PT1_24    GPIO1->DRs.DR24
#define PT1_25    GPIO1->DRs.DR25
#define PT1_26    GPIO1->DRs.DR26
#define PT1_27    GPIO1->DRs.DR27
#define PT1_28    GPIO1->DRs.DR28
#define PT1_29    GPIO1->DRs.DR29
#define PT1_30    GPIO1->DRs.DR30
#define PT1_31    GPIO1->DRs.DR31

//定义PT2_的端口  
#define PT2_0     GPIO2->DRs.DR0
#define PT2_1     GPIO2->DRs.DR1
#define PT2_2     GPIO2->DRs.DR2
#define PT2_3     GPIO2->DRs.DR3
#define PT2_4     GPIO2->DRs.DR4
#define PT2_5     GPIO2->DRs.DR5
#define PT2_6     GPIO2->DRs.DR6
#define PT2_7     GPIO2->DRs.DR7
#define PT2_8     GPIO2->DRs.DR8
#define PT2_9     GPIO2->DRs.DR9
#define PT2_10    GPIO2->DRs.DR10
#define PT2_11    GPIO2->DRs.DR11
#define PT2_12    GPIO2->DRs.DR12
#define PT2_13    GPIO2->DRs.DR13
#define PT2_14    GPIO2->DRs.DR14
#define PT2_15    GPIO2->DRs.DR15
#define PT2_16    GPIO2->DRs.DR16
#define PT2_17    GPIO2->DRs.DR17
#define PT2_18    GPIO2->DRs.DR18
#define PT2_19    GPIO2->DRs.DR19
#define PT2_20    GPIO2->DRs.DR20
#define PT2_21    GPIO2->DRs.DR21
#define PT2_22    GPIO2->DRs.DR22
#define PT2_23    GPIO2->DRs.DR23
#define PT2_24    GPIO2->DRs.DR24
#define PT2_25    GPIO2->DRs.DR25
#define PT2_26    GPIO2->DRs.DR26
#define PT2_27    GPIO2->DRs.DR27
#define PT2_28    GPIO2->DRs.DR28
#define PT2_29    GPIO2->DRs.DR29
#define PT2_30    GPIO2->DRs.DR30
#define PT2_31    GPIO2->DRs.DR31

//定义PT3_的端口  
#define PT3_0     GPIO3->DRs.DR0
#define PT3_1     GPIO3->DRs.DR1
#define PT3_2     GPIO3->DRs.DR2
#define PT3_3     GPIO3->DRs.DR3
#define PT3_4     GPIO3->DRs.DR4
#define PT3_5     GPIO3->DRs.DR5
#define PT3_6     GPIO3->DRs.DR6
#define PT3_7     GPIO3->DRs.DR7
#define PT3_8     GPIO3->DRs.DR8
#define PT3_9     GPIO3->DRs.DR9
#define PT3_10    GPIO3->DRs.DR10
#define PT3_11    GPIO3->DRs.DR11
#define PT3_12    GPIO3->DRs.DR12
#define PT3_13    GPIO3->DRs.DR13
#define PT3_14    GPIO3->DRs.DR14
#define PT3_15    GPIO3->DRs.DR15
#define PT3_16    GPIO3->DRs.DR16
#define PT3_17    GPIO3->DRs.DR17
#define PT3_18    GPIO3->DRs.DR18
#define PT3_19    GPIO3->DRs.DR19
#define PT3_20    GPIO3->DRs.DR20
#define PT3_21    GPIO3->DRs.DR21
#define PT3_22    GPIO3->DRs.DR22
#define PT3_23    GPIO3->DRs.DR23
#define PT3_24    GPIO3->DRs.DR24
#define PT3_25    GPIO3->DRs.DR25
#define PT3_26    GPIO3->DRs.DR26
#define PT3_27    GPIO3->DRs.DR27
#define PT3_28    GPIO3->DRs.DR28
#define PT3_29    GPIO3->DRs.DR29
#define PT3_30    GPIO3->DRs.DR30
#define PT3_31    GPIO3->DRs.DR31

//定义PT4_的端口  
#define PT4_0     GPIO4->DRs.DR0
#define PT4_1     GPIO4->DRs.DR1
#define PT4_2     GPIO4->DRs.DR2
#define PT4_3     GPIO4->DRs.DR3
#define PT4_4     GPIO4->DRs.DR4
#define PT4_5     GPIO4->DRs.DR5
#define PT4_6     GPIO4->DRs.DR6
#define PT4_7     GPIO4->DRs.DR7
#define PT4_8     GPIO4->DRs.DR8
#define PT4_9     GPIO4->DRs.DR9
#define PT4_10    GPIO4->DRs.DR10
#define PT4_11    GPIO4->DRs.DR11
#define PT4_12    GPIO4->DRs.DR12
#define PT4_13    GPIO4->DRs.DR13
#define PT4_14    GPIO4->DRs.DR14
#define PT4_15    GPIO4->DRs.DR15
#define PT4_16    GPIO4->DRs.DR16
#define PT4_17    GPIO4->DRs.DR17
#define PT4_18    GPIO4->DRs.DR18
#define PT4_19    GPIO4->DRs.DR19
#define PT4_20    GPIO4->DRs.DR20
#define PT4_21    GPIO4->DRs.DR21
#define PT4_22    GPIO4->DRs.DR22
#define PT4_23    GPIO4->DRs.DR23
#define PT4_24    GPIO4->DRs.DR24
#define PT4_25    GPIO4->DRs.DR25
#define PT4_26    GPIO4->DRs.DR26
#define PT4_27    GPIO4->DRs.DR27
#define PT4_28    GPIO4->DRs.DR28
#define PT4_29    GPIO4->DRs.DR29
#define PT4_30    GPIO4->DRs.DR30
#define PT4_31    GPIO4->DRs.DR31

//定义PT5_的端口  
#define PT5_0     GPIO5->DRs.DR0
#define PT5_1     GPIO5->DRs.DR1
#define PT5_2     GPIO5->DRs.DR2
#define PT5_3     GPIO5->DRs.DR3
#define PT5_4     GPIO5->DRs.DR4
#define PT5_5     GPIO5->DRs.DR5
#define PT5_6     GPIO5->DRs.DR6
#define PT5_7     GPIO5->DRs.DR7
#define PT5_8     GPIO5->DRs.DR8
#define PT5_9     GPIO5->DRs.DR9
#define PT5_10    GPIO5->DRs.DR10
#define PT5_11    GPIO5->DRs.DR11
#define PT5_12    GPIO5->DRs.DR12
#define PT5_13    GPIO5->DRs.DR13
#define PT5_14    GPIO5->DRs.DR14
#define PT5_15    GPIO5->DRs.DR15
#define PT5_16    GPIO5->DRs.DR16
#define PT5_17    GPIO5->DRs.DR17
#define PT5_18    GPIO5->DRs.DR18
#define PT5_19    GPIO5->DRs.DR19
#define PT5_20    GPIO5->DRs.DR20
#define PT5_21    GPIO5->DRs.DR21
#define PT5_22    GPIO5->DRs.DR22
#define PT5_23    GPIO5->DRs.DR23
#define PT5_24    GPIO5->DRs.DR24
#define PT5_25    GPIO5->DRs.DR25
#define PT5_26    GPIO5->DRs.DR26
#define PT5_27    GPIO5->DRs.DR27
#define PT5_28    GPIO5->DRs.DR28
#define PT5_29    GPIO5->DRs.DR29
#define PT5_30    GPIO5->DRs.DR30
#define PT5_31    GPIO5->DRs.DR31



//定义PT1_的输出输入方向  
#define DIR1_0       GPIO1->GDIRs.GDIR0
#define DIR1_1       GPIO1->GDIRs.GDIR1
#define DIR1_2       GPIO1->GDIRs.GDIR2
#define DIR1_3       GPIO1->GDIRs.GDIR3
#define DIR1_4       GPIO1->GDIRs.GDIR4
#define DIR1_5       GPIO1->GDIRs.GDIR5
#define DIR1_6       GPIO1->GDIRs.GDIR6
#define DIR1_7       GPIO1->GDIRs.GDIR7
#define DIR1_8       GPIO1->GDIRs.GDIR8
#define DIR1_9       GPIO1->GDIRs.GDIR9
#define DIR1_10      GPIO1->GDIRs.GDIR10
#define DIR1_11      GPIO1->GDIRs.GDIR11
#define DIR1_12      GPIO1->GDIRs.GDIR12
#define DIR1_13      GPIO1->GDIRs.GDIR13
#define DIR1_14      GPIO1->GDIRs.GDIR14
#define DIR1_15      GPIO1->GDIRs.GDIR15
#define DIR1_16      GPIO1->GDIRs.GDIR16
#define DIR1_17      GPIO1->GDIRs.GDIR17
#define DIR1_18      GPIO1->GDIRs.GDIR18
#define DIR1_19      GPIO1->GDIRs.GDIR19
#define DIR1_20      GPIO1->GDIRs.GDIR20
#define DIR1_21      GPIO1->GDIRs.GDIR21
#define DIR1_22      GPIO1->GDIRs.GDIR22
#define DIR1_23      GPIO1->GDIRs.GDIR23
#define DIR1_24      GPIO1->GDIRs.GDIR24
#define DIR1_25      GPIO1->GDIRs.GDIR25
#define DIR1_26      GPIO1->GDIRs.GDIR26
#define DIR1_27      GPIO1->GDIRs.GDIR27
#define DIR1_28      GPIO1->GDIRs.GDIR28
#define DIR1_29      GPIO1->GDIRs.GDIR29
#define DIR1_30      GPIO1->GDIRs.GDIR30
#define DIR1_31      GPIO1->GDIRs.GDIR31

//定义PT2_的输出输入方向  
#define DIR2_0       GPIO2->GDIRs.GDIR0
#define DIR2_1       GPIO2->GDIRs.GDIR1
#define DIR2_2       GPIO2->GDIRs.GDIR2
#define DIR2_3       GPIO2->GDIRs.GDIR3
#define DIR2_4       GPIO2->GDIRs.GDIR4
#define DIR2_5       GPIO2->GDIRs.GDIR5
#define DIR2_6       GPIO2->GDIRs.GDIR6
#define DIR2_7       GPIO2->GDIRs.GDIR7
#define DIR2_8       GPIO2->GDIRs.GDIR8
#define DIR2_9       GPIO2->GDIRs.GDIR9
#define DIR2_10      GPIO2->GDIRs.GDIR10
#define DIR2_11      GPIO2->GDIRs.GDIR11
#define DIR2_12      GPIO2->GDIRs.GDIR12
#define DIR2_13      GPIO2->GDIRs.GDIR13
#define DIR2_14      GPIO2->GDIRs.GDIR14
#define DIR2_15      GPIO2->GDIRs.GDIR15
#define DIR2_16      GPIO2->GDIRs.GDIR16
#define DIR2_17      GPIO2->GDIRs.GDIR17
#define DIR2_18      GPIO2->GDIRs.GDIR18
#define DIR2_19      GPIO2->GDIRs.GDIR19
#define DIR2_20      GPIO2->GDIRs.GDIR20
#define DIR2_21      GPIO2->GDIRs.GDIR21
#define DIR2_22      GPIO2->GDIRs.GDIR22
#define DIR2_23      GPIO2->GDIRs.GDIR23
#define DIR2_24      GPIO2->GDIRs.GDIR24
#define DIR2_25      GPIO2->GDIRs.GDIR25
#define DIR2_26      GPIO2->GDIRs.GDIR26
#define DIR2_27      GPIO2->GDIRs.GDIR27
#define DIR2_28      GPIO2->GDIRs.GDIR28
#define DIR2_29      GPIO2->GDIRs.GDIR29
#define DIR2_30      GPIO2->GDIRs.GDIR30
#define DIR2_31      GPIO2->GDIRs.GDIR31

//定义PT3_的输出输入方向  
#define DIR3_0       GPIO3->GDIRs.GDIR0
#define DIR3_1       GPIO3->GDIRs.GDIR1
#define DIR3_2       GPIO3->GDIRs.GDIR2
#define DIR3_3       GPIO3->GDIRs.GDIR3
#define DIR3_4       GPIO3->GDIRs.GDIR4
#define DIR3_5       GPIO3->GDIRs.GDIR5
#define DIR3_6       GPIO3->GDIRs.GDIR6
#define DIR3_7       GPIO3->GDIRs.GDIR7
#define DIR3_8       GPIO3->GDIRs.GDIR8
#define DIR3_9       GPIO3->GDIRs.GDIR9
#define DIR3_10      GPIO3->GDIRs.GDIR10
#define DIR3_11      GPIO3->GDIRs.GDIR11
#define DIR3_12      GPIO3->GDIRs.GDIR12
#define DIR3_13      GPIO3->GDIRs.GDIR13
#define DIR3_14      GPIO3->GDIRs.GDIR14
#define DIR3_15      GPIO3->GDIRs.GDIR15
#define DIR3_16      GPIO3->GDIRs.GDIR16
#define DIR3_17      GPIO3->GDIRs.GDIR17
#define DIR3_18      GPIO3->GDIRs.GDIR18
#define DIR3_19      GPIO3->GDIRs.GDIR19
#define DIR3_20      GPIO3->GDIRs.GDIR20
#define DIR3_21      GPIO3->GDIRs.GDIR21
#define DIR3_22      GPIO3->GDIRs.GDIR22
#define DIR3_23      GPIO3->GDIRs.GDIR23
#define DIR3_24      GPIO3->GDIRs.GDIR24
#define DIR3_25      GPIO3->GDIRs.GDIR25
#define DIR3_26      GPIO3->GDIRs.GDIR26
#define DIR3_27      GPIO3->GDIRs.GDIR27
#define DIR3_28      GPIO3->GDIRs.GDIR28
#define DIR3_29      GPIO3->GDIRs.GDIR29
#define DIR3_30      GPIO3->GDIRs.GDIR30
#define DIR3_31      GPIO3->GDIRs.GDIR31

//定义PT4_的输出输入方向  
#define DIR4_0       GPIO4->GDIRs.GDIR0
#define DIR4_1       GPIO4->GDIRs.GDIR1
#define DIR4_2       GPIO4->GDIRs.GDIR2
#define DIR4_3       GPIO4->GDIRs.GDIR3
#define DIR4_4       GPIO4->GDIRs.GDIR4
#define DIR4_5       GPIO4->GDIRs.GDIR5
#define DIR4_6       GPIO4->GDIRs.GDIR6
#define DIR4_7       GPIO4->GDIRs.GDIR7
#define DIR4_8       GPIO4->GDIRs.GDIR8
#define DIR4_9       GPIO4->GDIRs.GDIR9
#define DIR4_10      GPIO4->GDIRs.GDIR10
#define DIR4_11      GPIO4->GDIRs.GDIR11
#define DIR4_12      GPIO4->GDIRs.GDIR12
#define DIR4_13      GPIO4->GDIRs.GDIR13
#define DIR4_14      GPIO4->GDIRs.GDIR14
#define DIR4_15      GPIO4->GDIRs.GDIR15
#define DIR4_16      GPIO4->GDIRs.GDIR16
#define DIR4_17      GPIO4->GDIRs.GDIR17
#define DIR4_18      GPIO4->GDIRs.GDIR18
#define DIR4_19      GPIO4->GDIRs.GDIR19
#define DIR4_20      GPIO4->GDIRs.GDIR20
#define DIR4_21      GPIO4->GDIRs.GDIR21
#define DIR4_22      GPIO4->GDIRs.GDIR22
#define DIR4_23      GPIO4->GDIRs.GDIR23
#define DIR4_24      GPIO4->GDIRs.GDIR24
#define DIR4_25      GPIO4->GDIRs.GDIR25
#define DIR4_26      GPIO4->GDIRs.GDIR26
#define DIR4_27      GPIO4->GDIRs.GDIR27
#define DIR4_28      GPIO4->GDIRs.GDIR28
#define DIR4_29      GPIO4->GDIRs.GDIR29
#define DIR4_30      GPIO4->GDIRs.GDIR30
#define DIR4_31      GPIO4->GDIRs.GDIR31

//定义PT5_的输出输入方向  
#define DIR5_0       GPIO5->GDIRs.GDIR0
#define DIR5_1       GPIO5->GDIRs.GDIR1
#define DIR5_2       GPIO5->GDIRs.GDIR2
#define DIR5_3       GPIO5->GDIRs.GDIR3
#define DIR5_4       GPIO5->GDIRs.GDIR4
#define DIR5_5       GPIO5->GDIRs.GDIR5
#define DIR5_6       GPIO5->GDIRs.GDIR6
#define DIR5_7       GPIO5->GDIRs.GDIR7
#define DIR5_8       GPIO5->GDIRs.GDIR8
#define DIR5_9       GPIO5->GDIRs.GDIR9
#define DIR5_10      GPIO5->GDIRs.GDIR10
#define DIR5_11      GPIO5->GDIRs.GDIR11
#define DIR5_12      GPIO5->GDIRs.GDIR12
#define DIR5_13      GPIO5->GDIRs.GDIR13
#define DIR5_14      GPIO5->GDIRs.GDIR14
#define DIR5_15      GPIO5->GDIRs.GDIR15
#define DIR5_16      GPIO5->GDIRs.GDIR16
#define DIR5_17      GPIO5->GDIRs.GDIR17
#define DIR5_18      GPIO5->GDIRs.GDIR18
#define DIR5_19      GPIO5->GDIRs.GDIR19
#define DIR5_20      GPIO5->GDIRs.GDIR20
#define DIR5_21      GPIO5->GDIRs.GDIR21
#define DIR5_22      GPIO5->GDIRs.GDIR22
#define DIR5_23      GPIO5->GDIRs.GDIR23
#define DIR5_24      GPIO5->GDIRs.GDIR24
#define DIR5_25      GPIO5->GDIRs.GDIR25
#define DIR5_26      GPIO5->GDIRs.GDIR26
#define DIR5_27      GPIO5->GDIRs.GDIR27
#define DIR5_28      GPIO5->GDIRs.GDIR28
#define DIR5_29      GPIO5->GDIRs.GDIR29
#define DIR5_30      GPIO5->GDIRs.GDIR30
#define DIR5_31      GPIO5->GDIRs.GDIR31


//定义PT1_的8位端口  
#define PT1_BYTE0   GPIO1->DRByte.Byte0
#define PT1_BYTE1   GPIO1->DRByte.Byte1
#define PT1_BYTE2   GPIO1->DRByte.Byte2
#define PT1_BYTE3   GPIO1->DRByte.Byte3

//定义PT2_的8位端口  
#define PT2_BYTE0   GPIO2->DRByte.Byte0
#define PT2_BYTE1   GPIO2->DRByte.Byte1
#define PT2_BYTE2   GPIO2->DRByte.Byte2
#define PT2_BYTE3   GPIO2->DRByte.Byte3


//定义PT3_的8位端口  
#define PT3_BYTE0   GPIO3->DRByte.Byte0
#define PT3_BYTE1   GPIO3->DRByte.Byte1
#define PT3_BYTE2   GPIO3->DRByte.Byte2
#define PT3_BYTE3   GPIO3->DRByte.Byte3

//定义PT4_的8位端口  
#define PT4_BYTE0   GPIO4->DRByte.Byte0
#define PT4_BYTE1   GPIO4->DRByte.Byte1
#define PT4_BYTE2   GPIO4->DRByte.Byte2
#define PT4_BYTE3   GPIO4->DRByte.Byte3

//定义PT5_的8位端口  
#define PT5_BYTE0   GPIO5->DRByte.Byte0
#define PT5_BYTE1   GPIO5->DRByte.Byte1
#define PT5_BYTE2   GPIO5->DRByte.Byte2
#define PT5_BYTE3   GPIO5->DRByte.Byte3


//定义PT1_的8位输出输入方向  
#define DIR1_BYTE0   GPIO1->DIR2_yte.Byte0
#define DIR1_BYTE1   GPIO1->DIR2_yte.Byte1
#define DIR1_BYTE2   GPIO1->DIR2_yte.Byte2
#define DIR1_BYTE3   GPIO1->DIR2_yte.Byte3

//定义PT2_的8位输出输入方向  
#define DIR2_BYTE0   GPIO2->DIR2_yte.Byte0
#define DIR2_BYTE1   GPIO2->DIR2_yte.Byte1
#define DIR2_BYTE2   GPIO2->DIR2_yte.Byte2
#define DIR2_BYTE3   GPIO2->DIR2_yte.Byte3

//定义PT3_的8位输出输入方向  
#define DIR3_BYTE0   GPIO3->DIR2_yte.Byte0
#define DIR3_BYTE1   GPIO3->DIR2_yte.Byte1
#define DIR3_BYTE2   GPIO3->DIR2_yte.Byte2
#define DIR3_BYTE3   GPIO3->DIR2_yte.Byte3

//定义PT4_的8位输出输入方向  
#define DIR4_BYTE0   GPIO4->DIR2_yte.Byte0
#define DIR4_BYTE1   GPIO4->DIR2_yte.Byte1
#define DIR4_BYTE2   GPIO4->DIR2_yte.Byte2
#define DIR4_BYTE3   GPIO4->DIR2_yte.Byte3

//定义PT5_的8位输出输入方向  
#define DIR5_BYTE0   GPIO5->DIR2_yte.Byte0
#define DIR5_BYTE1   GPIO5->DIR2_yte.Byte1 
#define DIR5_BYTE2   GPIO5->DIR2_yte.Byte2
#define DIR5_BYTE3   GPIO5->DIR2_yte.Byte3


//定义PT1_的16位端口  
#define PT1_WORD0   GPIO1->DRWord.Word0
#define PT1_WORD1   GPIO1->DRWord.Word1

//定义PT2_的16位端口  
#define PT2_WORD0   GPIO2->DRWord.Word0
#define PT2_WORD1   GPIO2->DRWord.Word1

//定义PT3_的16位端口  
#define PT3_WORD0   GPIO3->DRWord.Word0
#define PT3_WORD1   GPIO3->DRWord.Word1


//定义PT4_的16位端口  
#define PT4_WORD0   GPIO4->DRWord.Word0
#define PT4_WORD1   GPIO4->DRWord.Word1


//定义PT5_的16位端口  
#define PT5_WORD0   GPIO5->DRWord.Word0
#define PT5_WORD1   GPIO5->DRWord.Word1


//定义PT1_的16位输出输入方向  
#define DIR1_WORD0   GPIO1->GDIRWord.Word0
#define DIR1_WORD1   GPIO1->GDIRWord.Word1


//定义PT2_的16位输出输入方向  
#define DIR2_WORD0   GPIO2->GDIRWord.Word0
#define DIR2_WORD1   GPIO2->GDIRWord.Word1


//定义PT3_的16位输出输入方向  
#define DIR3_WORD0   GPIO3->GDIRWord.Word0
#define DIR3_WORD1   GPIO3->GDIRWord.Word1


//定义PT4_的16位输出输入方向  
#define DIR4_WORD0   GPIO4->GDIRWord.Word0
#define DIR4_WORD1   GPIO4->GDIRWord.Word1


//定义PT5_的16位输出输入方向  
#define DIR5_WORD0   GPIO5->GDIRWord.Word0
#define DIR5_WORD1   GPIO5->GDIRWord.Word1

typedef enum
{
    A2     , A3    ,A4     ,A5     ,A6     ,A7     ,A8     ,A9     ,A10    ,A11    ,A12    ,A13    ,
      
    B1     ,B2     ,B3     ,B4     ,B6     ,B7     ,B8     ,B9     ,B11    ,B12    ,B13    ,B14    ,
    
    C1     ,C2     ,C3     ,C4     ,C5     ,C6     ,C7     ,C8     ,C9     ,C10    ,C11    ,C12    ,C13    ,C14    ,
    
    D1     ,D2     ,D3     ,D4     ,D5     ,D6     ,D7     ,D8     ,D9     ,D10    ,D11    ,D12    ,D13    ,D14    ,
    
    E1     ,E3     ,E4     ,E5     ,E7     ,E8     ,E10    ,E11    ,E12    ,E14    ,
    
    F1     ,F2     ,F3     ,F4     ,F11    ,F12    ,F13    ,F14    ,
        
    G1     ,G2     ,G3     ,G4     ,G5     ,G10    ,G11    ,G12    ,G13    ,G14  ,  
    
    H1     ,H2     ,H3     ,H4     ,H5     ,H10    ,H11    ,H12    ,H13    ,H14  ,
      
    J1     ,J2     ,J3     ,J4     ,J11    ,J12    ,J13    ,J14    ,
     
    K1     ,K7     ,K10    ,K11    ,K12    ,K14    ,
  
    L3     ,L4     ,L5     ,L6     ,L7     ,L10    ,L11    ,L12    ,L13    ,L14  ,   
    
    M3     ,M4     ,M5     ,M11    ,M12    ,M13    ,M14    ,
       
    N3     ,N4     , 
      
    P2     ,P3     ,P4     ,P5  , 
     
}GPIO_Name_t;

//#define A2           GPIO4, 27
//#define A3           GPIO4, 20
//#define A4           GPIO4, 17
//#define A5           GPIO4, 16
//#define A6           GPIO4, 13
//#define A7           GPIO3, 26
//#define A8           GPIO2, 06
//#define A9           GPIO2, 07
//#define A10          GPIO2, 11
//#define A11          GPIO2, 16
//#define A12          GPIO2, 24
//#define A13          GPIO2, 25
//
//
//#define B1           GPIO4, 15
//#define B2           GPIO4, 18
//#define B3           GPIO4, 26
//#define B4           GPIO4, 19
//#define B6           GPIO4, 14
//#define B7           GPIO3, 25
//#define B8           GPIO2, 05
//#define B9           GPIO2, 08
//#define B11          GPIO2, 17
//#define B12          GPIO2, 23
//#define B13          GPIO2, 26
//#define B14          GPIO2, 31
//
//
//#define C1           GPIO4, 21
//#define C2           GPIO4, 09
//#define C3           GPIO3, 22
//#define C4           GPIO3, 19
//#define C5           GPIO4, 31
//#define C6           GPIO4, 30
//#define C7           GPIO3, 27
//#define C8           GPIO2, 04
//#define C9           GPIO2, 09
//#define C10          GPIO2, 12
//#define C11          GPIO2, 18
//#define C12          GPIO2, 22
//#define C13          GPIO2, 27
//#define C14          GPIO2, 30
//
//
//#define D1           GPIO4, 28
//#define D2           GPIO4, 25
//#define D3           GPIO4, 24
//#define D4           GPIO3, 20
//#define D5           GPIO3, 18
//#define D6           GPIO3, 24
//#define D7           GPIO2, 00
//#define D8           GPIO2, 03
//#define D9           GPIO2, 10
//#define D10          GPIO2, 13
//#define D11          GPIO2, 19
//#define D12          GPIO2, 21
//#define D13          GPIO2, 28
//#define D14          GPIO2, 29
//
//
//#define E1           GPIO4, 29
//#define E3           GPIO4, 00
//#define E4           GPIO3, 23
//#define E5           GPIO3, 21
//#define E7           GPIO2, 01
//#define E8           GPIO2, 02
//#define E10          GPIO2, 14
//#define E11          GPIO2, 15
//#define E12          GPIO2, 20
//#define E14          GPIO1, 06
//
//
//
//#define F1           GPIO4, 22
//#define F2           GPIO4, 04
//#define F3           GPIO4, 01
//#define F11          GPIO1, 04
//#define F12          GPIO1, 07
//#define F13          GPIO1, 08
//#define F14          GPIO1, 09
//
//
//#define G1           GPIO4, 10
//#define G2           GPIO4, 23
//#define G3           GPIO4, 11
//#define G4           GPIO4, 03
//#define G5           GPIO4, 05
//#define G10          GPIO1, 11
//#define G11          GPIO1, 03
//#define G12          GPIO1, 30
//#define G13          GPIO1, 10
//#define G14          GPIO1, 05
//
//
//
//#define H1           GPIO4, 12
//#define H2           GPIO3, 16
//#define H3           GPIO4, 08
//#define H4           GPIO4, 07
//#define H5           GPIO4, 06
//#define H10          GPIO1, 01
//#define H11          GPIO1, 29
//#define H12          GPIO1, 28
//#define H13          GPIO1, 24
//#define H14          GPIO1, 14
//
//
//#define J1           GPIO3, 14
//#define J2           GPIO3, 17
//#define J3           GPIO3, 13
//#define J4           GPIO3, 12
//#define J11          GPIO1, 16
//#define J12          GPIO1, 22
//#define J13          GPIO1, 27
//#define J14          GPIO1, 31
//
//
//
//#define K1           GPIO3, 15
//#define K7           GPIO5, 01
//#define K10          GPIO1, 23
//#define K11          GPIO1, 17
//#define K12          GPIO1, 21
//#define K14          GPIO1, 12
//
//
//
//#define L3           GPIO3, 06
//#define L4           GPIO3, 07
//#define L5           GPIO3, 00
//#define L6           GPIO5, 00
//#define L7           GPIO5, 02
//#define L10          GPIO1, 15
//#define L11          GPIO1, 18
//#define L12          GPIO1, 20
//#define L13          GPIO1, 26
//#define L14          GPIO1, 13
//
//
//
//#define M3           GPIO3, 02
//#define M4           GPIO3, 03
//#define M5           GPIO3, 01
//#define M11          GPIO1, 02
//#define M12          GPIO1, 19
//#define M13          GPIO1, 25
//#define M14          GPIO1, 00
//
//
//
//#define N3           GPIO3, 05
//#define N4           GPIO3, 09
//
//
//
//#define P2           GPIO3, 04
//#define P3           GPIO3, 08
//#define P4           GPIO3, 10
//#define P5           GPIO3, 11



#endif
