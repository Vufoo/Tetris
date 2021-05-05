// Lab10.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/16/2021 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

//************WARNING***********
// The LaunchPad has PB7 connected to PD1, PB6 connected to PD0
// Option 1) do not use PB7 and PB6
// Option 2) remove 0-ohm resistors R9 R10 on LaunchPad
//******************************

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "SSD1306.h"
#include "Print.h"
#include "Random.h"
#include "ADC.h"
#include "DAC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "TExaS.h"
#include "Switch.h"
#include "grid.h"

//********************************************************************************
// debuging profile, pick up to 7 unused bits and send to Logic Analyzer
#define PB54                  (*((volatile uint32_t *)0x400050C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// use for debugging profile
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PB5       (*((volatile uint32_t *)0x40005080)) 
#define PB4       (*((volatile uint32_t *)0x40005040)) 

// TExaSdisplay logic analyzer shows 7 bits 0,PB5,PB4,PF3,PF2,PF1,0 
// edit this to output which pins you use for profiling
// you can output up to 7 pins
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PB54; // sends at 10kHz
}
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// edit this to initialize which pins you use for profiling
void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x32;      // activate port B,F,E
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DEN_R |=  0x0E;   // enable digital I/O on PF3,2,1
  GPIO_PORTB_DIR_R |=  0x30;   // output on PB4 PB5
  GPIO_PORTB_DEN_R |=  0x30;   // enable on PB4 PB5  
	GPIO_PORTE_DIR_R &= ~0x0F;   // input on PE 3-0
	GPIO_PORTE_DEN_R |= 0x0F;    // enable PE3-PE0
		
		
}
//********************************************************************************
 
void Delay100ms(uint32_t count); // time delay in 0.1 seconds



void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 (built-in LED)
  GPIO_PORTF_PUR_R |= 0x10;
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF
}

void SysTick_Init(void){ 
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 4000000; // triggers every 50 ms
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000; // priority 1
	NVIC_ST_CTRL_R = 0X0007;
}
typedef enum {English, Spanish} Language_t;
Language_t myLanguage = English;
typedef enum {SCORE, GAMEOVER, LANGUAGE} phrase_t;
 char Score_English[] ="Score:";
 char Score_Spanish[] ="Puntuaci\xA2n";
 char Language_English[]="English";
 char Language_Spanish[]="French";
 const char *Phrases[4]={
	Score_English, Score_Spanish, Language_English, Language_Spanish
};


char language;
char Words[2] = {'e', 's'};

uint32_t spawn = 1; //1 = spawn new piece
uint32_t speed = 20;  
uint32_t level = 20;	//speed of falling block initially
uint32_t semaphore = 0;	//getting adc data flag
uint32_t count=0;	//counter of systick
uint32_t time=0; 	//time counter
uint32_t ADCdata;
uint32_t currentPosition=110; //middle position
uint32_t score = 0;
//uint32_t scoreflag = 1;
uint32_t increment = 0;
 
void SysTick_Handler(void){ 
	ADCdata = ADC_In();
	semaphore = 1;
	
		
	
	if(count%level==0){
	
		
		//SSD1306_OutBuffer();
		clearPiece();
		PF1 ^= 0x02;
		time++;
		
		if(checkY()==0){		//if the block still needs to fall and not blocked by boundar or pieces
			for(int i = 0; i<4; i++){
				piece[i].x += 5;
			}
			piece[0].bottomX += 5;
			drawPiece();
		}
		
		else{		// if piece stops falling and set to grid
			uint32_t x, y;  
			for(int i = 0; i<4; i++){
				x = (piece[i].x-1)/5;
				y = (piece[i].y-17)/5;
				Layout[y][x] = 1; 
			} 
			drawGrid();
			spawn = 1;
		}
		SSD1306_OutBuffer();
		
	}
	
	if(count%200==0&&score!=0){ //increment speed every 10 seconds
		if(speed==1){
			speed = 1;
		}
		else{
			speed--;
		}
		
	}
	
	
  if(count%100 ==0){
		Sound_Etc();
	}
	
	count++;
}

uint32_t random(void){//returns 1-7 randomly
	return ((Random32()>>24)%7)+1;//(rand()%(7-1+1))+1;
}

void clearLayout(void){		//clears grid
	for(uint32_t i = 0; i<10; i++){
		for(uint32_t j = 0; j<25; j++){
			Layout[i][j] = 0;
		}
	}
}

uint32_t Convert(uint32_t data){ //slide potentiometer
	
	uint32_t difference;
  
	uint32_t conversion = (175*data)/4096+17; //number 17-191
	
	//if(piece[0].wall!=2){
	if(currentPosition<conversion){ //left
		difference = conversion - currentPosition;
		difference /= 10;
		for(int i = 0; i<difference; i++){
				piece[0].direction = 0;  //going left
			  //if(checkX() == 0||(piece[0].wall!=2)){
				pieceLeft();
				//}
				
		}
		currentPosition = conversion;
	}
//}
	
	//if(piece[0].wall!=1){
	if(currentPosition>conversion){ //right
		difference = currentPosition - conversion;
		difference /= 10;
		for(int i = 0; i<difference; i++){
				piece[0].direction = 1;     //going right
				//if(checkX()==0||(piece[0].wall==1)){
				pieceRight();
				//}
		}
		currentPosition = conversion;
	}
//}
	SSD1306_OutBuffer();
	return 0;
}





