/*
*********************************************************************************************************
*
*	模块名称 : 主程序
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : TM7705读写例程
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2014-04-20 发布1.0版本
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"STC89C52_TM7705双路ADC模块例程"
#define EXAMPLE_DATE	"2014-04-20"
#define DEMO_VER		"1.0"

static void PrintfLogo(void);
static void PrintfHardInfo(void);
static void TM7705_Demo(void);

code char sum_P0[16][13] = {{0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef},
							{0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0},
							{0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01},
							{0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12},
							{0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23},
							{0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34},
							{0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45},
							{0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56},
							{0xab,0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67},
							{0xbc,0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78},
							{0xcd,0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89},
							{0xde,0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a},
							{0xef,0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab},
							{0xf0,0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc},
							{0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd},
							{0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde}};

code char sum_P2[16] = {0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef,0xf0};

int i,j;

void main(void)
{
	bsp_Init();     		/* 初始化底层硬件,该函数在 bsp.c文件 */
	PrintfLogo();		/* 打印例程logo */
	// PrintfHardInfo();	/* 打印硬件接线信息 */
	TM7705_Demo();
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_Demo
*	功能说明: 定时读取TM7705的ADC值，并打印到串口。请通过PC机串口工具查看结果。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void TM7705_Demo(void)
{
	uint16_t adc1, adc2;
	
	bsp_InitTM7705();			/* 初始化配置TM7705 */

	TM7705_CalibSelf(1);		/* 自校准。执行时间较长，约180ms */		
	adc1 = TM7705_ReadAdc(1);				
	
	TM7705_CalibSelf(2);		/* 自校准。执行时间较长，约180ms */
	adc2 = TM7705_ReadAdc(2);	
	
		
	for(j=0;j<16;j++){
		for(i=0;i<13;i++){
			bsp_Idle();		
			P0 = sum_P0[j][i];
			P2 = sum_P2[j];
			printf("j:%d----i:%d----P2:%x==",j,i,sum_P2[j]);
			printf("==P0:%x====",sum_P0[j][i]);
			bsp_DelayMS(150);

			#if 1	
				/* 双通道切换采样，执行一轮实际那约 160ms */			
				adc1 = TM7705_ReadAdc(1);	/* 执行时间 80ms */	
				adc2 = TM7705_ReadAdc(2);			
				//adc2 = TM7705_ReadAdc(2);	/* 执行时间 80ms */
			#else
				/* 如果只采集1个通道，则刷新速率 50Hz （缺省设置的，最大可以设置为500Hz） */
				adc1 = TM7705_ReadAdc(1);	/* 执行时间 20ms (50Hz速率刷新时） */		
				adc2 = 0;
			#endif
				
			/* 打印采集数据 */
			{
				int volt1, volt2;
				
				/* 计算实际电压值（近似估算的），如需准确，请进行校准 */
				volt1 = ((int32_t)adc1 * 5000) / 65535;
				volt2 = ((int32_t)adc2 * 5000) / 65535;
				
				/* 打印ADC采样结果 */	
				// printf("CH1=%5ld (%5dmV) \r CH2=%5ld (%5dmV)\r\n", (long int)adc1, volt1, (long int)adc2, volt2);
				
				printf("CH1=%5ld (%5dmV) \r\n", (long int)adc1, volt1);

				bsp_DelayMS(250);
			}
		}
	}
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* 友情提示：
		使用STVD开发时，printf/sprintf 入口参数 %d 必须是 int 类型(16位)，否则打印结果不正确。
		IAR无此问题。
	*/

	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	// 打印例程名称 //
	printf("* 例程版本   : %s\r\n", DEMO_VER);		// 打印例程版本 //
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	// 打印例程日期 //
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/*
*********************************************************************************************************
*	函 数 名: PrintfHardInfo
*	功能说明: 打印硬件接线信息
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfHardInfo(void)
{
	/*printf("接线方法: \r\n");
	printf("AD7705模块    51开发板(STC89C52, 11.0592M晶振)\r\n");
    printf("  VCC   <------  5.0V      5V供电\r\n");
    printf("  GND   -------  GND       地\r\n");	  
	printf("  CS    <------  P1.0      SPI片选\r\n");
	printf("  RST   <------  P1.1      SPI复位\r\n");
	printf("  DIN   <------  P1.2      SPI MOSI\r\n");
	printf("  SCK   <------  P1.3      SPI时钟\r\n");
	printf("  DOUT  -------> P1.4      SPI MISO\r\n");
	printf("  DRDY  <------> P1.5      满信号，数据准备好\r\n");*/
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

