/* See main.h for further detail */

#include "main.h"

void main(void) {
	init();
    
    os_add_task( step,            300, 1); //Steps every 0.3s (falling blocks)
    os_add_task( check_switches,  50, 1);
    
    point p;
    p.x = 4;
    p.y = 21;
    currentBlock = getRandomBlock(p);
    
    drawGrid();
    
    displayScore();
    display_string_xy("Next Block", SCREEN_DISPLACEMENT + 16 * BLOCk_SIZE, 240 - 22 * BLOCk_SIZE);
    p.x = 17;
    p.y = 16;
    nextBlock = getRandomBlock(p);
    drawWholeBlock(nextBlock, DEEP_SKY_BLUE);
    
    for(;;){}
    
}

/* rand_init() is taken from:
   https://github.com/Giodiro/AVR-Space_Invaders
*/
uint16_t rand_init() {
    //ADC conversion from unused pins should give random results.
    //an amplification factor of 200x is used.
    ADMUX |= _BV(REFS0) | _BV(MUX3) | _BV(MUX1) | _BV(MUX0);
    //Prescaler
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);
    //Enable and start
    ADCSRA |= _BV(ADEN) | _BV(ADSC);
    //Wait until complete
    while(! (ADCSRA & _BV(ADIF))) {
        _delay_ms(2);
    }
    //Read result
    uint16_t res = ADCL;
    res |= ADCH << 8;
    //Disable
    ADCSRA &= ~_BV(ADEN);
    
    //The XOR should increase the randomness?
    //since the converted number is only 10 bits
    return res ^ 0xACE1u;
}

// Draws the grid on the screen
void drawGrid(){
    rectangle r;
    r.left = SCREEN_DISPLACEMENT;
    r.right = SCREEN_DISPLACEMENT + 100;
    r.top = 0;
    r.bottom = 240;
	cli();
    clear_screen();
    fill_rectangle( r, BLACK);
    sei();
    
    for (int x = 0; x < 10; x++){
        for (int y = 0; y < 24; y++){
            point p;
            p.x = x;
            p.y = y; 
            removeBlock(p); // sets the blocks to white
        }
    }

    r.left = SCREEN_DISPLACEMENT + 16 * BLOCk_SIZE;
    r.right = SCREEN_DISPLACEMENT + 21 * BLOCk_SIZE;
    r.bottom = 240 - 15 * BLOCk_SIZE;
    r.top = 240 - 21 * BLOCk_SIZE;
    fill_rectangle( r, BLACK);
    
    for (int x = 16; x < 21; x++){
        for (int y = 15; y < 21; y++){
               point p;
                p.x = x;
                p.y = y; 
                removeBlock(p); // sets the blocks to white
        }
        
    }
}

block getLiteralPosVals(block b){
    b.p1.x += b.start.x;
    b.p2.x += b.start.x;
    b.p3.x += b.start.x;
    b.p4.x += b.start.x;
    b.p1.y += b.start.y;
    b.p2.y += b.start.y;
    b.p3.y += b.start.y;
    b.p4.y += b.start.y;
    return b;
}

void drawWholeBlock(block b, uint16_t col){
    b = getLiteralPosVals(b);
    drawBlock(b.p1, col);
    drawBlock(b.p2, col);
    drawBlock(b.p3, col);
    drawBlock(b.p4, col);
}

void drawBlock(point p, uint16_t col){
    rectangle r;
    r.left = SCREEN_DISPLACEMENT + p.x * BLOCk_SIZE + 2;
    r.right = SCREEN_DISPLACEMENT + p.x * BLOCk_SIZE + BLOCk_SIZE - 2;
    r.bottom = 240 - p.y * BLOCk_SIZE - 2;
    r.top = 240 - p.y * BLOCk_SIZE - BLOCk_SIZE + 2;
    cli();
    fill_rectangle( r, col);
    sei();
}

void removeWholeBlock(block b){
    drawWholeBlock(b, WHITE);
}

void removeBlock(point p){
    drawBlock(p, WHITE);
}

/* Configure I/O Ports */
void init(void) {

	/* 8MHz clock, no prescaling (DS, p. 48) */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
    LED_INIT;
    
    //Rotary Encoder
    DDRE &= ~_BV(PE4);      /* Sets RotaA to input*/
    DDRE &= ~_BV(PE5);      /* Sets RotaB to input*/
    PORTE |= _BV(PE4);      /* Activates pull-up  */
    PORTE |= _BV(PE5);      /* Activates pull-up  */
    
    init_lcd();
    os_init_scheduler();
    os_init_ruota();
    
    srand(rand_init());
    sei();
}

int collect_delta(int state) {
	rot_position += os_enc_delta();
	return state;
}

