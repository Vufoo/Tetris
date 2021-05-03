#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "SSD1306.h"
#include "Sound.h"
#include "Images.h"
#include "grid.h"


uint32_t rowNumber;
uint32_t Layout[10][25]; //[row/y][col/x]

Block_t piece[4] = {{0,0,0,0,0},
										{0,0,0,0,0},
										{0,0,0,0,0},
										{0,0,0,0,0}};


void drawGrid(){

	int xblock = 0;
	int yblock = 0;
	for(int row = 0; row<10; row++){
		for(int col = 0; col<25; col++){
			xblock = (col*5)+1;
			yblock = (row*5)+17;
			if(Layout[row][col]){
				SSD1306_DrawBMP(xblock, yblock, Block, 0, SSD1306_WHITE);
			}
			
			else{
				SSD1306_DrawBMP(xblock, yblock, BlackBlock, 0, SSD1306_WHITE);
			}
		}
	}
	
	/*
	int xblock = 0;
	int yblock = 0;
	for(int col = 0; col<25; col++){
	for(int row = 0; row<10; row++){
		
			xblock = (col*5)+1;
			yblock = (row*5)+17;
			if(Layout[row][col]){
				SSD1306_DrawBMP(xblock, yblock, Block, 0, SSD1306_WHITE);
			}
			else{
				SSD1306_DrawBMP(xblock, yblock, BlackBlock, 0, SSD1306_WHITE);
			}
			
		}
	}
	*/
	
}

uint8_t checkY(void){ //checks if it hit ground or if on top of other another piece
	
	uint32_t x=0;
	uint32_t y=0;
	
	for(int i = 0; i<4; i++){
		x = (piece[i].x -1)/5 ;
	  y = (piece[i].y -17)/5;
		
		if(Layout[y][x+1]!=0){//if it hits other block
			return 1;
		}
		
		if(x==24){ //if it hits bottom row
			return 1;
			
		}
	}
	return 0;
	
}

uint8_t checkX(void){ //checks if hit boundary or side of another block
	
	uint32_t x=0;
	uint32_t y=0;
	
	for(int i = 0; i<4; i++){
		x = (piece[i].x -1)/5 ;
	  y = (piece[i].y -17)/5;
		
		if(y==0 ||y==9){
			if(y==0){
				piece[0].wall = 1;  //on right wall
			}
			if(y==9){
				piece[0].wall = 2; //on left wall
			}
			return 0;
			
		}
	
		if(piece[0].direction == 0){//direction left
			if(Layout[y+1][x]!=0){//if it hits other block on x axis coming left
				return 1;
			}
		}
		if(piece[0].direction == 1){
			if(Layout[y-1][x]!=0){//if it hits other block on x axis coming right
				return 1;
			}
		}
		
		
	}
	piece[0].wall = 0;
	return 0;
	
}

uint8_t checkRow(void){ //if row is filled
	uint32_t flag = 0;
	for(int row = 24; row>=0; row--){
		for(int i = 0; i<10; i++){
			if(Layout[i][row] == 0){
				flag = 0;
				break;
			}				
			else{
				flag = 1;
				
			}
		}
		if(flag){
			rowNumber = row;
			break;
		}
		
	}
	return flag;
}

void deleteRow(void){
	//uint32_t x, y; 
	
	while(rowNumber>=1){
		for(int i = 0; i<10; i++){
			Layout[i][rowNumber] = 0;
			//uint32_t a = (rowNumber*5)+1;
			//uint32_t b = (i*5)+17;
			//SSD1306_DrawBMP(a+5,b, BlackBlock,0,SSD1306_BLACK);
			
			
			if(Layout[i][rowNumber-1]){  //increments whole board downwards after row cleared
				Layout[i][rowNumber-1] = 0;
				//x = ((rowNumber-1)*5)+1;
				//y = (i*5)+17;
				//SSD1306_DrawBMP(x,y, BlackBlock,0,SSD1306_BLACK);
				Layout[i][rowNumber] = 1;
			}	
			
		}
		rowNumber--;
	}
	drawGrid();
	
}


////////SPAWNING