void startGame(){
	SSD1306_DrawBMP(0, 63, border5, 0, SSD1306_WHITE); //border outline
	SSD1306_OutBuffer();
	

  while(1){
		if(Layout[0][0]==1||Layout[1][0]==1||Layout[2][0]==1||Layout[3][0]==1||Layout[4][0]==1||Layout[5][0]==1||Layout[6][0]==1||Layout[7][0]==1||Layout[8][0]==1||Layout[9][0]==1){//gameover
			Sound_GameOver();
			break;
		}
		
		if(language == 'e'){

		SSD1306_SetCursor(10,0);
		SSD1306_OutString("score");
		}
		else{
			SSD1306_SetCursor(7,0);
			SSD1306_OutString("Puntuacion");
		}
	
		SSD1306_SetCursor(16,0);
		SSD1306_OutUDec16(score);
		SSD1306_SetCursor(0,0);
		SSD1306_OutUDec16(21-speed);
		
		if(spawn==1){//if need to spawn
			switch(random()){
				case 1:
					SpawnSPiece();
					break;
				case 2:
					SpawnLPiece();
					break;
				case 3:
					SpawnJPiece();
					break;
				case 4:
					SpawnOPiece();
					break;
				case 5:
					SpawnIPiece();
					break;
				case 6:
					SpawnZPiece();
					break;
				case 7:
					SpawnTPiece();
					break;
				
			}
			spawn = 0;
		}
		
		if((GPIO_PORTE_DATA_R&0x01)!=0){ //rotating piece
			while((GPIO_PORTE_DATA_R&0x01)!=0){}
			
			switch(piece[0].type){
				case 1:
					rotateSPiece();
					break;
				case 2:
					rotateLPiece();
					break;
				case 3:
					rotateJPiece();
					break;
				case 4:
					rotateOPiece();//doesnt do anything
					break; 
				case 5:
					rotateIPiece();
					break;
				case 6:
					rotateZPiece();
					break;
				case 7:
					rotateTPiece();
					break;
			}
			
			SSD1306_OutBuffer();
			
		}
		if(checkRow()==1){
			deleteRow();
			SSD1306_OutBuffer();
			score += 20;
			Sound_Shoot();
		}
		
		
		if((GPIO_PORTE_DATA_R&0x02)!=0){
			
			while((GPIO_PORTE_DATA_R&0x02)!=0){
				level = 1;
			}
		}
		
		level = speed;
	  while(semaphore == 0){}
		Convert(ADCdata);
		semaphore = 0;
		
		
		
  }
}

///////////////MAIN//////////////////
int main(void){
  DisableInterrupts();
	language = Words[0];
  // pick one of the following three lines, all three set to 80 MHz
  PLL_Init();                   // 1) call to have no TExaS debugging
	DAC_Init();
	Random_Init(1);
	Sound_Init();
	ADC_Init();
	PortF_Init();
	//Profile_Init();
  TExaS_Init(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_Init(&ScopeTask);       // or 3) call to activate analog scope PD2
	
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear();   
  Random_Init(1);
  Profile_Init(); // PB5,PB4,PF3,PF2,PF1 
		SSD1306_DrawBMP(0,63, tetris2, 0, SSD1306_WHITE);
		SSD1306_OutBuffer();
		SSD1306_SetCursor(8,5);
		SSD1306_OutString("PLAY!");
		SSD1306_SetCursor(7,7);
		SSD1306_OutString("ENG/ESP");
		
		
		//SSD1306_DrawBMP(0,63, tetris2, 0, SSD1306_WHITE);
		
		//SSD1306_OutBuffer();
		


		while(1){
			if((GPIO_PORTE_DATA_R&0x01)==1){		//button to play game
				break;
			}
			if((GPIO_PORTE_DATA_R&0x04)!=0){		//button to change to english
				
				language = Words[0];
				SSD1306_SetCursor(7,7);
				SSD1306_OutString("ENGLISH");
				SSD1306_SetCursor(8,5);
				SSD1306_OutString("PLAY!");
				
				
			}
			if((GPIO_PORTE_DATA_R&0x08)!=0){		//button to change to spanish
				
				language = Words[1];
				SSD1306_SetCursor(7,7);
				SSD1306_OutString("SPANISH");
				SSD1306_SetCursor(8,5);
				SSD1306_OutString("JUGAR");
				
			}
			
		}
		
		EnableInterrupts();
		SysTick_Init(); //game starts
		
		while(1){
		
		startGame();			//starts game
											//game ends
		//level = 4000000000;  //stops incrementing after game ends
		NVIC_ST_CTRL_R=5; //stop interrupt

		clearLayout();		//clears grid
		//scoreflag = 0;		// dont show score on end
		
		while((GPIO_PORTE_DATA_R&0x01)==0){
			
			if(language == 'e'){
			SSD1306_SetCursor(6,4);
			SSD1306_OutString("GAME OVER");
			}
			else{
				SSD1306_SetCursor(3,4);
				SSD1306_OutString("JUEGO TERMINADO");
			}
		
		}
		//scoreflag = 1; //  display score on after game over screen
		speed = 20; //reset back to normal speed
		score = 0;
		NVIC_ST_CTRL_R=7; //start interrupt
		
	

		}
}






// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

