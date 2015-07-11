/*************************************
 *
 * SSD1306 configuration file
 *
 * Definitions and such for the lcd display controller
 *
 *************************************/

// "Fundamental Commands"
#define SET_CONTRAST 		0x81 // 256 possible values
#define DISPLAY_ON 		0xA4 // displays what is in the display's ram
#define DISPLAY_ALL_ON		0xA5 // turns on display and all pixels
#define NORMAL_DISPLAY		0xA6 // a.k.a not inverse
#define INVERSE_DISPLAY 	0xA7 // a.k.a. not normal
#define DISPLAY_SLEEP		0xAE
#define DISPLAY_WAKE		0xAF

// Scrolling Commands
#define HORZ_RIGHT_SCROLL 	0x26
#define HORZ_LEFT_SCROLL	0x27
/*
 * After starting the scroll send a 0x00, then the starting page address
 * 0x00 = page0, 0x01 = page1, 0x02 = page2, ... 0x07 = pag7
 * next the "time interval between each scroll step in terms of frame frequency"
 * Scroll  timing -  0=5 frames, 1=64, 2=128, 3=256, 4=3, 5=4, 6=25, 7=2
 * then the ending page that scrolls
 * followed by 0x00 then 0xff
 * See the datasheet or experemint and hope for the best
 ******/
#define VERT_RIGHT_SCROLL	0x29
#define VERT_LEFT_SCROLL	0x2A
/*
 * Send an 0x00, page start, sroll timing, page end
 * Then vertical scrolling offset. 0x01 = 1 row, 0x3F = 63 rows
 * Indicates direction 1 = down, 63 = up
 *******/
#define STOP_SCROLL		0x2E 
#define START_SCROLL		0x2F

#define VERT_SCROLL		0xA3
/*
 * then send number of rows on the top that will NOT scroll
 * then send number of rows to actually scroll
 * so if you want the top to scroll you'd set 0 for the top rows and some number for the
 * other rows
 ******/

#define HORIZONTAL_ADDRESSING	0x20



