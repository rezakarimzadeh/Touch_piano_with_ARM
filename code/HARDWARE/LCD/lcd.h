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


#ifndef __LCD_H
#define __LCD_H	

#include "sys.h"	 
#include "stdlib.h"

#define ORIENTATION_VERTICAL

typedef struct  
{										    
	u16 width;			
	u16 height;			
	u16 id;				
	u8  dir;			
	u8	wramcmd;		
	u8  setxcmd;		
	u8  setycmd;		
}_lcd_dev; 	  


extern _lcd_dev lcddev;	

extern u16  POINT_COLOR;
extern u16  BACK_COLOR; 

#ifdef ORIENTATION_VERTICAL

	#define WIDTH       	240 
	#define HEIGHT      	400

#else

	#define WIDTH       	400
	#define HEIGHT      	240

#endif

typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;
#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
 
	    															  
void LCD_Init           (void);
void LCD_Window			(u16 Xstart, u16 Ystart, u16 Xend, u16 Yend);
void LCD_WindowMax      (void);
void LCD_SetCursor		(u16 Xpos, u16 Ypos);
void LCD_PutPixel 		(u16 x, u16 y, u16 color); 
void LCD_Clear          (u16 color);
void LCD_Fill			(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_SetTextColor   (u16 color);
void LCD_SetBackColor   (u16 color);
void LCD_Show_Image		(int x0, int y0, int w, int h, const u16 *code);			
void LCD_ShowChar		(u16 x, u16 y, u8 num, u8 size, u8 mode);
void LCD_ShowString		(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowxNum		(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
void LCD_DrawLine		(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle	(u16 x1, u16 y1, u16 x2, u16 y2);
void Draw_Circle		(u16 x0,u16 y0,u8 r);

/* LCD RGB color definitions */
#define Black           0x0000		/*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */
#define WHITE           0XFFFF      /* 255, 255, 255 */   
#define RED             0xF800      /* 255,   0,   0 */
#define BLUE            0x001F      /*   0,   0, 255 */
							  		 
#endif  

