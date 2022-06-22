/*
******************************************************************************
* HX8352B Driver Core
* Branch Version : 1.2.0
* Ramin Jafarpour @ ECA.ir Co.			06/08/15
* raminmicro@gmail.com
* www.ECA.ir
* www.Eshop.ECA.ir
* www.Forum.ECA.ir
******************************************************************************
*/

#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	   
 
u16 POINT_COLOR=0x0000;
u16 BACK_COLOR=0xFFFF; 

static volatile unsigned short TextColor = Black, BackColor = White;
uint16_t DeviceCode;

_lcd_dev lcddev;

__asm void wait()
{
    nop
    BX lr
}

void wait_delay(int count)
{
  while(count--);
}

void delayms(int dly)
{
	int i;
	while(dly--)
	{
		for(i=0;i<10000;i++)
		wait();
	}
}
		   
void LCD_WR_REG(u16 regval)
{ 
	LCD->LCD_REG=regval;
}

void LCD_WR_DATA(u16 data)
{										    	   
	LCD->LCD_RAM=data;		 
}

u16 LCD_RD_DATA(void)
{										    	   
	return LCD->LCD_RAM;		 
}					   

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		
	LCD->LCD_RAM = LCD_RegValue;   		 
}	

void wr_reg (unsigned char reg, unsigned short val) 
{
	LCD->LCD_REG = reg;		
	LCD->LCD_RAM = val;  	
}

u16 LCD_ReadReg(u8 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		
	delay_us(5);		  
	return LCD_RD_DATA();		
}   

void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 

void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;
}

void opt_delay(u8 i)
{
	while(i--);
}

void LCD_Init(void)
{ 										  
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);//使能PORTB,D,E,G以及AFIO复用功能时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PB0 推挽输出 背光
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	//PORTE复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 

	//	//PORTG12复用推挽输出 A0	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	 //	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure); 

	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
	readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 


	writeTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
	writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
	writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK	
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 


	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
	 
	delay_ms(50); // delay 50 ms 
	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
	lcddev.id = LCD_ReadReg(0x0000);   
	//printf(" LCD ID1:%x\r\n",lcddev.id); //打印LCD ID 

	wr_reg(0x001A, 0x0004); // BT[3:0]=0100, VCL=-VCI; VGH=VCI+2DDVDH; VGL=-2DDVDH
	wr_reg(0x001B, 0x000C); // VRH[4:0]=0Ch, VREG1=(2.5v*1.9)=4.75V		VCIRE=1; 
	// VCOM offset
	wr_reg(0x0023, 0x0000); // SELVCM=0, R24h and R25h selects VCOM
	wr_reg(0x0024, 0x0040); // VCM[6:0]=1000000, VCOMH voltage=VREG1*0.748 (originally 5F)
	wr_reg(0x0025, 0x000F); // VDV[4:0]=01111, VCOMH amplitude=VREG*1.00
	wr_reg(0x002D, 0x0006); // NOW[2:0]=110, Gate output non-overlap period = 6 clocks
	delayms(20);
	// Power on Setting
	wr_reg(0x0018, 0x0008); // RADJ[3:0]=1000, Display frame rate 60Hz 100%
	wr_reg(0x0019, 0x0001); // OSC_EN=1, start OSC
	delayms(20);
	wr_reg(0x0001, 0x0000); // DSTB=0, out deep sleep
	wr_reg(0x001F, 0x0088); // STB=0
	wr_reg(0x001C, 0x0006); // AP[2:0]=110, High OPAMP current (default 011)
	delayms(10);
	wr_reg(0x001F, 0x0080); // DK=0
	delayms(10);
	wr_reg(0x001F, 0x0090); // PON=1
	delayms(5);
	wr_reg(0x001F, 0x00D0); // VCOMG=1
	delayms(10);
	wr_reg(0x0017, 0x0005); // IFPF[2:0]=101, 16-bit/pixel
	
	// Panel Configuration
	//wr_reg(0x0036, 0x0011); // REV_PANEL=1, SM_PANEL=1, GS_PANEL=1, SS_PANEL=1
	//wr_reg(0x0029, 0x0031); // NL[5:0]=110001, 400 lines
	//wr_reg(0x0071, 0x001A); // RTN0


	//Gamma 2.2 Setting
	
	wr_reg(0x0040, 0x0000);	
	wr_reg(0x0041, 0x0009);
	wr_reg(0x0042, 0x0012);
	wr_reg(0x0043, 0x0004);
	wr_reg(0x0044, 0x0000);
	wr_reg(0x0045, 0x0023);//
	wr_reg(0x0046, 0x0003);
	wr_reg(0x0047, 0x005E);//
	wr_reg(0x0048, 0x0000);
	wr_reg(0x0049, 0x0000);
	wr_reg(0x004A, 0x0000);
	wr_reg(0x004B, 0x0000);
	wr_reg(0x004C, 0x0000);
	wr_reg(0x004D, 0x0000);
	wr_reg(0x004E, 0x0000);
	wr_reg(0x0057, 0x004F);//

#ifdef ORIENTATION_VERTICAL
	wr_reg(0x0016, 0x00C8);	
#else
	wr_reg(0x0016, 0x00A8);//A8