int check_switches(int state) {

	if (get_switch_press(_BV(SWN))) {
        //display_string("North\n");
        cli();
        removeWholeBlock(currentBlock);
        currentBlock = rotateBlock(currentBlock);
        drawWholeBlock(currentBlock, DEEP_SKY_BLUE);
        sei();
	}

	if (get_switch_press(_BV(SWE))) {
			//display_string("East\n");
        movePieceRight();
        moveCheck = 1;
	}

	if (get_switch_press(_BV(SWS))) {
		//display_string("South\n");
        movePieceDown();               
	}

	if (get_switch_press(_BV(SWW))) {
        //display_string("West\n");
        movePieceLeft();
        moveCheck = 1;
	}

	if (get_switch_long(_BV(SWC))) {
			//display_string("[S]l Centre\n");

	}

	if (get_switch_short(_BV(SWC))) {
			//display_string("[S]s Centre\n");
	}

	if (get_switch_rpt(_BV(SWN))) {
			//display_string("[R]r North\n");
	}

	if (get_switch_rpt(_BV(SWE))) {
		//display_string("[R]r East\n");
        if (moveCheck <= 0){
            movePieceRight();
        } else {
            moveCheck--;
        }
	}

	if (get_switch_rpt(_BV(SWS))) {
			//display_string("[R]r South\n");
            movePieceDown();
	}

	if (get_switch_rpt(_BV(SWW))) {
		//display_string("[R]r West\n");
        if (moveCheck <= 0){
            movePieceLeft();
        } else {
            moveCheck--;
        }
	}

	if (get_switch_rpt(SWN)) {
			//display_string("[R]r North\n");
	}


	if (get_switch_long(_BV(OS_CD))) {
		//display_string("Detected SD card.\n");
	}

	return state;
}

void movePieceDown(){
    block b = currentBlock;
    b = getLiteralPosVals(b);
    if (blockAboveZero(b)){
        if (!(grid[b.p1.x][b.p1.y - 1] || grid[b.p2.x][b.p2.y - 1] ||
        grid[b.p3.x][b.p3.y - 1] || grid[b.p4.x][b.p4.y  - 1])){
            cli();
            removeWholeBlock(currentBlock);
            currentBlock.start.y -= 1;
            drawWholeBlock(currentBlock, DEEP_SKY_BLUE);
            sei();
        }
    }
}

bool blockAboveZero(block b){
    return b.p1.y > 0 && b.p2.y > 0 && b.p3.y > 0 && b.p4.y > 0;
}

void displayScore(){
    display_score(score, SCREEN_DISPLACEMENT + 100 + 60, 120);
}

void displayNexBlock(){
    removeWholeBlock(nextBlock);
    point p;
    p.x = 17;
    p.y = 16;
    
    nextBlock = getRandomBlock(p);
    drawWholeBlock(nextBlock, DEEP_SKY_BLUE);
}

void movePieceLeft(){
    if(currentBlock.start.x > 0){
        block b = currentBlock;
        b = getLiteralPosVals(b);
        if (!(grid[b.p1.x - 1][b.p1.y] || grid[b.p2.x - 1][b.p2.y] ||
        grid[b.p3.x - 1][b.p3.y] || grid[b.p4.x - 1][b.p4.y])){
            cli();
            removeWholeBlock(currentBlock);
            currentBlock.start.x -= 1;
            drawWholeBlock(currentBlock, DEEP_SKY_BLUE);
            sei();
        }
    }
}

void movePieceRight(){
    block b = currentBlock;
    int maxX = b.p1.x; // stores maximum x
    if (b.p2.x > maxX)
        maxX = b.p2.x;
    if (b.p3.x > maxX)
        maxX = b.p3.x;
    if (b.p4.x > maxX)
        maxX = b.p4.x;
    if (maxX + b.start.x < GRID_WIDTH - 1){
        b = getLiteralPosVals(b);
        if (!(grid[b.p1.x + 1][b.p1.y] || grid[b.p2.x + 1][b.p2.y] ||
        grid[b.p3.x + 1][b.p3.y] || grid[b.p4.x + 1][b.p4.y])){
            cli();
            removeWholeBlock(currentBlock);
            currentBlock.start.x += 1;
            drawWholeBlock(currentBlock, DEEP_SKY_BLUE);
            sei();
        }
    }
    
}

int step(int state){
       
    cli();
    block b = currentBlock;
    b.start.y -= 1;
    if (canPieceMove(b)){
        removeWholeBlock(currentBlock);
        drawWholeBlock(b, DEEP_SKY_BLUE);
        currentBlock = b;        
    } else {
        placeBlock(currentBlock);
        checkRowsFull(currentBlock);
        point p;
        p.x = 4;
        p.y = 21;
        currentBlock = nextBlock;
        currentBlock.start = p;
        displayNexBlock();
    }
    sei();
    return state;
}

