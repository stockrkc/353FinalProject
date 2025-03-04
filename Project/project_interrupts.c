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

#define NUM_LEDS 0x8


static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;
static volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;
static volatile uint16_t move_count = 25;
static volatile PS2_DIR_t ghost_dir = PS2_DIR_UP;
volatile bool AlertTimer;
volatile int blinkLED;


//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void)
{
	//will check the X values first since that holds precedence
	//checks if the joystick is returning a value greater than 2.4 (high threshold)
	if(PS2_X_DATA > 0x0C00) {
		return PS2_DIR_LEFT;
	}
	//checks if the joystick is returning a value less than .85 (low threshold)
	else if(PS2_X_DATA < 0x0400) {
		return PS2_DIR_RIGHT;
	}
	//then will check the y values after x is determined to be nothing
	//checks if the joystick is returning a value greater than 2.4 (high threshold)
	else if(PS2_Y_DATA > 0x0C00) {
		return PS2_DIR_UP;
	}
	//checks if the joystick is returning a value less than .85 (low threshold)
	else if(PS2_Y_DATA < 0x0400) {
		return PS2_DIR_DOWN;
	}
	//if in between the high and low threshold the joystick must not be moved and therefore
	//in the center
	else {
		return PS2_DIR_CENTER;
	}
}
  

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the Invader
//*****************************************************************************
void TIMER2A_Handler(void)
{		
	static PS2_DIR_t lastdir = PS2_DIR_RIGHT;
	
	
		if(PS2_DIR != PS2_DIR_CENTER) {
		//checks if contacted edge
		bool contact = contact_edge(PS2_DIR, PACMAN_X_COORD, PACMAN_Y_COORD,  pacmanupHeightPixels,  pacmanupWidthPixels);
		//bool wall = hit_wall(PS2_DIR, PACMAN_X_COORD, PACMAN_Y_COORD,  pacmanupHeightPixels,  pacmanupWidthPixels);
			
		lastdir = PS2_DIR;
		dir = lastdir;
		if(!contact){
			move_image(PS2_DIR, &PACMAN_X_COORD, &PACMAN_Y_COORD, pacmanupHeightPixels,  pacmanupWidthPixels); 
		}
		
	} else {
		bool contact = contact_edge(lastdir, PACMAN_X_COORD, PACMAN_Y_COORD,  pacmanupHeightPixels,  pacmanupWidthPixels);
	  //bool wall = hit_wall(lastdir, PACMAN_X_COORD, PACMAN_Y_COORD,  pacmanupHeightPixels,  pacmanupWidthPixels);

		if(!contact){
			move_image(lastdir, &PACMAN_X_COORD, &PACMAN_Y_COORD, pacmanupHeightPixels,  pacmanupWidthPixels);
		}
		
		
	}
	
	
	ALERT_PACMAN = true;

    // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER3 ISR is used to determine when to move the spaceship
//*****************************************************************************
void TIMER3A_Handler(void)

{
			//if the move count is greater than 0 the ship has pixels left to move
		if(move_count > 0){
			//decrement the move count so the next time it is called the ship will move one more
			move_count = move_count - 1;
			//since the move count is not 0, checks to make sure the ship isn't on the edge
			if (contact_edge(ghost_dir, GHOST_X_COORD, GHOST_Y_COORD, ghostpcHeightPixels, ghostpcWidthPixels)) {
				//if the ship is touching the edge, get new direction and move count
				ghost_dir = get_new_direction(ghost_dir);
				move_count = get_new_move_count();
			} else {
					move_image(ghost_dir, &GHOST_X_COORD, &GHOST_Y_COORD, ghostpcHeightPixels, ghostpcWidthPixels);
			}
		//if move count is 0, get new direction and move count
		}	else {
				ghost_dir = get_new_direction(ghost_dir);
				move_count = get_new_move_count();
		}
		
		ALERT_GHOST = true;
		
		// Clear the interrupt
		TIMER3->ICR |= TIMER_ICR_TATOCINT;

}


//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	ADC0 -> PSSI |= ADC_PSSI_SS2;
	
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	//gets the X and Y data from the analog to digital converter
  PS2_X_DATA = ADC0->SSFIFO2;
	PS2_Y_DATA = ADC0->SSFIFO2;
	
	//uses the x and y data to determine new direction
	PS2_DIR = ps2_get_direction();
	
  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

void TIMER1A_Handler(void)
{

			if(AlertTimer){
				if(blinkLED == 0) 
                    {
                        lp_io_set_pin(BLUE_BIT);
                        blinkLED = 0;
                    }
				else if(blinkLED == 1) {
          lp_io_clear_pin(BLUE_BIT);
          blinkLED = 1;
                    }
									}

    TIMER1 -> ICR = TIMER_ICR_TATOCINT;
}
				
