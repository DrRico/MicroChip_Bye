/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : Ӳ���ײ��������򼯺�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-10-12 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
	bsp_InitUart(); 	/* ��ʼ������ */
}


void bsp_Idle(void)
{
	;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayMS
*	����˵��: ��ʱ����������89C52���ԣ� 12Tָ�����ڣ� 11.0592����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayMS(uint16_t n)
{
	/* ����������: 2014-04-23
	   ���´��룬�Ѿ�ͨ���߼�������֤��	bsp_DelayMS(500) ʱ��ʵ���ӳ�498ms 
	*/
	uint16_t k;

	while(n--)
	{
		for (k = 0; k < 75; k++);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/