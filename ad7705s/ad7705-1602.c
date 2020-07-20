 //--------------------------AD7705------------------------
//功能：ADC，可编程控制增益和截止频率
//-------------------------------------------------------
/***************************************************************
*程序功能描述：
*本程序可作为AD7705的调试程序，能够使AD7705工作在16倍增益
*双极性缓冲模式下工作，更新频率为200HZ，数字滤波器的截止频率为52.4HZ
*如果用在F020中只要改变如下端口即可：
*sbit AD7705_DATA=P3^0;       // AD7705 input & output data
*sbit AD7705_CLK=P3^1;        // AD7705 数据传输时钟
*sbit AD7705_DRDY=P1^2;       // AD7705 数据输入输出检测位
*sbit AD7705_CS  =P1^3;       // AD7705 片选
*数据输出储存在全局变量：unsigned char MSB_Data,LSB_Data中
*其中前者储存高八位，后者储存第八位
*******************************************************************/

#include<reg52.h>
#include<absacc.h>
#include<intrins.h>
#include<math.h>

sbit RS = P2^7;   //定义端口 
sbit RW = P2^6;
sbit EN = P2^5;


sbit R_S = P1^1; 

#define RS_CLR RS=0 
#define RS_SET RS=1
#define RW_CLR RW=0 
#define RW_SET RW=1 
#define EN_CLR EN=0
#define EN_SET EN=1				//液晶显示代码
#define AD_CLK_WIDTH 1 /*AD7705串口时钟脉冲宽度*/
//-------------------------串口通讯----------------------
sbit AD7705_DATA=P1^2;       // AD7705 数据输入输出
sbit AD7705_CLK=P1^3;        // AD7705 数据传输时钟
sbit AD7705_DRDY=P1^5;       // AD7705 数据输入输出检测位
sbit AD7705_CS=P1^0;       // AD7705 片选
unsigned char MSB_Data,LSB_Data;

unsigned char Timerint1,Timerint2,Sec,Min,H,Alm,AT,AlmTime;
unsigned int ndata;
			

unsigned char no[6];

/******************************************************************/
/*                    微秒延时函数                                */
/******************************************************************/
void delay_us(unsigned int n) //延时 如果需要高精度延时 请嵌入汇编
{ 
 for( ;n>0 ;n-- );
 }
/******************************************************************/
/*                    毫秒函数声明                                */
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
/*                   写入命令函数                                 */
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
/*                   写入数据函数                                 */
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
/*                   清屏函数                                     */
/******************************************************************/
 void LCD_clear(void) 
 { 
 LCD_write_com(0x01); 
 delay_ms(5);}
/******************************************************************/
/*                   写入字符串函数                               */
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
/*                   初始化函数                                   */
/******************************************************************/
 void LCD_init(void) 
 {
   LCD_write_com(0x38);    /*显示模式设置*/ 
   delay_ms(5); 
   LCD_write_com(0x38); 
   delay_ms(5); 
   LCD_write_com(0x38); 
   delay_ms(5); 
   LCD_write_com(0x38);  
   LCD_write_com(0x08);    /*显示关闭*/ 
   LCD_write_com(0x01);    /*显示清屏*/ 
   LCD_write_com(0x06);    /*显示光标移动设置*/ 
   delay_ms(5); 
   LCD_write_com(0x0C);    /*显示开及光标设置*/
   }
   
/******************************************************************/
/*                   消磁                                   */
/******************************************************************/

//--------------------------------------------------------------------
//函数名：AD7705_Write_Reg()
//功能：对AD7705中的各寄存器进行写操作
//参数：要对AD7705各寄存器写入得控制字
//返回值：无
//变量：无
//备注：无
//--------------------------------------------------------------------
void AD7705_Write_Reg(unsigned char Data_byte)
{ 
 unsigned char i;
 AD7705_CS = 0;						 //选通片选
 AD7705_CLK = 1;					 
        _nop_();					 //延时可不要
  		_nop_();
 for(i = 0; i < 8; i++) 
 {
     AD7705_DATA = (bit)(Data_byte & 0x80);			//取位值
 	 AD7705_CLK = 0;
     _nop_();
 	 _nop_();
  	 _nop_();
  	 AD7705_CLK = 1;
     Data_byte <<= 1;							   //向左移位
 }
  _nop_();
AD7705_CS = 1;								   //禁止片选
}

