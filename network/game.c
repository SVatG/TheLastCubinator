/**
 * Ultrafancy "metasphere" effect.
 */

#include "../tools.h"
#include <stdio.h>
#include <fcntl.h>
#include "../Vector.h"
#include "game.h"

//debug messages to shell
//#define GAME_DEBUG

int time = 0;
player_t p[2];

typedef struct updateData
{
	Vector oldPos;
	Vector movDir;
	Vector rotDir;
} updateData_t;

void gameInit(void) {
	
	p[0].pos = MakeVector(  1.0, 0.0, 0.0 );
	p[1].pos = MakeVector( -1.0, 0.0, 0.0 );
	
	for(int i = 0; i < 2; i++)
	{
		p[i].life = 0.3;
		
		p[i].input.forwards = 0;
		p[i].input.backwards = 0;
		p[i].input.left = 0;
		p[i].input.right = 0;
		
		p[i].input.shoot = 0;
		p[i].input.dodgeUp = 0;
		p[i].input.dodgeDown = 0;
		
		p[i].dodgeTime = 0;
		p[i].dodgeAcc = 0;
		p[i].shotTime = 0;
		
		p[i].shotInd = 0;
		for(int j = 0; j < 200; j++)
		{
			p[i].shotAct[j] = 0;
		}
	}
}

// Called every 10 ms
int gameUpdate(void)
{
	time++;
	updateData_t data[2];
	
	// Save some important values.
	for(int i = 0; i < 2; i++)
	{
		data[i].oldPos = p[i].pos;
		p[i].pos.y = 0;
	}
	Scalar dist = VectorDistance(p[0].pos, p[1].pos);
	for(int i = 0; i < 2; i++)
	{
		data[i].movDir = VectorNorm(VectorSub(p[!i].pos, p[i].pos));
		data[i].rotDir = VectorNorm(VectorCross(data[i].movDir, YVector));
	}
	// Rotational movement.
	for(int i = 0; i < 2; i++)
	{
		if(p[i].input.right)
		{
			p[i].pos = VectorAdd(p[i].pos, VectorMul(data[i].rotDir, 0.05));
		}
		if(p[i].input.left)
		{
			p[i].pos = VectorSub(p[i].pos, VectorMul(data[i].rotDir, 0.05));
		}
		p[i].pos = VectorAdd(p[i].pos, VectorMul(data[i].movDir, VectorDistance(p[i].pos, p[!i].pos) - dist));
	}
	for(int i = 0; i < 2; i++)
	{
		// Axis movement.
		if(p[i].input.forwards)
		{
			p[i].pos = VectorAdd(p[i].pos, VectorMul(data[i].movDir, 0.05));
		}
		if(p[i].input.backwards)
		{
			p[i].pos = VectorSub(p[i].pos, VectorMul(data[i].movDir, 0.05));
		}
		// Dodge movement
		p[i].pos.y = data[i].oldPos.y;
		if(time - p[i].dodgeTime > 60)
		{
			if(p[i].input.dodgeUp)
			{
				p[i].dodgeTime = time;
				p[i].dodgeAcc = 0.15;
			}
			else if(p[i].input.dodgeDown)
			{
				p[i].dodgeTime = time;
				p[i].dodgeAcc = -0.15;
			}
			else
			{
				p[i].pos.y = 0;
				p[i].dodgeAcc = 0;
			}
		}
		else
		{
			p[i].dodgeAcc = (p[i].dodgeAcc + 0.01 * sgn(p[i].pos.y)) * 0.93;
			p[i].pos.y = p[i].pos.y - p[i].dodgeAcc;
		}
	}
	// Check out-of-bounds.
	for(int i = 0; i < 2; i++)
	{
		if(VectorDistance(p[i].pos, ZeroVector ) > 10.0)
		{
			Scalar sd = VectorDistance(p[i].pos, ZeroVector) - 10.0;
			p[i].pos = VectorSub(p[i].pos, VectorMul(VectorNorm(VectorSub(p[i].pos, ZeroVector)), sd));
		}
	}
	if(VectorDistance(p[0].pos, p[1].pos) < 1)
	{
		float abd = (VectorDistance(p[0].pos, p[1].pos) - 1) / 2;
		for(int i = 0; i < 2; i++) {p[i].pos = VectorAdd(p[i].pos, VectorMul(data[i].movDir, abd));}
	}
	for(int i = 0; i < 2; i++)
	{
		// Shoot
		data[i].movDir = VectorNorm(VectorSub(p[!i].pos, p[i].pos));
		if(p[i].input.shoot && time - p[i].shotTime > 2 )
		{
			p[i].shot[p[i].shotInd] = p[i].pos;
			p[i].shotDir[p[i].shotInd] = VectorMul(data[i].movDir, 0.1);
			p[i].shotAct[p[i].shotInd] = 1;
			p[i].shotInd = (p[i].shotInd + 1) % SHOTS_NUM;
			p[i].shotTime = time;
		}
		// Move bullets
		for(int j = 0; j < SHOTS_NUM; j++ ) {
			if(p[i].shotAct[j])
			{
				p[i].shot[j] = VectorAdd(p[i].shot[j], p[i].shotDir[j]);
				if(VectorDistance(p[i].shot[j], p[!i].pos) < 0.2 )
				{
					p[i].shotAct[j] = 0;
					p[!i].life -= 0.01;
					
					//printf("Shot hit %d!\n", !i);
				}
				else if(VectorDistance(ZeroVector, p[i].shot[j]) > 10.3)
				{
					p[i].shotAct[j] = 0;
				}
			}
		}
	}
	return 0;
}

// Clean up what needs cleaning up.
void gameCleanup(void) {
	// Nothing
}
