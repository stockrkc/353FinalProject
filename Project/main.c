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

volatile bool game = true;
uint8_t score;
uint8_t highscore;


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

//functions*******************************
//start screen functions
void startScreen(void){
	uint8_t touch_event;
  i2c_status_t td_status;
  i2c_status_t x_status;
  i2c_status_t y_status;
	uint16_t x,y;
		
	char msg[80];

	lcd_clear_screen(LCD_COLOR_BLACK);
	//draws the game start screen 
	lcd_draw_image(120, pclogoWidthPixels, 120, pclogoHeightPixels, pclogoBitmaps, LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
	
	while(1){
		//sees if the screen is being touched
			touch_event = ft6x06_read_td_status();
			
		//if the screen is touched, reads in the x and y values of the point that was touched
			if(touch_event > 0){
				x = ft6x06_read_x();
				y = ft6x06_read_y();
			//	printf("%d , %d \n\r",x, y);
				//checks to see if "start" was pressed and if it was, exits out of function and starts the game
				if((y >= 40 & y <= 130) & (x >= 40 & x <= 200)){
					game = true;
					return;
				}
		}
		    gp_timer_wait(TIMER0_BASE, 5000000);

		}
  
 };
//game over screen

int gameOverScreen(void){
	//	bool game_over = false;
	//	bool winner = false;
	uint8_t touch_event;
  i2c_status_t td_status;
  i2c_status_t x_status;
  i2c_status_t y_status;
	uint16_t x,y;
		
	char msg[80];
		
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	lcd_draw_image(120, gameoverWidthPixels, 80, gameoverHeightPixels, gameoverBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
	lcd_draw_image(120, loseWidthPixels, 240, loseHeightPixels,  loseBitmaps, LCD_COLOR_YELLOW, LCD_COLOR_BLACK);

	while(1){
			touch_event = ft6x06_read_td_status();
			if(touch_event > 0){
				x = ft6x06_read_x();
				y = ft6x06_read_y();
				//printf("%d , %d \n\r",x, y);
				if((y >= 60 & y <= 120) & (x >= 160 & x <= 239)){
					return 1;
				} else if((y >= 60 & y <= 120) & (x >= 0 & x <= 100)) {
					return 0;
				}
		}
		    gp_timer_wait(TIMER0_BASE, 5000000);

		}

	
}



///////////////////////////////////////////////
int 
	main(void)
{
		int newScreen;

		project_hardware_init();

    while(1){
			printf("Running...");
			startScreen();
			lcd_clear_screen(LCD_COLOR_BLACK);
			hw3_main();
			lcd_clear_screen(LCD_COLOR_BLACK);
			newScreen = gameOverScreen();
			while(newScreen == 1){
				hw3_main();
				newScreen = gameOverScreen();
			}
		}
 }
	