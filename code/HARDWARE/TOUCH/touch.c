#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "24cxx.h"	    

#define ERR_RANGE 40 
#define SAVE_ADDR_BASE 50
//////////////////////////////////////////////////////////////////////////////////	 
//ECA 2014
//All rights reserved	
//www.eca.ir
//////////////////////////////////////////////////////////////////////////////////
_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0,
 	0,
	0,
	0,
	0,
	0,
	0,	  	 		
	0,
	1,	  	 		
};					
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 	 
		TCLK=1;		   
	}		 			    
} 		 
    
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		 	 
	TDIN=0; 	 
	TCS=0; 		 
	TP_Write_Byte(CMD); 
	delay_us(6); 
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		    	    
	TCLK=0; 	     	    
	for(count=0;count<16;count++) 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	  	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	 
	TCS=1;		  
	return(Num);   
}
 
#define READ_TIMES 5 	 
#define LOST_VAL 1	  	 
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0; 
	*x=xtemp;
	*y=ytemp;
	return 1;
}

u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);
	LCD_DrawLine(x,y-12,x,y+13);
	LCD_PutPixel(x+1,y+1, POINT_COLOR);
	LCD_PutPixel(x-1,y+1, POINT_COLOR);
	LCD_PutPixel(x+1,y-1, POINT_COLOR);
	LCD_PutPixel(x-1,y-1, POINT_COLOR);
	Draw_Circle(x,y,6);
}	  

void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_PutPixel(x,y, POINT_COLOR); 
	LCD_PutPixel(x+1,y, POINT_COLOR);
	LCD_PutPixel(x,y+1, POINT_COLOR);
	LCD_PutPixel(x+1,y+1, POINT_COLOR);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  

u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)
	{
		if(tp)TP_Read_XY2(&tp_dev.x,&tp_dev.y);
		else if(TP_Read_XY2(&tp_dev.x,&tp_dev.y))
		{
	 		tp_dev.x=tp_dev.xfac*tp_dev.x+tp_dev.xoff;
			tp_dev.y=tp_dev.yfac*tp_dev.y+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;  
			tp_dev.x0=tp_dev.x;
			tp_dev.y0=tp_dev.y;  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			tp_dev.sta&=~(1<<7);	
		}else
		{
			tp_dev.x0=0;
			tp_dev.y0=0;
			tp_dev.x=0xffff;
			tp_dev.y=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;
}	  
//////////////////////////////////////////////////////////////////////////	 
								    
void TP_Save_Adjdata(void)
{
	s32 temp,temp2;			 
	temp=tp_dev.xfac*100000000;//保存x校正因素      
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);   
	temp2=tp_dev.yfac*100000000;//保存y校正因素    
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp2,4);
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);		    
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);	
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);	
	temp=0X0A;
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
	
	printf("\nSave Data Xf=%i Yf=%i X=%i Y=%i", temp, temp2, tp_dev.xoff, tp_dev.yoff);
}

u8 TP_Get_Adjdata(void)
{					  
	s32 tempfac, tempfac2;
	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);
	if(tempfac==0X0A)			   
	{    												 
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);		   
		tp_dev.xfac=(float)tempfac/100000000;
		tempfac2=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);			          
		tp_dev.yfac=(float)tempfac2/100000000;
	    
		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);			   	  
 	    
		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);				 	  
 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);
		if(tp_dev.touchtype)//
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}	
		printf("\nGet Data Xf=%i Yf=%i X=%i Y=%i", tempfac, tempfac2, tp_dev.xoff, tp_dev.yoff);		
		return 1;	 
	}
	printf("\nNo EEPROM config for touch calibrate\n");
	
	return 0;
}	 

const u8* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  

void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowxNum(40+24,160,x0,4,16,0);		
	LCD_ShowxNum(40+24+80,160,y0,4,16,0);	
	LCD_ShowxNum(40+24,180,x1,4,16,0);		
	LCD_ShowxNum(40+24+80,180,y1,4,16,0);	
	LCD_ShowxNum(40+24,200,x2,4,16,0);		
	LCD_ShowxNum(40+24+80,200,y2,4,16,0);	
	LCD_ShowxNum(40+24,220,x3,4,16,0);		
	LCD_ShowxNum(40+24+80,220,y3,4,16,0);	
 	LCD_ShowxNum(40+56,lcddev.width,fac,3,16,0);

}
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_Clear(WHITE);
	POINT_COLOR=Black;
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);
	TP_Drow_Touch_Point(20,20,RED);
	tp_dev.sta=0;
	tp_dev.xfac=0;
	while(1)
	{
		tp_dev.scan(1);
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);
						   			   
			pos_temp[cnt][0]=tp_dev.x;
			pos_temp[cnt][1]=tp_dev.y;
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);		
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);
					break;
				case 4:	
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	
   	 					TP_Drow_Touch_Point(20,20,RED);		
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);
   	 					TP_Drow_Touch_Point(20,20,RED);								
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
						continue;
					} 
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2); 
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2); 
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	 
   	 					TP_Drow_Touch_Point(20,20,RED);								 
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]); 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2; 
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]); 
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	
   	 					TP_Drow_Touch_Point(20,20,RED);								
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;
						if(tp_dev.touchtype)
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");
					delay_ms(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);
					return;
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}		  
 
u8 TP_Init(void)
{			    		   
	 
	
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_13);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				  
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_SetBits(GPIOF,GPIO_Pin_9);

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		  
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	lcddev.width = 240;
	lcddev.height = 400;
	
  	TP_Read_XY(&tp_dev.x,&tp_dev.y);  
 	AT24CXX_Init(); 
	if(TP_Get_Adjdata())return 0; 
	else			   
	{ 										    
		LCD_Clear(WHITE);
	    TP_Adjust();   
		TP_Save_Adjdata();	 
	}			
	TP_Get_Adjdata();	
	return 1; 									 
}


