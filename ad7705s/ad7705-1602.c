 //--------------------------AD7705------------------------
//���ܣ�ADC���ɱ�̿�������ͽ�ֹƵ��
//-------------------------------------------------------
/***************************************************************
*������������
*���������ΪAD7705�ĵ��Գ����ܹ�ʹAD7705������16������
*˫���Ի���ģʽ�¹���������Ƶ��Ϊ200HZ�������˲����Ľ�ֹƵ��Ϊ52.4HZ
*�������F020��ֻҪ�ı����¶˿ڼ��ɣ�
*sbit AD7705_DATA=P3^0;       // AD7705 input & output data
*sbit AD7705_CLK=P3^1;        // AD7705 ���ݴ���ʱ��
*sbit AD7705_DRDY=P1^2;       // AD7705 ��������������λ
*sbit AD7705_CS  =P1^3;       // AD7705 Ƭѡ
*�������������ȫ�ֱ�����unsigned char MSB_Data,LSB_Data��
*����ǰ�ߴ���߰�λ�����ߴ���ڰ�λ
*******************************************************************/

#include<reg52.h>
#include<absacc.h>
#include<intrins.h>
#include<math.h>

sbit RS = P2^7;   //����˿� 
sbit RW = P2^6;
sbit EN = P2^5;


sbit R_S = P1^1; 

#define RS_CLR RS=0 
#define RS_SET RS=1
#define RW_CLR RW=0 
#define RW_SET RW=1 
#define EN_CLR EN=0
#define EN_SET EN=1				//Һ����ʾ����
#define AD_CLK_WIDTH 1 /*AD7705����ʱ���������*/
//-------------------------����ͨѶ----------------------
sbit AD7705_DATA=P1^2;       // AD7705 �����������
sbit AD7705_CLK=P1^3;        // AD7705 ���ݴ���ʱ��
sbit AD7705_DRDY=P1^5;       // AD7705 ��������������λ
sbit AD7705_CS=P1^0;       // AD7705 Ƭѡ
unsigned char MSB_Data,LSB_Data;

unsigned char Timerint1,Timerint2,Sec,Min,H,Alm,AT,AlmTime;
unsigned int ndata;
			

unsigned char no[6];

/******************************************************************/
/*                    ΢����ʱ����                                */
/******************************************************************/
void delay_us(unsigned int n) //��ʱ �����Ҫ�߾�����ʱ ��Ƕ����
{ 
 for( ;n>0 ;n-- );
 }
/******************************************************************/
/*                    ���뺯������                                */
/******************************************************************/
 void delay_ms(unsigned char i) 
 { 
 unsigned char a, b; 
 for (a = 1; a < i; a++) 
 {
   for (b = 1; b; b++) 
   {   ;   } 
    }
 }
/******************************************************************/
/*                   д�������                                 */
/******************************************************************/
 void LCD_write_com(unsigned char com) 
 {  
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 P0 = com; 
 delay_us(5); 
 EN_CLR;
 }
/******************************************************************/
/*                   д�����ݺ���                                 */
/******************************************************************/
 void LCD_write_Data(unsigned char Data) 
 { 
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 P0 = Data; 
 delay_us(5); 
 EN_CLR;
 }
/******************************************************************/
/*                   ��������                                     */
/******************************************************************/
 void LCD_clear(void) 
 { 
 LCD_write_com(0x01); 
 delay_ms(5);}
/******************************************************************/
/*                   д���ַ�������                               */
/******************************************************************/
 void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 {     
 LCD_write_com(0x80 + x);     
 }
 else 
 {     
 LCD_write_com(0xC0 + x);     
 }        
 while (*s) 
 {     
 LCD_write_Data( *s);     
 s ++;     
 }
 }

/******************************************************************/
/*                   ��ʼ������                                   */
/******************************************************************/
 void LCD_init(void) 
 {
   LCD_write_com(0x38);    /*��ʾģʽ����*/ 
   delay_ms(5); 
   LCD_write_com(0x38); 
   delay_ms(5); 
   LCD_write_com(0x38); 
   delay_ms(5); 
   LCD_write_com(0x38);  
   LCD_write_com(0x08);    /*��ʾ�ر�*/ 
   LCD_write_com(0x01);    /*��ʾ����*/ 
   LCD_write_com(0x06);    /*��ʾ����ƶ�����*/ 
   delay_ms(5); 
   LCD_write_com(0x0C);    /*��ʾ�����������*/
   }
   
/******************************************************************/
/*                   ����                                   */
/******************************************************************/

//--------------------------------------------------------------------
//��������AD7705_Write_Reg()
//���ܣ���AD7705�еĸ��Ĵ�������д����
//������Ҫ��AD7705���Ĵ���д��ÿ�����
//����ֵ����
//��������
//��ע����
//--------------------------------------------------------------------
void AD7705_Write_Reg(unsigned char Data_byte)
{ 
 unsigned char i;
 AD7705_CS = 0;						 //ѡͨƬѡ
 AD7705_CLK = 1;					 
        _nop_();					 //��ʱ�ɲ�Ҫ
  		_nop_();
 for(i = 0; i < 8; i++) 
 {
     AD7705_DATA = (bit)(Data_byte & 0x80);			//ȡλֵ
 	 AD7705_CLK = 0;
     _nop_();
 	 _nop_();
  	 _nop_();
  	 AD7705_CLK = 1;
     Data_byte <<= 1;							   //������λ
 }
  _nop_();
AD7705_CS = 1;								   //��ֹƬѡ
}

