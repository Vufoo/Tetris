#ifndef GRID_H
#define GRID_H
#include <stdint.h>

/*
struct Grid{
	uint32_t status;
	uint8_t life;
};

typedef const struct Grid gridBlock_t;
*/

struct Block{
	uint32_t x;  //left corner of block (x/column) 25
	uint32_t y;  //left corner of block y 10
	uint32_t bottomX; //x coordinate of block closest to ground
	uint32_t type; //piece type 1-7
	uint32_t status; //if piece is dead(1) or still falling(0)/just spawned
	uint32_t wall; //if piece is on wall or not
	uint32_t direction;//going left(0) or right(1)
};

typedef struct Block Block_t;

void clearPiece(void);
uint8_t checkY(void);
uint8_t checkX(void);
void drawGrid(void);
void drawPiece(void);

void SpawnLPiece(void); // This piece is teal
void SpawnZPiece(void); //This piece is red
void SpawnIPiece(void); //This piece is orange
void SpawnOPiece(void); //This piece is yellow
void SpawnTPiece(void); //This piece is purple
void SpawnJPiece(void);
void SpawnSPiece(void);

void rotateSPiece(void);
void rotateLPiece(void);
void rotateJPiece(void);
void rotateOPiece(void);
void rotateIPiece(void);
void rotateZPiece(void);
void rotateTPiece(void);

void pieceRight(void);
void pieceLeft(void);

uint8_t checkRow(void);
void deleteRow(void);

extern uint32_t Layout[10][25];
extern Block_t piece[4]; 
extern uint32_t rowNumber;


#endif
