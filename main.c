/**
 * GLUT main app for things.
 */

#include "tools.h"
#include "game.h"

// Global status things.
int key[256];
int specialKey[256];

// Key press handler, normal keys.
void handleKeypress( unsigned char k, int x, int y ) {
	key[ k ] = 1;
	
	// Some special cases.
	switch( k ) {
		case 27: // Escape
			exit( 0 );
		case 'p': // Neat for debugging
			sleep( 1 );
		break;
	}
}
void handleKeyUp( unsigned char k, int x, int y ) {
	key[ k ] = 0;
}

// Key press handler, special keys.
void handleSpecialKeypress( int k, int x, int y ) {
	specialKey[ k ] = 1;
}
void handleSpecialUp( int k, int x, int y ) {
	specialKey[ k ] = 0;
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
 		exit( 1 );
 	}

	glutPostRedisplay();
	glutTimerFunc( 10, update, 0 );
}

// Initialization functions.
void initOGL(int argc, char** argv) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Build modestring.
	char tmpString[255];
	char modeString[255];
	modeString[0] = 0;
	itoa( (int)screenHeight, tmpString, 10 );
	strcat( modeString, tmpString );
	strcat( modeString, "x" );
	itoa( (int)screenWidth, tmpString, 10 );
	strcat( modeString, tmpString );
	strcat( modeString, ":24" );
	glutGameModeString( modeString );

	// glutEnterGameMode();

	// Alternative: Windowed.
	glutCreateWindow("Fages");

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

int main(int argc, char** argv) {

	initOGL( argc, argv );
	initFunctions();

 	gameInit();

	glutMainLoop();

	return 0;
}
