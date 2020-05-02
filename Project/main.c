// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"

//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{

  lcd_config_gpio();
	lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);	
	lcd_draw_image(
                          120,                       // X Center Point
                          map1WidthPixels,   // Image Horizontal Width
                          160,                       // Y Center Point
                          map1HeightPixels,  // Image Vertical Height
                          map1Bitmaps,       // Image
                          LCD_COLOR_BLUE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );	
		lcd_draw_image(
                          120,                       // X Center Point
                          pacmanWidthPixels,   // Image Horizontal Width
                          160,                       // Y Center Point
                          pacmanHeightPixels,  // Image Vertical Height
                          pacmanBitmaps,       // Image
                          LCD_COLOR_YELLOW,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );	
												
			lcd_draw_image(
                          80,                       // X Center Point
                          ghostpcWidthPixels,   // Image Horizontal Width
                          160,                       // Y Center Point
                          ghostpcHeightPixels,  // Image Vertical Height
                          ghostpcBitmaps,       // Image
                          LCD_COLOR_CYAN,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );											
		while(1) {};
	
		};