//-------------------------------------------------------------------
//��������AD7705_Read_Reg
//���ܣ���AD7705�и��Ĵ���
//��������
//����ֵ����AD7705�Ĵ����ж���������
//��������
//��ע����
//-------------------------------------------------------------------
unsigned char AD7705_Read_Reg()
{ 
 unsigned char i;
 unsigned char Data_read;

 AD7705_CS = 0;										 //ѡͨƬѡ
 AD7705_CLK = 1;
 _nop_();
 for(i = 0; i < 8; i++) 
 {
     AD7705_CLK = 0;
     _nop_();
     _nop_();
  	Data_read <<= 1;
  	Data_read |= (unsigned char)AD7705_DATA;
  	_nop_();
  	_nop_();
    AD7705_CLK = 1;
    _nop_();
    _nop_();
 }
   _nop_();
	AD7705_CS = 1;
  while (! AD7705_DRDY);
  return(Data_read);
}
//------------------------------------------------------------------
//��������AD7705_Start
//���ܣ���AD7705�ĸ��Ĵ���д���ʼ������
//��������
//����ֵ����
//��������
//��ע����
//------------------------------------------------------------------
void AD7705_Start(unsigned char n)
{ 
  unsigned char i;
 delay_ms(50); 
 
for(i=0;i<35;i++)
{ /* ��������32�� DIN=1 ʹ���ڸ�λ */
   AD7705_CLK=0;
   delay_us(AD_CLK_WIDTH);
   AD7705_DATA =1;
   delay_us(AD_CLK_WIDTH);
   AD7705_CLK=1;
   delay_us(AD_CLK_WIDTH);
}
 AD7705_Write_Reg(0x20);			 //������һ��дʱ�ӼĴ���������ѡ���һ�Բ��ͨ��
 AD7705_Write_Reg(0x08); //�������Ƶ��20HZ���˲�����ֹƵ��Ϊ5.24HZ
                         //���ж���Ƶ���ҹ���Ƶ�ʴ���2M
 AD7705_Write_Reg(0x10);		   //ѡ�����üĴ������ҽ���д����
 AD7705_Write_Reg(0x7a);		   //��У׼������Ϊ128��˫���Ի���ģ��
 AD7705_Write_Reg(0x21);
 AD7705_Write_Reg(0x08);			 
 AD7705_Write_Reg(0x11);		   //ѡ�����üĴ������ҽ���д����
 AD7705_Write_Reg(0x7a);           //��У׼������Ϊ128��˫���Ի���ģ��		   
}

//------------------------------------------------------------------
//��������AD7705_Read
//���ܣ���AD7705���ݼĴ����ж�����
//��������
//����ֵ����
//��������
//��ע����
//-------------------------------------------------------------------
void AD7705_Read(unsigned char n)
{ 
 	
 AD7705_Write_Reg(0x38+n);	   	//ѡ��AD7705�е����ݼĴ���
 while(AD7705_DRDY);		
 MSB_Data = AD7705_Read_Reg();			//����AD7705�����ݼĴ����ĸ߰�λ
 LSB_Data = AD7705_Read_Reg();			//����AD7705�����ݼĴ����ĵͰ�λ
 ndata= MSB_Data*256+ LSB_Data;
 if(ndata>32768)
  {
  ndata=ndata-32768;
  }
  else
  ndata=32768-ndata;

}




//--------------------------------------------------------------
//��������appear
//���ܣ�Һ����ʾ�ɼ�������
//��������
//��������
//����ֵ����
//��ע����
//----

void appear(unsigned char n)
{     
	unsigned char i;
	
	for(i=0;i<5;i++)
		{
			switch (i)
			{
				case 0:no[0]=ndata/10000+48;break;
				case 1:no[1]=ndata%10000/1000+48;break;
				case 2:no[2]=ndata%1000/100+48;break;
				case 3:no[3]=ndata%100/10+48;break;
				case 4:no[4]=ndata%10+48;break;	
			}
			LCD_write_com(0x80+i+n*0x40); 
			LCD_write_Data(no[i] );
		}
 

}

/******************************************************************/
/*                   ������                                       */
/******************************************************************/  
main()
{
 
LCD_init(); 
 
EA = 1;           //����CPU�ж�
ET0 = 1;          //��ʱ��0�жϴ�
TMOD = 0x51;      //�趨ʱ��0,1Ϊģʽ1��16λģʽ
TH1=0x00; TL1=0x00;
TH0=0x3c; TL0=0xb0;     //�趨ʱֵΪ50ms
TR0 = 1;
          //��ʼ��ʱ

 delay_ms(100);
while(1)
{  
		R_S=0;
		delay_us(10); 
		R_S=1;
		delay_us(100);
		AD7705_Start(0);
		AD7705_Read(0);						//��AD7705���ݼĴ���������
		appear(0);							
		delay_ms(10); 
    AD7705_Start(1); 
    AD7705_Read(1);						//��AD7705���ݼĴ���������
		appear(1);							
		delay_ms(1); 
 }
}

/*******/

void TM_Time0(void) interrupt 1 //��ʱ��0�ж�
{ 

   unsigned char Dm;
    TH0=0x3c;TL0=0xb0;
	TR0=1;
    Timerint1++; 
  if(Timerint1==10)
   { 
    Timerint2++; 

    if(Timerint2==2)
	 {
	  Timerint2=0;
      Sec++; 
      if (Dm==0)Dm=1;

	  if (Sec>59)
	    {
		if(Alm==1)
		{AT++;
		 if(AT>AlmTime)AT=AlmTime;}
		 else AT=0;
		Min++;
		Sec=0;
		if(Min>10)Dm=0;
		 if(Min>59)
		  {
		  H++;
		  Min=0;
		  if(H==24)H=0;
		  }
		} 
	 }
	Timerint1=0;
	}		
}