void placeBlock(block b){
    b = getLiteralPosVals(b);
    grid[b.p1.x][b.p1.y] = true;
    grid[b.p2.x][b.p2.y] = true;
    grid[b.p3.x][b.p3.y] = true;
    grid[b.p4.x][b.p4.y] = true;  
}

bool canPieceMove(block b){
    b = getLiteralPosVals(b);
    return canMove(b.p1) && canMove(b.p2) && canMove(b.p3) && canMove(b.p4);  
}

//checks if the current block can move down (by checking it's bottom blocks)
bool canMove(point p){
 
	if (p.y < 0 || p.x < 0 || p.x > GRID_WIDTH - 1){
		return false;
	}
    return !grid[p.x][p.y];
}

void checkRowsFull(block b){
    int maxY = b.p1.y; // stores maximum x
    if (b.p2.y > maxY)
        maxY = b.p2.y;
    if (b.p3.y > maxY)
        maxY = b.p3.y;
    if (b.p4.y > maxY)
        maxY = b.p4.y;
    
    maxY += b.start.y;
    
    for (int i = maxY; i >= b.start.y; i--){
        checkRowFull(i);
    }    
}

void checkRowFull(int y){
    bool full = true;
    for (int x = 0; x < GRID_WIDTH; x++){
        if (!grid[x][y]){
            full = false;
        }
    }
    
    if (full){
        score += 1;
        displayScore();
        for (int x = 0; x < GRID_WIDTH; x++){
            point p;
            p.x = x;
            p.y = y;
            removeBlock(p);           
        }
        
        bool empty;
        
        for (int y_pos = y + 1; y_pos < GRID_HEIGHT; y_pos++){
            empty = true;
            for (int x = 0; x < GRID_WIDTH; x++){
                grid[x][y_pos - 1] = grid[x][y_pos];
                if (grid[x][y_pos]){
                    empty = false;
                    point p;
                    p.x = x;
                    p.y = y_pos;
                    removeBlock(p);
                    p.y -= 1;
                    drawBlock(p, DEEP_SKY_BLUE);
                }
            }
            
            if (empty){
                return;
            }
        }
    }
}

// rotates a block by 90 degrees clockwise
block rotateBlock(block b1){
    block b2;
    b2.start = b1.start;
    b2.p1 = b1.p1;
    b2.p2 = b1.p2;
    b2.p3 = b1.p3;
    b2.p4 = b1.p4;
    
    int maxX1 = b2.p1.x; // stores maximum x
    if (b2.p2.x > maxX1)
        maxX1 = b2.p2.x;
    if (b2.p3.x > maxX1)
        maxX1 = b2.p3.x;
    if (b2.p4.x > maxX1)
        maxX1 = b2.p4.x;
        
    // sets new x to old y
    b2.p1.x = b1.p1.y; 
    b2.p2.x = b1.p2.y;
    b2.p3.x = b1.p3.y;
    b2.p4.x = b1.p4.y;
    
    // sets new y to maxX1 - old x
    b2.p1.y = maxX1 - b1.p1.x;
    b2.p2.y = maxX1 - b1.p2.x;
    b2.p3.y = maxX1 - b1.p3.x;
    b2.p4.y = maxX1 - b1.p4.x;
    
    if (canPieceMove(b2)){
        return b2; // if the rotation is possible
    }
    
    return b1;
}

// Takes in two integers, and returns them in a point structure
point setPoint(int x, int y){
    point p;
    p.x = x;
    p.y = y;
    return p;
}

// Returns a random 'block' of the seven shapes available 
block getRandomBlock(point s){
    block b;
    b.start = s;
    
    int r = rand() % 7;
    switch (r){
        case 0:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(0,1);
            b.p3 = setPoint(1,1);
            b.p4 = setPoint(1,2);
            break;
        case 1:
            b.p1 = setPoint(0,1);
            b.p2 = setPoint(0,2);
            b.p3 = setPoint(1,0);
            b.p4 = setPoint(1,1);
            break;
        case 2:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(1,0);
            b.p3 = setPoint(1,1);
            b.p4 = setPoint(2,0);
            break;
        case 3:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(0,1);
            b.p3 = setPoint(1,0);
            b.p4 = setPoint(1,1);
            break;
        case 4:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(1,0);
            b.p3 = setPoint(1,1);
            b.p4 = setPoint(1,2);
            break;
        case 5:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(0,1);
            b.p3 = setPoint(0,2);
            b.p4 = setPoint(1,0);
            break;
        case 6:
            b.p1 = setPoint(0,0);
            b.p2 = setPoint(0,1);
            b.p3 = setPoint(0,2);
            b.p4 = setPoint(0,3);
            break;
        default:
            display_string("Something went wrong with\n block creation\n");
            break;
    }
    return b;
}