void SpawnSPiece(void){  
	SSD1306_DrawBMP(1, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(1, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 47, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 42;
	piece[1].x = 1;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 42;
	piece[3].x = 6;
	piece[3].y = 47;
	
	piece[0].type = 1;
	piece[0].status = 0; //alive piece
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0; //not hugging wall
	
	
	
}

void SpawnLPiece(void){
	SSD1306_DrawBMP(1, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 47, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 37;
	piece[1].x = 6;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 42;
	piece[3].x = 6;
	piece[3].y = 47;
	
	piece[0].type = 2;
	piece[0].status = 0;
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}

void SpawnJPiece(void){
	SSD1306_DrawBMP(1, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 32, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 42;
	piece[1].x = 6;
	piece[1].y = 42;
	piece[2].x = 6;
	piece[2].y = 37;
	piece[3].x = 6;
	piece[3].y = 32;
	
	piece[0].type = 3;
	piece[0].status = 0;
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}

void SpawnOPiece(void){
	SSD1306_DrawBMP(1, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(1, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 42;
	piece[1].x = 1;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 42;
	piece[3].x = 6;
	piece[3].y = 37;
	
	piece[0].type = 4;
	piece[0].status = 0;
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}

void SpawnIPiece(void){
	SSD1306_DrawBMP(6, 32, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 47, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 6;
	piece[0].y = 32;
	piece[1].x = 6;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 42;
	piece[3].x = 6;
	piece[3].y = 47;
	
	piece[0].type = 5;
	piece[0].status = 0;
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}

void SpawnZPiece(void){
	SSD1306_DrawBMP(1, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(1, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 32, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 42;
	piece[1].x = 1;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 37;
	piece[3].x = 6;
	piece[3].y = 32;
	
	piece[0].type = 6;
	piece[0].status = 0; //which way its rotated
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}


void SpawnTPiece(void){
	SSD1306_DrawBMP(1, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 37, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 42, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(6, 47, Block, 0, SSD1306_WHITE);
	
	piece[0].x = 1;
	piece[0].y = 42;
	piece[1].x = 6;
	piece[1].y = 37;
	piece[2].x = 6;
	piece[2].y = 42;
	piece[3].x = 6;
	piece[3].y = 47;
	
	piece[0].type = 7;
	piece[0].status = 0;
	piece[0].bottomX = 6;  //lowest x of piece
	piece[0].wall = 0;
}

/////////ROTATION

void rotateSPiece(void){  //good
	
	clearPiece();
	
	switch(piece[0].status){
		case 0:
			if((checkX()||checkY())==0){
		//piece[0].x = 1;
		//piece[0].y -=42;
		piece[1].x +=10;
		//piece[1].y = 37;
		//piece[2].x = 6;
		//piece[2].y = 42;
		//piece[3].x -= 5;
		piece[3].y -=10;
	
		piece[0].bottomX +=5;

		piece[0].status = 1; //alive piece
			}
			else{
			piece[0].status = 0;
			}
		break;
		
		case 1:
		
			if((checkX()||checkY())==0){
		//piece[0].x = 0;
		//piece[0].y -=5;
	
		piece[1].x -=10;
		//piece[1].y = 37;
	
		//piece[2].x = 6;
		//piece[2].y = 42;
	
		//piece[3].x -= 5;
		piece[3].y += 10;
	
		piece[0].bottomX -= 5;
		piece[0].status = 0;
			}
			else{
				piece[0].status = 1;
			}
		}
	
	
		drawPiece();
}



void rotateLPiece(void){  //good
	clearPiece();
	
	switch(piece[0].status){
		case 0:
			piece[0].x += 10;
			//piece[0].y = 37;
			piece[1].x += 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -=5;
			piece[3].y -=5;
		
			piece[0].status = 1;
			piece[0].bottomX +=5;
			break;
		
		
		case 1:
			//piece[0].x += 10;
			piece[0].y += 10;
			piece[1].x -= 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x +=5;
			piece[3].y -=5;
		
			piece[0].status = 2;
			break;
		
		case 2:
			piece[0].x -= 10;
			//piece[0].y += 10;
			piece[1].x += 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -=5;
			piece[3].y +=5;
		
			piece[0].status = 3;
			break;
		
		case 3:
			//piece[0].x -= 10;
			piece[0].y -= 10;
			piece[1].x -= 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x +=5;
			piece[3].y +=5;
		
			piece[0].bottomX -=5;
			piece[0].status = 0;
			break;
	}

	drawPiece();
}





void rotateJPiece(void){//good
	
	clearPiece();
	
	switch(piece[0].status){
		case 0:
			//piece[0].x += 10;
			piece[0].y -= 10;
			piece[1].x -= 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x +=5;
			piece[3].y +=5;
		
			piece[0].status = 1;
			piece[0].bottomX +=5;
			break;
		
		
		case 1:
			piece[0].x += 10;
			//piece[0].y += 10;
			piece[1].x += 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -=5;
			piece[3].y +=5;
		
			piece[0].status = 2;
			break;
		
		case 2:
			//piece[0].x -= 10;
			piece[0].y += 10;
			piece[1].x += 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -=5;
			piece[3].y -=5;
		
			piece[0].status = 3;
			break;
		
		case 3:
			piece[0].x -= 10;
			//piece[0].y -= 10;
			piece[1].x -= 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x +=5;
			piece[3].y -=5;
		
			piece[0].bottomX -=5;
			piece[0].status = 0;
			break;
	}
	drawPiece();
}

void rotateOPiece(void){ //good
	//doesnt do anything cause its a square
}

void rotateIPiece(void){//good

	
	clearPiece();
	
	switch(piece[0].status){
	  case 0:
			
			piece[0].x += 10;
			piece[0].y += 10;
			piece[1].x += 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -= 5;
			piece[3].y -= 5;
			
			piece[0].status = 1;
			piece[0].bottomX += 10;
	
			
			break;
		
		case 1:
			
			
			piece[0].x -= 10;
			piece[0].y -= 10;
			piece[1].x -= 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x += 5;
			piece[3].y += 5;
			piece[0].status = 0;
			
			piece[0].bottomX -= 10;
			
			break;
	}
	
	drawPiece();
}

void rotateZPiece(void){//good
	
	clearPiece();
	
	switch(piece[0].status){
		case 0:
		//piece[0].x = 1;
		piece[0].y -= 10;
		piece[1].x += 5;
		piece[1].y -= 5;
		//piece[2].x = 6;
		//piece[2].y = 42;
		piece[3].x += 5;
		piece[3].y += 5;
	
		piece[0].bottomX +=5;

		piece[0].status = 1; 
		break;
		
		case 1:
		//piece[0].x = 0;
		piece[0].y +=10;
	
		piece[1].x -=5;
		piece[1].y += 5;
	
		//piece[2].x = 6;
		//piece[2].y = 42;
	
		piece[3].x -= 5;
		piece[3].y -= 5;
	
		piece[0].bottomX -= 5;
		piece[0].status = 0;
		}

	drawPiece();
}


void rotateTPiece(void){
	
	clearPiece();
	
	switch(piece[0].status){
		case 0:
			//piece[0].x = 1;
			//piece[0].y = 42;
			piece[1].x += 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			//piece[3].x += 5;
			piece[3].y -= 10;
				
			piece[0].status = 1;
			piece[0].bottomX +=5 ;
			break;
		
		
		case 1:
			piece[0].x += 10;
		  //piece[0].y += 5;
			piece[1].x -= 5;
			piece[1].y += 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			//piece[3].x -=5;
			//piece[3].y +=5;
		
			piece[0].status = 2;
			break;
		
		case 2:
			piece[0].x -= 5;
			piece[0].y += 5;
			piece[1].x -= 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x +=5;
			piece[3].y +=5;
		
			piece[0].status = 3;
			break;
		
		case 3:
			piece[0].x -= 5;
			piece[0].y -= 5;
			piece[1].x += 5;
			piece[1].y -= 5;
			//piece[2].x = 6;
			//piece[2].y = 42;
			piece[3].x -=5;
			piece[3].y +=5;
		
			piece[0].bottomX -=5;
			piece[0].status = 0;
			
			break;
	}
	
	drawPiece();
}

void pieceRight(void){
	clearPiece();
	
		//piece[0].direction = 1;
	if(checkX()==0&&(piece[0].wall!=1)){
		for(int i = 0;  i<4; i++){
			piece[i].y -= 5;
		
		}
	}
	drawPiece();
	
}

void pieceLeft(void){
	clearPiece();
	
		//piece[0].direction = 0;
		if(checkX()==0&&(piece[0].wall!=2)){
		for(int i = 0; i<4; i++){
			piece[i].y += 5;
			
			}
		}
	
	drawPiece();
}

void clearPiece(void){
	SSD1306_DrawBMP(piece[0].x, piece[0].y, BlackBlock, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[1].x, piece[1].y, BlackBlock, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[2].x, piece[2].y, BlackBlock, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[3].x, piece[3].y, BlackBlock, 0, SSD1306_WHITE);
}

void drawPiece(void){
	
	SSD1306_DrawBMP(piece[0].x, piece[0].y, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[1].x, piece[1].y, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[2].x, piece[2].y, Block, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(piece[3].x, piece[3].y, Block, 0, SSD1306_WHITE);
}




