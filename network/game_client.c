/**
Game functions only used by the client. Includes all drawing.
*/

#include "../tools.h"
#include <stdio.h>
#include <fcntl.h>
#include "../Vector.h"
#include "game.h"

#define R 0
#define G 1
#define B 2
#define A 3

#define PLAYER 0
#define SHOT 1

float colors[2][2][4] = {{{0, 0.4, 1, 1}, {0, 0.4, 1, 1}}, {{1, 0, 0.4, 1}, {1, 0, 0.4, 1}}};

//from game.c
extern player_t p[2];
extern int time;

//draws a cube at position (x, y, z); front and top define the rotation of the cube and must be orthogonal to each other
void cubeRotation(float x, float y, float z, float d, Vector front, Vector top)
{
	glBegin(GL_QUADS);
	
	Vector right = VectorNorm(VectorCross(front, top));
	front = VectorNorm(front);
	top = VectorNorm(top);
	
	/*
	front right top = (x, y, z) + d * front + d * right + d * top
	= (x, y, z) + d * (front + right + top)
	*/
	Vector frt = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(front, right, top), d));
	Vector frb = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(front, right, VectorNeg(top)), d));
	Vector flt = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(front, VectorNeg(right), top), d));
	Vector flb = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(front, VectorNeg(right), VectorNeg(top)), d));
	Vector brt = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(VectorNeg(front), right, top), d));
	Vector brb = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(VectorNeg(front), right, VectorNeg(top)), d));
	Vector blt = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(VectorNeg(front), VectorNeg(right), top), d));
	Vector blb = VectorAdd(MakeVector(x, y, z), VectorMul(VectorAdd3(VectorNeg(front), VectorNeg(right), VectorNeg(top)), d));
	
	//Top
	glNormal3f(top.x, top.y, top.z);
	glVertex3f(frt.x, frt.y, frt.z);
	glVertex3f(brt.x, brt.y, brt.z);
	glVertex3f(blt.x, blt.y, blt.z);
	glVertex3f(flt.x, flt.y, flt.z);
	
	//Bottom
	glNormal3f(-top.x, -top.y, -top.z);
	glVertex3f(frb.x, frb.y, frb.z);
	glVertex3f(brb.x, brb.y, brb.z);
	glVertex3f(blb.x, blb.y, blb.z);
	glVertex3f(flb.x, flb.y, flb.z);
	
	//Left
	glNormal3f(-right.x, -right.y, -right.z);
	glVertex3f(flt.x, flt.y, flt.z);
	glVertex3f(blt.x, blt.y, blt.z);
	glVertex3f(blb.x, blb.y, blb.z);
	glVertex3f(flb.x, flb.y, flb.z);
	
	//Right
	glNormal3f(right.x, right.y, right.z);
	glVertex3f(frt.x, frt.y, frt.z);
	glVertex3f(brt.x, brt.y, brt.z);
	glVertex3f(brb.x, brb.y, brb.z);
	glVertex3f(frb.x, frb.y, frb.z);
	
	//Front
	glNormal3f(front.x, front.y, front.z);
	glVertex3f(frt.x, frt.y, frt.z);
	glVertex3f(frb.x, frb.y, frb.z);
	glVertex3f(flb.x, flb.y, flb.z);
	glVertex3f(flt.x, flt.y, flt.z);
	
	// Back
	glNormal3f(-front.x, -front.y, -front.z);
	glVertex3f(brt.x, brt.y, brt.z);
	glVertex3f(brb.x, brb.y, brb.z);
	glVertex3f(blb.x, blb.y, blb.z);
	glVertex3f(blt.x, blt.y, blt.z);
	
	glEnd();
}

void graphicInit(void) {
	// General
	glEnable( GL_DEPTH_TEST );
	glClearDepth( 1.0f );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel( GL_SMOOTH );
	//glEnable( GL_CULL_FACE );
	//glCullFace( GL_BACK );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	
	// Lights.
	glEnable( GL_LIGHT0 );
	
	GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat position[] = { -10.0f, 10.0f, 0.0f, 1.0f };
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight );
	glLightfv( GL_LIGHT0, GL_POSITION, position );
	
	glEnable( GL_LIGHTING );
	
	// Perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 90.0, screenWidth / screenHeight, 1.0, 200.0 );
	
	// LINES.
	glLineWidth( 2.0 );
}

