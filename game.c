/**
 * Ultrafancy "metasphere" effect.
 */

#include "tools.h"
#include <stdio.h>
#include <fcntl.h>
#include "Vector.h"

//debug messages to shell
//#define GAME_DEBUG

//enable cube rotation code
#define CUBE_ROTATION

int time = 0;

// Player A shooting
Vector shotA[200];
Vector shotAd[200];
int shotAa[200];
int shotAind;
int shotAtime;

// Player B shooting
Vector shotB[200];
Vector shotBd[200];
int shotBa[200];
int shotBind;
int shotBtime;

// Life counters
float lifeA = 0.3;
float lifeB = 0.3;

// Dodging
int dodgeATime;
int dodgeBTime;
float dodgeAacc;
float dodgeBacc;


//draws a cube at position (x, y, z); front and top define the rotation of the cube and must be orthogonal to each other
void cubeRotation(float x, float y, float z, float d, Vector front, Vector top)
{
	glBegin( GL_QUADS );

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

// Variables
Vector posA;
Vector posB;

void gameInit(void) {
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

	// Data
	posA = MakeVector(  1.0, 0.0, 0.0 );
	posB = MakeVector( -1.0, 0.0, 0.0 );

	// LINES.
	glLineWidth( 2.0 );

	// Shots
	shotAind = 0;
	shotBind = 0;
	for( int i = 0; i < 200; i++ ) {
		shotAa[i] = 0;
		shotBa[i] = 0;
	}
}

// Called by GLUT to draw things.
void gameDraw() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Camera
	Scalar dist = VectorDistance( posA, posB );
	Vector direction = VectorSub( posA, posB );
	Vector mid = VectorAdd( posB, VectorMul( direction, 0.5 ) );
	Vector cam = VectorAdd( mid, MakeVector( dist, dist, dist ) );
	gluLookAt( cam.x, 4.0, cam.z, mid.x, mid.y, mid.z, 0, 1, 0 );

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
	
	// Player positions.
// 	Vector movDir = VectorNorm( VectorSub( posA, posB ) );
// 	Vector rotDir = VectorNorm( VectorCross( YVector, movDir ) );
	Vector posAo = posA;
	posAo.y = 0;
	Vector posBo = posB;
	posBo.y = 0;
	
	// Player A.
	//how calculate the normal for the top face of the player cube?
	// - calculate the cross of front and the y axis (make a vector in the xz plane)
	// - calculate the cross of front and the xz plane vector
	//looking for an easier way...
	Vector cubeFront = VectorSub(posB, posA);
	Vector cubeSide = VectorCross(cubeFront, YVector);
	glColor4f(0.0, 0.4, 1.0, 1.0);
	cubeRotation(posA.x, posA.y, posA.z, lifeA, cubeFront, VectorCross(cubeFront, cubeSide));

	// Player B.
	cubeFront = VectorSub(posA, posB);
	cubeSide = VectorCross(cubeFront, YVector);
	glColor4f(1.0, 0.0, 0.4, 1.0);
	cubeRotation(posB.x, posB.y, posB.z, lifeB, cubeFront, VectorCross(cubeFront, cubeSide));
	
	// Circles.
	glDisable( GL_LIGHTING );
	glBegin( GL_LINES );
	for( int i = 0; i < 720; i++ ) {
		// General: x/z
		float xs = sin(deg2rad(0.5 * i)) * dist;
		float zs = cos(deg2rad(0.5 * i)) * dist;
		float xe = sin(deg2rad(0.5 * (i + 1))) * dist;
		float ze = cos(deg2rad(0.5 * (i + 1))) * dist;

		// Per-player: y, player A
		float yd = 2.0 - VectorDistance(
			MakeVector( xs + posB.x, 0, zs + posB.z ), posAo );
		yd = yd < 0 ? 0 : yd*yd;
		float ys = ((yd*yd)/10.0)*posA.y;
		ys = ys > 0.0 && ys > posA.y ? posA.y : ys;
		ys = ys < 0.0 && ys < posA.y ? posA.y : ys;
		yd = 2.0 - VectorDistance(
			MakeVector( xe + posB.x, 0, ze + posB.z ), posAo );
		yd = yd < 0 ? 0 : yd*yd;
		float ye = ((yd*yd)/10.0)*posA.y;
		ye = ye > 0.0 && ye > posA.y ? posA.y : ye;
		ye = ye < 0.0 && ye < posA.y ? posA.y : ye;
		
		// Draw player A circle
		if( (xs+posB.x)*(xs+posB.x)+(zs+posB.z)*(zs+posB.z) > 105.0 ||
			(xe+posB.x)*(xe+posB.x)+(ze+posB.z)*(ze+posB.z) > 105.0)
		{
			glColor4f( 0.2, 0.2, 0.2, 1.0 );
		}
		else {
			glColor4f( 0.5, 0.5, 0.5, 1.0 );
		}
		glVertex3f( xs + posB.x, ys, zs + posB.z );
		glVertex3f( xe + posB.x, ye, ze + posB.z );

		// Per-player: y, player B
		yd = 2.0 - VectorDistance(
			MakeVector( xs + posA.x, 0, zs + posA.z ), posBo );
		yd = yd < 0 ? 0 : yd*yd;
		ys = ((yd*yd)/10.0)*posB.y;
		ys = ys > 0.0 && ys > posB.y ? posB.y : ys;
		ys = ys < 0.0 && ys < posB.y ? posB.y : ys;
		yd = 2.0 - VectorDistance(
			MakeVector( xe + posA.x, 0, ze + posA.z ), posBo );
		yd = yd < 0 ? 0 : yd*yd;
		ye = ((yd*yd)/10.0)*posB.y;
		ye = ye > 0.0 && ye > posB.y ? posB.y : ye;
		ye = ye < 0.0 && ye < posB.y ? posB.y : ye;
		
		// Draw player B circle
		if( (xs+posA.x)*(xs+posA.x)+(zs+posA.z)*(zs+posA.z) > 105.0 ||
			(xe+posA.x)*(xe+posA.x)+(ze+posA.z)*(ze+posA.z) > 105.0)
		{
			glColor4f( 0.2, 0.2, 0.2, 1.0 );
		}
		else {
			glColor4f( 0.5, 0.5, 0.5, 1.0 );
		}
		glVertex3f( xs + posA.x, ys, zs + posA.z );
		glVertex3f( xe + posA.x, ye, ze + posA.z );
	}
	glEnd();
	glEnable( GL_LIGHTING );
	
	// CHEEEEEEEEEEEEEN
	// Bullets
	glColor4f( 0.0, 0.4, 1.0, 1.0 );
	for( int i = 0; i < 200; i++ ) {
		if( shotAa[i] ) {
			cubeRotation(shotA[i].x, shotA[i].y, shotA[i].z, 0.1, shotAd[i], YVector);
		}
	}
	glColor4f( 1.0, 0.0, 0.4, 1.0 );
	for( int i = 0; i < 200; i++ ) {	
		if( shotBa[i] ) {
			cubeRotation(shotB[i].x, shotB[i].y, shotB[i].z, 0.1, shotBd[i], YVector);
		}
	}

#ifdef GAME_DEBUG
	printf("Player A: %f, %f, %f --- Player B: %f, %f, %f\n", posA.x, posA.y,
			posA.z, posB.x, posB.y, posB.z);
#endif
}

