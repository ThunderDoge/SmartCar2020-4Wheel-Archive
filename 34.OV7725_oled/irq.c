/*-------------------------------------------------------
【平    台】龙邱i.MX RT1064核心板-智能车板
【编    写】LQ-005
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2019年3月12日
【dev.env.】IAR8.30.1及以上版本
【Target 】 i.MX RT1064
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 600MHz
【USB PLL】 480MHz
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
【注   意】 中断服务函数 不知道名字可以去startup_MIMXRT1064.s里面找
---------------------------------------------------------*/
#include "LQ_GPIO_Cfg.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_lpuart.h"
#include "stdio.h"