// Called by GLUT to draw things.
void gameDraw()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	// Camera
	Scalar dist = VectorDistance(p[0].pos, p[1].pos);
	Vector direction = VectorSub(p[0].pos, p[1].pos);
	Vector mid = VectorAdd(p[1].pos, VectorMul(direction, 0.5));
	Vector cam = VectorAdd(mid, MakeVector(dist, dist, dist));
	gluLookAt(cam.x, 4, cam.z, mid.x, mid.y, mid.z, 0, 1, 0);
	
	// Some background
	glDisable( GL_LIGHT0 );
	glColor4f( 1.0, 0.4, 0.0, 1.0 );
	for( int c = -50; c < 11; c++ ) {
		for( int d = 0; d < 20; d++ ) {
			float i = (d-(time/100.0));
			glColor4f( 1.0, 0.4, 0.0, 1.0 );
			int rtime = time % 50;
			float rfact = c + (rtime/50.0);
			float xmod = sin(deg2rad(18 * i)) * (11.0 - rfact);
			float zmod = cos(deg2rad(18 * i)) * (11.0 - rfact);
			float ymod = -pow(2, rfact) * 0.1 - 1.0;
			cubeRotation(xmod, ymod, zmod, 0.1, MakeVector(xmod, 0, zmod), YVector);
			float xd = sin(deg2rad(18 * (i + 1))) * (11.0 - rfact) - xmod;
			float zd = cos(deg2rad(18 * (i + 1))) * (11.0 - rfact) - zmod;
			float xdd = sin(deg2rad(18 * i)) * (10.0 - rfact) - xmod;
			float zdd = cos(deg2rad(18 * i)) * (10.0 - rfact) - zmod;
			glColor4f(1.0, 0.4, 1.0, 1.0);
			glBegin( GL_LINES );
				glVertex3f(xmod + xd, ymod, zmod + zd);
				glVertex3f(xmod, ymod, zmod);
				glVertex3f(xmod + xdd, ymod - pow(2, rfact) * 0.1, zmod + zdd);
				glVertex3f(xmod, ymod, zmod);
			glEnd();
		}
		
	}
	glEnable( GL_LIGHT0 );
	
	// Border!
	glColor4f( 0.4, 1.0, 0.0, 1.0 );
	for( int i = 0; i < 180; i++ ) {
		float xmod = sin( 2*(i+time/10.0)*(PI/180.0) ) * 10.3;
		float zmod = cos( 2*(i+time/10.0)*(PI/180.0) ) * 10.3;
		cubeRotation(xmod, 0, zmod, 0.05, MakeVector(xmod, 0, zmod), YVector);
	}
	
	//how calculate the normal for the top face of the player cube?
	// - calculate the cross of front and the y axis (make a vector in the xz plane)
	// - calculate the cross of front and the xz plane vector
	//looking for an easier way...
	for(int i = 0; i < 2; i++)
	{
		Vector cubeFront = VectorSub(p[!i].pos, p[i].pos);
		Vector cubeSide = VectorCross(cubeFront, YVector);
		glColor4f(colors[i][PLAYER][R], colors[i][PLAYER][G], colors[i][PLAYER][B], colors[i][PLAYER][A]);
		cubeRotation(p[i].pos.x, p[i].pos.y, p[i].pos.z, p[i].life, cubeFront, VectorCross(cubeFront, cubeSide));
	}
	
	// Circles.
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for(int i = 0; i < 720; i++)
	{
		float xs = sin(deg2rad(0.5 * i)) * dist;
		float zs = cos(deg2rad(0.5 * i)) * dist;
		float xe = sin(deg2rad(0.5 * (i + 1))) * dist;
		float ze = cos(deg2rad(0.5 * (i + 1))) * dist;
		for(int j = 0; j < 2; j++)
		{// B = j; A = !j
			float yd = 2.0 - VectorDistance(MakeVector(xs + p[j].pos.x, p[!j].pos.y, zs + p[j].pos.z), p[!j].pos);
			float ys = 0;
			if(yd > 0)
			{
				ys = yd * yd * yd * yd * p[!j].pos.y / 10;
				ys = ys > 0 ? min(p[!j].pos.y, ys) : ys < 0 ? max(p[!j].pos.y, ys) : 0;
			}
			yd = 2.0 - VectorDistance(MakeVector(xe + p[j].pos.x, p[!j].pos.y, ze + p[j].pos.z), p[!j].pos);
			float ye = 0;
			if(yd > 0)
			{
				ye = yd * yd * yd * yd * p[!j].pos.y / 10;
				ye = ye > 0 ? min(p[!j].pos.y, ye) : ye < 0 ? max(p[!j].pos.y, ye) : 0;
			}
			if((xs + p[j].pos.x) * (xs + p[j].pos.x) + (zs + p[j].pos.z) * (zs + p[j].pos.z) > 105.0 ||
				(xe + p[j].pos.x) * (xe + p[j].pos.x) + (ze + p[j].pos.z) * (ze + p[j].pos.z) > 105.0)
			{
				glColor4f( 0.2, 0.2, 0.2, 1.0 );
			}
			else {
				glColor4f( 0.5, 0.5, 0.5, 1.0 );
			}
			glVertex3f(xs + p[j].pos.x, ys, zs + p[j].pos.z);
			glVertex3f(xe + p[j].pos.x, ye, ze + p[j].pos.z);
			
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
	
	// CHEEEEEEEEEEEEEN
	// Bullets
	for(int i = 0; i < 2; i++)
	{
		glColor4f(colors[i][SHOT][R], colors[i][SHOT][G], colors[i][SHOT][B], colors[i][SHOT][A]);
		for( int j = 0; j < SHOTS_NUM; j++ ) {
			if(p[i].shotAct[j] ) {
				cubeRotation(p[i].shot[j].x, p[i].shot[j].y, p[i].shot[j].z, 0.1, p[i].shotDir[j], YVector);
			}
		}
	}
}

// Clean up what needs cleaning up.
void graphicCleanup(void) {
	// Nothing
}
