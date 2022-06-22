/*-----------------------------------------------
			STM32F103ZET6 Training Board V2

					www.ECA.ir
				 www.eShop.ECA.ir
				 www.Forum.ECA.ir
------------------------------------------------*/

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "math.h"
#include "lcd.h"
#include "loginpic.h"
#include <cstring>
#include "music.h"



#define icon_size 20 
#define onoff_size 20 
#define COLOR_INV	"OFF";

void piano(void);
void intro_page(void);
void main_page(void);
void make_sound(int keypressed);
void initTim2(void) ;
void TIM2_IRQHandler(void);
	uint16_t x = 0;
	uint16_t y[160];
  float zarib=1;
	int counterf=0;
	//int arr[40617]={0};
	int i=0;
	char k;
	int tmp;
	int stop=0;
	int pressed;
	
	int flag1=1;// must be char

	u8 key;
	//////
void TIM2_IRQHandler(void)
{ 
    if(TIM2->SR & TIM_SR_UIF)
    {
      		tp_dev.scan(0);//Touch scan
		if(tp_dev.sta&TP_PRES_DOWN)			
		{	
		 	if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
			{
				stop=1;
				if((tp_dev.x>100)&&(tp_dev.y>40)&&(tp_dev.y<64))
					pressed=12;
				else if((tp_dev.x>100)&&(tp_dev.y>92)&&(tp_dev.y<116))
					pressed=10;
				else if((tp_dev.x>100)&&(tp_dev.y>196)&&(tp_dev.y<220))
					pressed=7;
				else if((tp_dev.x>100)&&(tp_dev.y>248)&&(tp_dev.y<272))
					pressed=5;
				else if((tp_dev.x>100)&&(tp_dev.y>300)&&(tp_dev.y<324))
					pressed=3;
				else if((tp_dev.y>0)&&(tp_dev.y<52))
					pressed=13;
				else if((tp_dev.y>52)&&(tp_dev.y<104))
					pressed=11;
				else if((tp_dev.y>104)&&(tp_dev.y<156))
					pressed=9;
				else if((tp_dev.y>156)&&(tp_dev.y<208))
					pressed=8;
				else if((tp_dev.y>208)&&(tp_dev.y<260))
					pressed=6;
				else if((tp_dev.y>260)&&(tp_dev.y<312))
					pressed=4;
				else if((tp_dev.y>312)&&(tp_dev.y<364))
					pressed=2;
				else if((tp_dev.y>364))
					pressed=1;
			}
		}  
    }
		GPIOF->ODR^=(1<<6);
    TIM2->SR = 0;
}
////////////////////////////////
 int main(void)
 {
  
	delay_init();	
	NVIC_Configuration();
	//uart_init(9600);	 
 	//LED_Init();			 
	LCD_Init();
	LCD_Clear(White);
	KEY_Init(); 
 	POINT_COLOR=RED;

	LCD_ShowString(60,190,200,16,16, "Micro Lab Project");	
	LCD_ShowString(70,210,200,16,16, "electrical Piano");	

 	tp_dev.init();
	delay_ms(1000);
	initTim2();
	
	
	intro_page();
}
 


void intro_page(void)
{
	LCD_Clear(WHITE);
	LCD_DrawRectangle(80,320,160,360);
	LCD_Fill(80,320,160,360,BLUE);
	LCD_Show_Image(0,20,200,130,*loginpic);

	BACK_COLOR = White;
	POINT_COLOR = Red;
	
	LCD_ShowString(20,160,200,16,16, " Piano");
	LCD_ShowString(20,180,200,16,16, " Represented By : ");
	LCD_ShowString(20,200,200,16,16, " Reza Karimzade ");
	LCD_ShowString(20,220,200,16,16, " Armin Froozanfar ");
	LCD_ShowString(20,240,200,16,16, " many tanx to:");
	LCD_ShowString(20,260,200,16,16, " Negin Gholamipour ");


	BACK_COLOR = BLUE;
	POINT_COLOR = Yellow;
	LCD_ShowString(100,330,100,16,16,"Begin");
	while(1)
	{
	 	key=KEY_Scan(0);//physical key scan
		tp_dev.scan(0);//Touch scan
		if(tp_dev.sta&TP_PRES_DOWN)			
		{	
		 	if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
			{	
				if((tp_dev.x>80)&&(tp_dev.x<160)&&(tp_dev.y>320)&&(tp_dev.y<360))
					main_page();	
			}
		}
		if (key==KEY_2) main_page();	
		if(key==KEY_1)	intro_page();
		delay_ms(10);
	}
}

/*
///////////																		/////////////
										Main Page Programm
///////////															   	/////////////
*/

void main_page(void)
{
	
	RCC -> APB1ENR |= 0x1 << 29;
	RCC -> APB2ENR |= 0x207F << 2;
	DAC -> CR |= 0x10000;
	BACK_COLOR = White;
	POINT_COLOR = Black;
	LCD_Clear(WHITE);
	LCD_DrawLine(0,52,239,52);
	LCD_DrawLine(0,104,239,104);
	LCD_DrawLine(0,156,239,156);
	LCD_DrawLine(0,208,239,208);
	LCD_DrawLine(0,260,239,260);
	LCD_DrawLine(0,312,239,312);
	LCD_DrawLine(0,364,239,364);
	LCD_Fill(100,40,239,64,Black);
	LCD_Fill(100,92,239,116,Black);
	LCD_Fill(100,196,239,220,Black);
	LCD_Fill(100,248,239,272,Black);
	LCD_Fill(100,300,239,324,Black);
	
		while(1)
	{
		if(stop==1)
		{
			stop=0;
			make_sound(pressed);
		}
	}
	
}
///////////////////////////////////////////////////////////////

///////////////////////////////////
void make_sound(int keypressed)
{
	zarib =.1 ;
	i=0;
	while(i<40617 && stop==0)
	{
		DAC -> DHR8R2 = (uint8_t)(zarib*array[i]);  
		tmp=1;
		for(k=1;k<=keypressed ; k++ )
		{
		 tmp=1.06*tmp;
		}
		tmp=floor(22*tmp);
		delay_us(tmp);
		i++;
	}	
}
////////////////////

void initTim2(void)
{
    RCC->APB1ENR |= RCC_APB1Periph_TIM2;

    TIM2->CR1 =  TIM_CR1_URS;    //TIM_CR1_DIR |
    TIM2->CR2 = 0;
    TIM2->CNT = 0;
    TIM2->PSC = 2500; // Clock prescaler;

    TIM2->ARR = 7200; // Auto reload value
    TIM2->SR = 0; // Clean interrups & events flag

    TIM2->DIER = TIM_DIER_UIE; // Enable update interrupts

    // NVIC_SetPriority & NVIC_EnableIRQ defined in core_cm3.h 
    NVIC_SetPriority (TIM2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); // set Priority for Cortex-M0 System Interrupts 
    NVIC_EnableIRQ (TIM2_IRQn);

    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN; // Enable timer
} 
////////////////////////