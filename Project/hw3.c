#include "hw3.h"
#include "main.h"

volatile uint16_t PACMAN_X_COORD = 10;
volatile uint16_t PACMAN_Y_COORD = 10;
volatile uint16_t GHOST_X_COORD = 120; 
volatile uint16_t GHOST_Y_COORD = 300;
volatile 	int livesRemaining;

volatile uint16_t coinx[9] = {20, 220, 220, 20, 20, 220, 120, 120, 120};
volatile uint16_t coiny[9] = {20, 300, 20, 300, 160, 160, 20, 300, 160};
static bool coinvis[9] = {true, true, true, true, true, true, true, true, true};


volatile PS2_DIR_t dir = PS2_DIR_RIGHT;
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
				if(x_coord + (.5 * image_width) > 239) {					//checks to see if right part of picture touches right screen boundary
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

void lcd_draw_coins(){
		
			if(coinvis[0]) {
				lcd_draw_image(20, coinWidthPixels, 20, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			} else{
				lcd_draw_rectangle_centered(20, 8, 20, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[1]) {
			lcd_draw_image(220, coinWidthPixels, 300, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}
				else{
					lcd_draw_rectangle_centered(220, 8, 300, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[2]) {
			lcd_draw_image(220, coinWidthPixels, 20, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			} else{
				lcd_draw_rectangle_centered(220, 8, 20, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[3]) {			
			lcd_draw_image(20, coinWidthPixels, 300, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(20, 8, 300, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[4]) {			
			lcd_draw_image(20, coinWidthPixels, 160, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(20, 8, 160, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[5]) {			
			lcd_draw_image(220, coinWidthPixels, 160, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(220, 8, 160, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[6]) {
			lcd_draw_image(120, coinWidthPixels, 20, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(120, 8, 20, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[7]) {
			lcd_draw_image(120, coinWidthPixels, 300, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(120, 8, 300, 8, LCD_COLOR_BLACK);
			}
			if(coinvis[8]) {
				
			lcd_draw_image(120, coinWidthPixels, 160, coinHeightPixels, coinBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			}else{
				lcd_draw_rectangle_centered(120, 8, 160, 8, LCD_COLOR_BLACK);
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
bool check_collision(
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
		lcd_draw_rectangle_centered(PACMAN_X_COORD, 19, PACMAN_Y_COORD, 19, LCD_COLOR_BLACK);
		PACMAN_X_COORD = 120;
    PACMAN_Y_COORD = 160;
		livesRemaining--; 
		return true;
	}
	
	return false; // return false if not met
}

bool coin_collision(
        volatile uint16_t coin_x_coord, 
        volatile uint16_t coin_y_coord, 
        uint8_t coin_height, 
        uint8_t coin_width,
        volatile uint16_t pacman_x_coord, 
        volatile uint16_t pacman_y_coord, 
        uint8_t pacman_height, 
        uint8_t pacman_width
)
{ bool x_intersect; // boolean value to determine if an axis in the x direction intersect eachother
	bool y_intersect; //boolean value to determine if an axis in the y direction intersect eachother

	
	// equation test if at any point the x axis on the right side of the ship is within the x range of the picture, then checks for the left side 
	if(((coin_x_coord+coin_width/2)<=(pacman_x_coord+pacman_width/2)&&(coin_x_coord+coin_width/2)>=(pacman_x_coord-pacman_width/2))||((coin_x_coord-coin_width/2)>=(pacman_x_coord-pacman_width/2)&&(coin_x_coord-coin_width/2)<=(pacman_x_coord+pacman_width/2))){
		x_intersect =true;	
	}
	
	// equation test if at any point the y axis on the top side of the ship is within the y range of the picture, then checks for the bottom side 
	if(((coin_y_coord+coin_height/2)<=(pacman_y_coord+pacman_height/2)&&(coin_y_coord+coin_height/2)>=(pacman_y_coord-pacman_height/2))||((coin_y_coord-coin_height/2)>=(pacman_y_coord-pacman_height/2)&&(coin_y_coord-coin_height/2)<=(pacman_y_coord+pacman_height/2))){
		y_intersect =true;	
	}
	
	//both conidition must be met for intersection to be true for the two objects 
	if((x_intersect==true)&&(y_intersect==true)){
		return true;
		
	}
	
	return false; // return false if not met
}

void pacman_coin(){
	
	
		int i=0;
	
		for (i=0;i<9;i++){
			if (coin_collision(coinx[i], coiny[i], coinHeightPixels, coinWidthPixels,PACMAN_X_COORD, PACMAN_Y_COORD, pacmanupHeightPixels, pacmanupWidthPixels)){
				coinvis[i] = false;
			}	
		}		
	
}


bool check_win(){
	
	
	
	
}



//*****************************************************************************
// Main application for HW3
//*****************************************************************************
int hw3_main(void)
{ 
	char str[20];
	bool check_win = false;
	int i;
	livesRemaining = 3;
	io_expander_write_reg(MCP23017_GPIOA_R, 0x07);
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	lcd_draw_coins();
         while(livesRemaining > 0)
				 {	
					 lcd_draw_coins();

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
							

						 check_collision(
                                            PACMAN_X_COORD,
                                            PACMAN_Y_COORD,
                                            pacmanupHeightPixels,
                                            pacmanupWidthPixels,
                                            GHOST_X_COORD,
                                            GHOST_Y_COORD,
                                            ghostpcHeightPixels,
                                            ghostpcWidthPixels
                                        );
																											
						

					if(livesRemaining == 2){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x03);
					}
					if(livesRemaining == 1){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x01);
					}
					if(livesRemaining == 0){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x00);
					}
				}
          
          if(ALERT_PACMAN)
          {
						
						uint8_t pacman[57];
						uint8_t i;
						
            ALERT_PACMAN = false;
            
						//determines what way pacman should be facing
						for (i=0;i<=57;i++){
							if(dir == PS2_DIR_LEFT) { 
								pacman[i]=pacmanleftBitmaps[i];
							}
							if(dir == PS2_DIR_UP) { 
								pacman[i]=pacmanupBitmaps[i];
							}
							if(dir == PS2_DIR_RIGHT) { 
								pacman[i]=pacmanrightBitmaps[i];
							}
							if(dir == PS2_DIR_DOWN) { 
								pacman[i]=pacmandownBitmaps[i];
							}
						}


						
            lcd_draw_image(
                          PACMAN_X_COORD,          // X Center Point
                          pacmanupWidthPixels,       // Image Horizontal Width
                          PACMAN_Y_COORD,          // Y Center Point
                          pacmanupHeightPixels,      // Image Vertical Height
                          pacman,           			// Image
                          LCD_COLOR_YELLOW,            // Foreground Color
                          LCD_COLOR_BLACK           // Background Color
                        );
						
											 check_collision(
                                            PACMAN_X_COORD,
                                            PACMAN_Y_COORD,
                                            pacmanupHeightPixels,
                                            pacmanupWidthPixels,
                                            GHOST_X_COORD,
                                            GHOST_Y_COORD,
                                            ghostpcHeightPixels,
                                            ghostpcWidthPixels
                                        );
																				
					
																				
					if(livesRemaining == 2){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x03);
					}
					if(livesRemaining == 1){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x01);
					}
					if(livesRemaining == 0){
						io_expander_write_reg(MCP23017_GPIOA_R, 0x00);
					}													
					
         
          }
         	
					pacman_coin();
					lcd_draw_coins(); 
				
				 }

		}		