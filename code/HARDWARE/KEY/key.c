#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ECA 2014
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
								    
// Initialization function keys
void KEY_Init(void) //IO initialization
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	// Initialize KEYS
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//enable PORTA, PORTE clock

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PE3~5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //set pullup input
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//initialize GPIOE 3,4,5,6

}
u8 KEY_Scan(u8 mode)
{	 
		u8 key_up=0;//button press release flag

		if(KEY1==0)				key_up|=0x01;
		else if(KEY2==0) 	key_up|=0x02;
		else if(KEY3==0) 	key_up|=0x04;
		else if(KEY4==0) 	key_up|=0x08;
	
		return key_up;
}
