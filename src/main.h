/* LaTetris, by Owen Salvage

A basic implementation of Tetris in which you simply gain score by completing full horizontal lines
across the screen from falling blocks.

Licence: This work is licensed under the Creative Commons Attribution License.
         View this license at http://creativecommons.org/about/licenses/
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>
#include<time.h> 
#include "lcd/lcd.h"
#include "rios/rios.h"
#include "ruota/ruota.h"
#include "lcd/svgrgb565.h"

#define LED_INIT    DDRB  |=  _BV(PINB7)
#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7)
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

#define GRID_WIDTH 10
#define GRID_HEIGHT 24
#define BLOCk_SIZE 10
#define SCREEN_DISPLACEMENT 55

typedef struct {
    int x, y;
} point;

typedef struct {
    point p1, p2, p3, p4, start;
} block;

void init(void);
uint16_t rand_init();
int blink(int);
int update_dial(int);
int collect_delta(int);
int check_switches(int);
int step(int);
bool canMove(point);
void drawGrid();
void drawWholeBlock(block, uint16_t);
void drawBlock(point, uint16_t);
block getLiteralPosVals(block);
void removeWholeBlock(block);
void removeBlock(point);
void movePieceDown();
bool blockAboveZero(block);
void movePieceLeft();
void movePieceRight();
bool canPieceMove(block);
block getRandomBlock(point);
void placeBlock(block);
void checkRowsFull(block);
void checkRowFull(int);
block rotateBlock(block);
point setPoint(int, int);
void displayScore();
void displayNexBlock();

int rot_position = 0; // of the rotary encoder
bool grid[GRID_WIDTH][GRID_HEIGHT];
block currentBlock;
block nextBlock;
point lastBlock;
int moveCheck = 0;
int score = 0;