//-------------------------------------------------------------------
//函数名：AD7705_Read_Reg
//功能：读AD7705中各寄存器
//参数：无
//返回值：从AD7705寄存器中读出的数据
//变量：无
//备注：无
//-------------------------------------------------------------------
unsigned char AD7705_Read_Reg()
{ 
 unsigned char i;
 unsigned char Data_read;

 AD7705_CS = 0;										 //选通片选
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
//函数名：AD7705_Start
//功能：向AD7705的各寄存器写入初始控制字
//参数：无
//返回值：无
//变量：无
//备注：无
//------------------------------------------------------------------
void AD7705_Start(unsigned char n)
{ 
  unsigned char i;
 delay_ms(50); 
 
for(i=0;i<35;i++)
{ /* 多于连续32个 DIN=1 使串口复位 */
   AD7705_CLK=0;
   delay_us(AD_CLK_WIDTH);
   AD7705_DATA =1;
   delay_us(AD_CLK_WIDTH);
   AD7705_CLK=1;
   delay_us(AD_CLK_WIDTH);
}
 AD7705_Write_Reg(0x20);			 //设置下一步写时钟寄存器，并且选择第一对差分通道
 AD7705_Write_Reg(0x08); //输出更新频率20HZ，滤波器截止频率为5.24HZ
                         //进行二分频而且工作频率大于2M
 AD7705_Write_Reg(0x10);		   //选定设置寄存器并且进行写操作
 AD7705_Write_Reg(0x7a);		   //自校准、增益为128、双极性缓冲模型
 AD7705_Write_Reg(0x21);
 AD7705_Write_Reg(0x08);			 
 AD7705_Write_Reg(0x11);		   //选定设置寄存器并且进行写操作
 AD7705_Write_Reg(0x7a);           //自校准、增益为128、双极性缓冲模型		   
}

//------------------------------------------------------------------
//函数名：AD7705_Read
//功能：从AD7705数据寄存器中读数据
//参数：无
//返回值：无
//变量：无
//备注：无
//-------------------------------------------------------------------
void AD7705_Read(unsigned char n)
{ 
 	
 AD7705_Write_Reg(0x38+n);	   	//选择AD7705中的数据寄存器
 while(AD7705_DRDY);		
 MSB_Data = AD7705_Read_Reg();			//读出AD7705中数据寄存器的高八位
 LSB_Data = AD7705_Read_Reg();			//读出AD7705中数据寄存器的低八位
 ndata= MSB_Data*256+ LSB_Data;
 if(ndata>32768)
  {
  ndata=ndata-32768;
  }
  else
  ndata=32768-ndata;

}




//--------------------------------------------------------------
//程序名：appear
//功能：液晶显示采集的数据
//参数：无
//变量：无
//返回值：无
//备注：无
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
/*                   主函数                                       */
/******************************************************************/  
main()
{
 
LCD_init(); 
 
EA = 1;           //允许CPU中断
ET0 = 1;          //定时器0中断打开
TMOD = 0x51;      //设定时器0,1为模式1，16位模式
TH1=0x00; TL1=0x00;
TH0=0x3c; TL0=0xb0;     //设定时值为50ms
TR0 = 1;
          //开始定时

 delay_ms(100);
while(1)
{  
		R_S=0;
		delay_us(10); 
		R_S=1;
		delay_us(100);
		AD7705_Start(0);
		AD7705_Read(0);						//读AD7705数据寄存器中数据
		appear(0);							
		delay_ms(10); 
    AD7705_Start(1); 
    AD7705_Read(1);						//读AD7705数据寄存器中数据
		appear(1);							
		delay_ms(1); 
 }
}

/*******/

void TM_Time0(void) interrupt 1 //定时器0中断
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