#endif
	wr_reg(0x0028, 0x0038); //GON=1; DTE=1; D[1:0]=10
	delayms(40);
	wr_reg(0x0028, 0x003C); //GON=1; DTE=1; D[1:0]=11

	delayms(10);
	
}  

/*
*********************************************************************************************************
* Description: 	Setting the coordinate of cursor
* Arguments  : 	(Xpos , Ypos) the coordinate
* Returns    : 	None
*********************************************************************************************************
*/

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	 wr_reg(0x0080, Xpos>>8);
	 wr_reg(0x0081, Xpos&0xFF);
	 wr_reg(0x0082, Ypos>>8);
	 wr_reg(0x0083, Ypos&0xFF); 
}

void LCD_Window(u16 Xstart, u16 Ystart, u16 Xend, u16 Yend)
{
	 wr_reg(0x0002, Xstart>>8);
	 wr_reg(0x0003, Xstart&0xFF);
	 wr_reg(0x0004, Xend>>8);
	 wr_reg(0x0005, Xend&0xFF);
	
	 wr_reg(0x0006, Ystart>>8);
	 wr_reg(0x0007, Ystart&0xFF);
	 wr_reg(0x0008, Yend>>8);
	 wr_reg(0x0009, Yend&0xFF);
	
#ifdef ORIENTATION_VERTICAL
	LCD_SetCursor(Xstart , Ystart);	
#else
	LCD_SetCursor(Ystart , Xstart);
#endif
}

/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_WindowMax (void) 
{
	LCD_Window(0, 0, WIDTH-1, HEIGHT-1);
}

void LCD_PutPixel (u16 x, u16 y, u16 color) 
{
#ifdef ORIENTATION_VERTICAL
	LCD_SetCursor(x, y);
#else
	LCD_SetCursor(y, x);
#endif	
	LCD_WR_REG(0x22);  					
	LCD_WR_DATA(color);
	//LCD_WindowMax();
}

/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Clear (unsigned short color) 
{
	unsigned int   x,y;

	LCD_WindowMax();
		  
	LCD_WR_REG(0x22);
	
	for(y = 0; y <HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			LCD_WR_DATA(color);
		}	
	}
}
/********************************************************************************************************
* Description: 	In designated areas within the specified color display 
* Arguments  : 	(xsta ,ysta) start coordinate
*				(xend ,yend) end   coordinate
*				color        I don't know 
* Returns    : 	None
********************************************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{                    
  u32 x,y;
											
	LCD_Window(xsta, ysta, xend, yend);			
	LCD_WR_REG(0x22);  					
	
	for (y=0; y<yend-ysta+1; y++)
	{
		for (x=0; x<xend-xsta+1; x++)
		{
			LCD_WR_DATA(color);
		}
	}	
	LCD_WindowMax();	
} 

/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetTextColor (unsigned short color) 
{
  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetBackColor (unsigned short color) 
{
  BackColor = color;
}

u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

void LCD_Show_Image(int x0, int y0, int w, int h, const u16 *code)
{
	u32 xcounter, ycounter;

	LCD_Window(x0 ,y0, x0+w-1, y0+h-1);
	LCD_WR_REG(0x22);
	
	for(ycounter=0;ycounter<h;ycounter++)
	{
		for(xcounter=0; xcounter<w; xcounter++)
		{
			LCD_WR_DATA(*code++);
		}
	}
	LCD_WindowMax();
}

void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	   
	num=num-' ';
	if(!mode)
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //1206
			else temp=asc2_1608[num][t];		 //1608	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80) 
					LCD_PutPixel(x, y, colortemp);
				else 
					LCD_PutPixel(x, y, BACK_COLOR);
				temp<<=1;
				y++;
				if(x>=WIDTH){POINT_COLOR=colortemp;return;}
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=WIDTH){POINT_COLOR=colortemp;return;}
					break;
				}
			}  	 
	    }    
	}else
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //1206
			else temp=asc2_1608[num][t];		 //1608	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80) LCD_PutPixel(x,y, POINT_COLOR); 
				temp<<=1;
				y++;
				if(x>=HEIGHT){POINT_COLOR=colortemp;return;}
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=WIDTH){POINT_COLOR=colortemp;return;}
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
} 

void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t, y, '0', size, mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y, ' ', size, mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1;
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x;
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{  
		LCD_PutPixel(uRow, uCol, POINT_COLOR);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);
	while(a<=b)
	{
		LCD_PutPixel(x0+a, y0-b, POINT_COLOR);             //5
 		LCD_PutPixel(x0+b, y0-a, POINT_COLOR);             //0           
		LCD_PutPixel(x0+b, y0+a, POINT_COLOR);             //4               
		LCD_PutPixel(x0+a, y0+b, POINT_COLOR);             //6 
		LCD_PutPixel(x0-a, y0+b, POINT_COLOR);             //1       
 		LCD_PutPixel(x0-b, y0+a, POINT_COLOR);             
		LCD_PutPixel(x0-a, y0-b, POINT_COLOR);             //2             
  		LCD_PutPixel(x0-b, y0-a, POINT_COLOR);             //7     	         
		a++;
		
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

//END




























