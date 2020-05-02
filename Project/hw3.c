#include "hw3.h"

volatile uint16_t PACMAN_X_COORD = 120;
volatile uint16_t PACMAN_Y_COORD = 160;
volatile uint16_t GHOST_X_COORD = 20; 
volatile uint16_t GHOST_Y_COORD = 40; 
volatile bool ALERT_PACMAN = true;
volatile bool ALERT_GHOST = true;
char STUDENT_NAME[] = "Allison McKinney, Jonathan Luong, Charles Stock";

typedef struct
{
    int top;
    int bottom;
    int left;
    int right;
} Rectangle;

//*****************************************************************************
// Determines if any part of the image would be off the screen if the image
// is moved in the specified direction.
//*****************************************************************************
bool contact_edge(
    volatile PS2_DIR_t direction,
    volatile uint16_t x_coord, 
    volatile uint16_t y_coord, 
    uint8_t image_height, 
    uint8_t image_width
)
{
	switch(direction) 
		{
			case PS2_DIR_LEFT:
			{
				if(x_coord - (.5 * image_width) <= 0) { //checks to see if left part of picture touches left screen boundary
					return true;
				}
				break;
			}
		  case PS2_DIR_RIGHT:
			{
				if(x_coord + (.5 * image_width) > 239) { //checks to see if right part of picture touches right screen boundary
					return true;
				}
				break;
			}
			case PS2_DIR_UP:
			{
				if(y_coord - (.5 * image_height) <= 0) { //checks to see if bottom part of picture touches bottom screen boundary
					return true;
				}
				break;
			}
			case PS2_DIR_DOWN:
			{
				if(y_coord + (.5 * image_height) > 319) { //checks to see if top part of picture touches top screen boundary
					return true;
				}
				break;
			} 
			case PS2_DIR_CENTER: //image is in the direction
			{
				return false;
				break;
			}
			case PS2_DIR_INIT: //image just initialize
			{ 
				return false;
				break;
			}
			
			return false;
		
		
		}
}

//*****************************************************************************
// Moves the image by one pixel in the provided direction.  The second and 
// third parameter should modify the current location of the image (pass by
// reference)
//*****************************************************************************
void move_image(
        volatile PS2_DIR_t direction,
        volatile uint16_t *x_coord, 
        volatile uint16_t *y_coord, 
        uint8_t image_height, 
        uint8_t image_width
)
{
		switch(direction) 
		{
			case PS2_DIR_LEFT:
			{ //in the case of PS2 in left position move image left by one pixel
				*x_coord = *x_coord - 1;
				break;
			}
		  case PS2_DIR_RIGHT:
			{//in the case of PS2 in right position move image right by one pixel
				*x_coord = *x_coord + 1;
				break;
			}
			case PS2_DIR_UP:
			{//in the case of PS2 in down position move image down by one pixel
				*y_coord = *y_coord - 1;
				break;
			}
			case PS2_DIR_DOWN:
			{//in the case of PS2 in up position move image up by one pixel
				*y_coord = *y_coord + 1;
				break;
			}
			case PS2_DIR_CENTER:
			{//in the case of PS2 is in the center, do not move
				break;
			}
			case PS2_DIR_INIT:
			{ //in the case of starting up do not move
				break;
			}
			
		}
   
}

//*****************************************************************************
// Determines is any portion of the two images are overlapping.  An image is
// considered to be overlapping if the two rectangles determined by the image
// height and widths are overlapping.  An overlap occurs even if the area that
// overlaps are portions of the images where the pixels do not display on the
// screen.
//*****************************************************************************
bool check_game_over(
        volatile uint16_t ghost_x_coord, 
        volatile uint16_t ghost_y_coord, 
        uint8_t ghost_height, 
        uint8_t ghost_width,
        volatile uint16_t pacman_x_coord, 
        volatile uint16_t pacman_y_coord, 
        uint8_t pacman_height, 
        uint8_t pacman_width
)
{ bool x_intersect; // boolean value to determine if an axis in the x direction intersect eachother
	bool y_intersect; //boolean value to determine if an axis in the y direction intersect eachother
	
	// equation test if at any point the x axis on the right side of the ship is within the x range of the picture, then checks for the left side 
	if(((ghost_x_coord+ghost_width/2)<=(pacman_x_coord+pacman_width/2)&&(ghost_x_coord+ghost_width/2)>=(pacman_x_coord-pacman_width/2))||((ghost_x_coord-ghost_width/2)>=(pacman_x_coord-pacman_width/2)&&(ghost_x_coord-ghost_width/2)<=(pacman_x_coord+pacman_width/2))){
		x_intersect =true;	
	}
	
	// equation test if at any point the y axis on the top side of the ship is within the y range of the picture, then checks for the bottom side 
	if(((ghost_y_coord+ghost_height/2)<=(pacman_y_coord+pacman_height/2)&&(ghost_y_coord+ghost_height/2)>=(pacman_y_coord-pacman_height/2))||((ghost_y_coord-ghost_height/2)>=(pacman_y_coord-pacman_height/2)&&(ghost_y_coord-ghost_height/2)<=(pacman_y_coord+pacman_height/2))){
		y_intersect =true;	
	}
	
	//both conidition must be met for intersection to be true for the two objects 
	if((x_intersect==true)&&(y_intersect==true)){
		return true;
	}
	
	return false; // return false if not met
}

//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  
  // Update the Space Shipt 60 times per second.
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}

//*****************************************************************************
// Main application for HW3
//*****************************************************************************
void hw3_main(void)
{ 
	
//	lcd_draw_image(
//													120,                       // X Center Point
//                          map1WidthPixels,   // Image Horizontal Width
//                          160,                       // Y Center Point
//                          map1HeightPixels,  // Image Vertical Height
//                          map1Bitmaps,       // Image
//                          LCD_COLOR_BLUE,           // Foreground Color
//                          LCD_COLOR_BLACK          // Background Color
//                        );	

		
         while(1)
				 {
          if(ALERT_GHOST)
          {
            ALERT_GHOST = false;
            
            lcd_draw_image(
                          GHOST_X_COORD,                       // X Center Point
                          ghostpcWidthPixels,   // Image Horizontal Width
                          GHOST_Y_COORD,                       // Y Center Point
                          ghostpcHeightPixels,  // Image Vertical Height
                          ghostpcBitmaps,       // Image
                          LCD_COLOR_CYAN,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
//              
          
//          if(ALERT_PACMAN)
//          {
//            ALERT_PACMAN = false;
//            
//            lcd_draw_image(
//                          PACMAN_X_COORD,          // X Center Point
//                          pacmanWidthPixels,       // Image Horizontal Width
//                          PACMAN_Y_COORD,          // Y Center Point
//                          pacmanHeightPixels,      // Image Vertical Height
//                          pacmanBitmaps,           // Image
//                          LCD_COLOR_YELLOW,            // Foreground Color
//                          LCD_COLOR_BLACK           // Background Color
//                        );
//         
//          }
          
				}
			}
			}					