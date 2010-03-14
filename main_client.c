/**
* GLUT main app for things.
*/

#include "tools.h"
#include "game.h"
#include "game_client.h"

// Global status things.
int key[256];
int specialKey[256];

//from game.c
extern player_t p[2];

// Key press handler, normal keys.
void handleKeypress(unsigned char k, int x, int y)
{
	key[k] = 1;
	switch(k)
	{
		case 27: // Escape
			exit(0);
		case 'p': // Neat for debugging
			sleep(1);
			break;
		//player B movement
		case 'w':
			p[1].input.forwards = 1;
			break;
		case 's':
			p[1].input.backwards = 1;
			break;
		case 'a':
			p[1].input.left = 1;
			break;
		case 'd':
			p[1].input.right = 1;
			break;
		case 'f':
			p[1].input.shoot = 1;
			break;
		case 'g':
			p[1].input.dodgeDown = 1;
			break;
		case 'h':
			p[1].input.dodgeUp = 1;
			break;
		//player A shoot & dodge
		case '-':
			p[0].input.shoot = 1;
			break;
		case ',':
			p[0].input.dodgeDown = 1;
			break;
		case '.':
			p[0].input.dodgeUp = 1;
			break;
	}
}

void handleKeyUp(unsigned char k, int x, int y)
{
	key[k] = 0;
	switch(k)
	{
		//player B movement
		case 'w':
			p[1].input.forwards = 0;
			break;
		case 's':
			p[1].input.backwards = 0;
			break;
		case 'a':
			p[1].input.left = 0;
			break;
		case 'd':
			p[1].input.right = 0;
			break;
		case 'f':
			p[1].input.shoot = 0;
			break;
		case 'g':
			p[1].input.dodgeDown = 0;
			break;
		case 'h':
			p[1].input.dodgeUp = 0;
			break;
		//player A shoot & dodge
		case '-':
			p[0].input.shoot = 0;
			break;
		case ',':
			p[0].input.dodgeDown = 0;
			break;
		case '.':
			p[0].input.dodgeUp = 0;
			break;
	}
}

// Key press handler, special keys.
void handleSpecialKeypress(int k, int x, int y)
{
	specialKey[k] = 1;
	switch(k)
	{
		//player A movement
		case GLUT_KEY_UP:
			p[0].input.forwards = 1;
			break;
		case GLUT_KEY_DOWN:
			p[0].input.backwards = 1;
			break;
		case GLUT_KEY_LEFT:
			p[0].input.left = 1;
			break;
		case GLUT_KEY_RIGHT:
			p[0].input.right = 1;
			break;
	}
}

void handleSpecialUp(int k, int x, int y)
{
	specialKey[k] = 0;
	switch(k)
	{
		//player A movement
		case GLUT_KEY_UP:
			p[0].input.forwards = 0;
			break;
		case GLUT_KEY_DOWN:
			p[0].input.backwards = 0;
			break;
		case GLUT_KEY_LEFT:
			p[0].input.left = 0;
			break;
		case GLUT_KEY_RIGHT:
			p[0].input.right = 0;
			break;
	}
}

// Resize handler.
void handleResize(int w, int h) {
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, (float)w / (float)h, 1.0, 200.0 );
}

// Scene drawing
void drawScene() {
	gameDraw();
	glutSwapBuffers();
}

// Called every 10 milliseconds to update things on screen
void update(int value) {
	if( gameUpdate() ) {
		gameCleanup();
		graphicCleanup();
		exit(1);
	}
	glutPostRedisplay();
	glutTimerFunc( 10, update, 0 );
}

// Initialization functions.
void initOGL(int argc, char** argv) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	
	// Build modestring.
	char modeString[255];
	modeString[0] = 0;
	sprintf(modeString, "%dx%d:24", (int) screenHeight, (int) screenWidth);
	glutGameModeString( modeString );
	
	glutEnterGameMode();
	
	// Alternative: Windowed.
	// glutCreateWindow("Fages");
	
	glewInit();
}

void initFunctions() {
	memset( key, 0, 256 );
	memset( specialKey, 0, 256 );
	
	glutDisplayFunc( drawScene );
	glutKeyboardFunc( handleKeypress );
	glutKeyboardUpFunc( handleKeyUp );
	glutSpecialFunc( handleSpecialKeypress );
	glutSpecialUpFunc( handleSpecialUp );
	glutReshapeFunc( handleResize );
	glutTimerFunc( 10, update, 0 );
}

int main(int argc, char** argv)
{
	initOGL( argc, argv );
	initFunctions();
	
	graphicInit();
	gameInit();
	
	glutMainLoop();
	
	return 0;
}
