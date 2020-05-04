#ifndef __HW3_H__
#define __HW3_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "timers.h"
#include "images.h"

extern volatile uint16_t PACMAN_X_COORD;
extern volatile uint16_t PACMAN_Y_COORD;
extern volatile uint16_t GHOST_X_COORD;
extern volatile uint16_t GHOST_Y_COORD;
extern volatile bool ALERT_PACMAN;
extern volatile bool ALERT_GHOST;

typedef enum{
  PS2_DIR_UP,
  PS2_DIR_DOWN,
  PS2_DIR_LEFT,
  PS2_DIR_RIGHT,
  PS2_DIR_CENTER,
  PS2_DIR_INIT,
} PS2_DIR_t;

extern volatile PS2_DIR_t dir;

bool hit_wall(
    volatile PS2_DIR_t direction,
    volatile uint16_t x_coord, 
    volatile uint16_t y_coord, 
    uint8_t image_height, 
    uint8_t image_width
);
		
//int hit_wall_helper(volatile int X_SPEED,
//		volatile int Y_SPEED, //direction of object
//    volatile uint16_t x_coord, //x coord of object
//    volatile uint16_t y_coord, 
//    uint16_t image_height, 
//    uint16_t image_width, int count
//	);




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
);

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
);

//*****************************************************************************
// Determines is any portion of the two images are overlapping.  An image is
// considered to be overlapping if the two rectangles determined by the image
// height and widths are overlapping.  An overlap occurs even if the area that
// overlaps are portions of the images where the pixels do not display on the
// screen.
//*****************************************************************************
bool check_collision(
        volatile uint16_t ship_x_coord, 
        volatile uint16_t ship_y_coord, 
        uint8_t ship_height, 
        uint8_t ship_width,
        volatile uint16_t invader_x_coord, 
        volatile uint16_t invader_y_coord, 
        uint8_t invader_height, 
        uint8_t invader_width
);

void lcd_draw_coins();		
//void lcd_draw_map();

        
        //*****************************************************************************
// Main application for HW3
//*****************************************************************************
int hw3_main(void);

#endif    