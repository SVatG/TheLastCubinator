#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define PI 3.14159265
#define deg2rad(x) ((PI*(x))/(180.0))
#define sgn(x) (((x)>0)?(1):(-1))
#define min(a, b) (((a) > (b))?((b)):((a)))
#define max(a, b) (((a) > (b))?((a)):((b)))

#define bool uint8_t
#define true 1
#define false 0


// Global status things.
extern int mouseX;
extern int mouseY;
extern int key[256];
extern int specialKey[256];

// Global defines. Could also be extern vars eventually.
#define screenHeight 1280.0
#define screenWidth 720.0

#endif
