/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 硬件底层驱动程序集合
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2012-10-12 armfly  ST固件库版本 V2.1.0
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	bsp_InitUart(); 	/* 初始化串口 */
}


void bsp_Idle(void)
{
	;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DelayMS
*	功能说明: 延时函数。按照89C52调试， 12T指令周期， 11.0592晶振
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DelayMS(uint16_t n)
{
	/* 安富莱电子: 2014-04-23
	   以下代码，已经通过逻辑分析验证。	bsp_DelayMS(500) 时，实际延迟498ms 
	*/
	uint16_t k;

	while(n--)
	{
		for (k = 0; k < 75; k++);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
