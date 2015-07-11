/*************************************
 *
 * SSD1306 configuration file
 *
 * Definitions and such for the lcd display controller
 *
 *************************************/

// "Fundamental Commands"
#define SSD1306_SET_CONTRAST 		0x81 // 256 possible values
#define SSD1306_DISPLAY_ON 		0xA4 // displays what is in the display's ram
#define SSD1306_DISPLAY_ALL_ON		0xA5 // turns on display and all pixels
#define SSD1306_NORMAL_DISPLAY		0xA6 // a.k.a not inverse
#define SSD1306_INVERSE_DISPLAY 	0xA7 // a.k.a. not normal
#define SSD1306_DISPLAY_SLEEP		0xAE
#define SSD1306_DISPLAY_WAKE		0xAF

// Scrolling Commands
#define SSD1306_HORZ_RIGHT_SCROLL 	0x26
#define SSD1306_HORZ_LEFT_SCROLL	0x27
/*
 * After starting the scroll send a 0x00, then the starting page address
 * 0x00 = page0, 0x01 = page1, 0x02 = page2, ... 0x07 = pag7
 * next the "time interval between each scroll step in terms of frame frequency"
 * Scroll  timing -  0=5 frames, 1=64, 2=128, 3=256, 4=3, 5=4, 6=25, 7=2
 * then the ending page that scrolls
 * followed by 0x00 then 0xff
 * See the datasheet or experemint and hope for the best
 ******/
#define SSD1306_VERT_RIGHT_SCROLL	0x29
#define SSD1306_VERT_LEFT_SCROLL	        0x2A
/*
 * Send an 0x00, page start, sroll timing, page end
 * Then vertical scrolling offset. 0x01 = 1 row, 0x3F = 63 rows
 * Indicates direction 1 = down, 63 = up
 *******/
#define SSSD1306_STOP_SCROLL		0x2E 
#define SSSD1306_START_SCROLL		0x2F

#define SSD1306_VERT_SCROLL		0xA3
/*
 * then send number of rows on the top that will NOT scroll
 * then send number of rows to actually scroll
 * so if you want the top to scroll you'd set 0 for the top rows and some number for the
 * other rows
 ******/

// Addressing Section
#define SSD1306_SET_ADDRESSING  	0x20
/*
 * Then send 0x01 for Horizontal Addressing, 0x02 for VerticalAddressing,
 * 0x03 for Page Addressing - Horizantal means you go left to right like reading
 * a book.  Vertical means you start in column 1 and go through the pages until
 * you get to the next column.  Page addressing means you don't move up and down 
 * just across the page
 ******/

#define SSD1306_SET_COLUMN              0x21
/*
 * Then send column start and column end. Doesn't work for paged mode.
 ******/

#define SSD1306_SET_PAGE                0x22
/*
 * Then send start page and end page you are accessing. Doesn't work for
 * paged mode (obviously).
 *******/

#define SSD1306_PAGE_0                  0xB0
#define SSD1306_PAGE_1                  0xB1
#define SSD1306_PAGE_2                  0xB2
#define SSD1306_PAGE_3                  0xB3
#define SSD1306_PAGE_4                  0xB4
#define SSD1306_PAGE_5                  0xB5
#define SSD1306_PAGE_6                  0xB6
#define SSD1306_PAGE_7                  0xB7
/*
 * When you are in page addressing mode this sets which page you are on
 *******/

// HARDWARE SETUP
/* You can pick which piece of memory represents the first page section
 * of the screen.  You send a value of 0x40 for the first block down to 
 * 0x7f.
 *******/

#define SSD1306_SEGMENT_REMAP_0         0xA0
#define SSD1306_SEGMENT_REMAP_1         0xA1
/*
 * Beats me.  One will work the other won't I guess.
 ******/

#define SSD1306_SET_MULTIPLEX_RATIO     0xA8
/*
 * Then send multiplex ratio. Beats me. Good luck
 ******/

#define SSD1306_SET_DISPLAY_OFFSET      0xD3
/*
 * Same as the last one?
 *****/

#define SSD1306_SET_COM_PINS            0xDA
/*
 * Another hardware related one that I don't know about
 ******/

// Timing Section
#define SSD1306_SET_DISPLAY_CLOCK_OSCILLATOR    0xD5
#define SSD1306_SET_PRE_CHARGE_PERIOD           0xD9
#define SSD1306_SET_VCOMH_DESELECT_LEVEL        0xDB
/*
 * See the datasheet
 *******/

#define SSD1306_NOOP                    0xE3