// Called every 10 ms
// Returns 1 for "effect over"
int gameUpdate(void) {
	time++;
	
	// Save some important values.
	Vector posAo = posA;
	Vector posBo = posB;
	posA.y = 0;
	posB.y = 0;
	Scalar dist = VectorDistance( posA, posB );
	Vector movDir = VectorNorm( ProjectVectorToPlane(
		VectorSub( posA, posB ), YVector ) );
	movDir.y = 0.0;
	
#ifdef GAME_DEBUG
	printf( "%f, %f, %f\n", movDir.x, movDir.y, movDir.z );
#endif
	
	Vector rotDir = VectorNorm( VectorCross( YVector, movDir ) );
	
	// Rotational movement.
	if( specialKey[ GLUT_KEY_LEFT ] ) {
		posA = VectorSub( posA, VectorMul( rotDir, 0.05 ) );
	}
	if( specialKey[ GLUT_KEY_RIGHT ] ) {
		posA = VectorAdd( posA, VectorMul( rotDir, 0.05 ) );
	}
	posA = VectorSub( posA, VectorMul(
		movDir, VectorDistance( posA, posB ) - dist ) );
	if( key[ 'a' ] ) {
		posB = VectorAdd( posB, VectorMul( rotDir, 0.05 ) );
	}
	if( key[ 'd' ] ) {
		posB = VectorSub( posB, VectorMul( rotDir, 0.05 ) );
	}
	posB = VectorSub( posB, VectorMul( movDir,
		dist - VectorDistance( posA, posB ) ) );
		
	// Axis movement.
	if( specialKey[ GLUT_KEY_UP ] ) {
		posA = VectorSub( posA, VectorMul( movDir, 0.05 ) );
	}
	if( specialKey[ GLUT_KEY_DOWN ] ) {
		posA = VectorAdd( posA, VectorMul( movDir, 0.05 ) );
	}
	if( key[ 'w' ] ) {
		posB = VectorAdd( posB, VectorMul( movDir, 0.05 ) );
	}
	if( key[ 's' ] ) {
		posB = VectorSub( posB, VectorMul( movDir, 0.05 ) );
	}

	// Dodge movement
	posA.y = posAo.y;
	posB.y = posBo.y;
	
	if( time - dodgeBTime > 60 ) {
		if( key[ 'g' ] ) {
			dodgeBTime = time;
			dodgeBacc = -0.15;
		}
		else if( key[ 'h' ] ) {
			dodgeBTime = time;
			dodgeBacc = +0.15;
		}
	}
	if( time - dodgeBTime > 60 ) {
		posB.y = 0;
		dodgeBacc = 0;
	}
	else {
		dodgeBacc = (dodgeBacc + 0.01 * sgn(posB.y)) * 0.93;
		posB.y = posB.y - dodgeBacc;
	}
	
	if( time - dodgeATime > 60 ) {
		if( key[ ',' ] ) {
			dodgeATime = time;
			dodgeAacc = -0.15;
		}
		else if( key[ '.' ] ) {
			dodgeATime = time;
			dodgeAacc = +0.15;
		}
	}
	if( time - dodgeATime > 60 ) {
		posA.y = 0;
		dodgeAacc = 0;
	}
	else {
		dodgeAacc = (dodgeAacc + 0.01 * sgn(posA.y)) * 0.93;
		posA.y = posA.y - dodgeAacc;
	}
	
	// Check out-of-bounds.
	if( VectorDistance( posA, ZeroVector ) > 10.0 ) {
		Scalar sd = VectorDistance( posA, ZeroVector ) - 10.0;
		posA = VectorSub( posA, VectorMul( VectorNorm(
			VectorSub( posA, ZeroVector ) ), sd) );

	}
	if( VectorDistance( posB, ZeroVector ) > 10.0 ) {
		Scalar sd = VectorDistance( posB, ZeroVector ) - 10.0;
		posB = VectorSub( posB, VectorMul( VectorNorm(
			VectorSub( posB, ZeroVector ) ), sd) );
	}
// 	if( VectorDistance( posA, posB ) < 1.0 ) {
// 		posA = posAo;
// 		posB = posBo;
// 	}
	if( VectorDistance( posA, posB ) < 1.0 ) {
		float abd = (VectorDistance( posA, posB ) - 1.0) / 2.0;
		posA = VectorSub( posA, VectorMul( movDir, abd ) );
		posB = VectorSub( posB, VectorMul( movDir, -abd ) );
	}

	// Shoot
	Vector shotDir = VectorNorm( VectorSub( posA, posB ) );
	if( key[ '-' ] && time - shotAtime > 2 ) {
		shotAtime = time;
		shotA[shotAind] = posA;
		shotAd[shotAind] = VectorMul( shotDir, -0.1 );
		shotAa[shotAind] = 1;
		shotAind = (shotAind+1) % 200;
	}
	if( key[ 'f' ] && time - shotBtime > 2 ) {
		shotBtime = time;
		shotB[shotBind] = posB;
		shotBd[shotBind] = VectorMul( shotDir, 0.1 );
		shotBa[shotBind] = 1;
		shotBind = (shotBind+1) % 200;
	}

	// Move bullets
	for( int i = 0; i < 200; i++ ) {
		if( shotAa[i] ) {
			shotA[i] = VectorAdd( shotA[i], shotAd[i] );
			if( VectorDistance( ZeroVector, shotA[i] ) > 10.3 ) {
				shotAa[i] = 0;
			}
			if( VectorDistance( shotA[i], posB ) < 0.2 ) {
				shotAa[i] = 0;
				lifeB -= 0.01;
			}
		}
		if( shotBa[i] ) {
			shotB[i] = VectorAdd( shotB[i], shotBd[i] );
			if( VectorDistance( ZeroVector, shotB[i] ) > 10.3 ) {
				shotBa[i] = 0;
			}
			if( VectorDistance( shotB[i], posA ) < 0.2 ) {
				shotBa[i] = 0;
				lifeA -= 0.01;
			}
		}
	}
	return( 0 );
}

// Clean up what needs cleaning up.
void gameCleanup(void) {
	// Nothing
}
