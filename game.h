#ifndef _GAME_H_
#define _GAME_H_

#include "Vector.h"

#define SHOTS_NUM 200

typedef struct input
{
	char forwards;
	char backwards;
	char left;
	char right;
	
	char shoot;
	char dodgeUp;
	char dodgeDown;
} input_t;

typedef struct player
{
	Vector pos;
	float life;
	input_t input;
	
	int dodgeTime;
	float dodgeAcc;
	
	Vector shot[SHOTS_NUM];
	Vector shotDir[SHOTS_NUM];
	int shotAct[SHOTS_NUM];
	int shotInd;
	int shotTime;
} player_t;

void gameInit(void);
int gameUpdate(void);
void gameCleanup(void);

//player_t p[2];

#endif