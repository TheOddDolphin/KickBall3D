//necessary includes
#include "stdafx.h"
#include "freeglut.h"
#include "gltools.h"	//only contains gltLoadTGA from the OpenGL superbible
#include <cmath>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include "math.h"
#include "windows.h"

using namespace std;

//necessary variable declarations
GLsizei Wheight = 720, Wwidth = 1280; //the dimensions of the viewing window

int score = 0; //score variable
bool kick = false; //indicates whether ball has been kicked or not
int tries = 10; //stores the number of remaining tries
int hoops = 8; //stores the number of hoops left to hit

GLfloat vectorZ = 0; GLfloat vectorX = 0; GLfloat vectorY = 0; //the current movement of the ball
																//along a specific axis

GLfloat s = 160; //speed of the ball, as per given equation
GLfloat t = 0; //time elapsed, as per given equation
GLfloat r; //(s*t), as per given equation

float theta; float phi; //angle of the ball movement, calculated using given equations

//declarations for gltLoadTGA function
char* imagePath; GLbyte *pBytes0; GLint iWidth, iHeight, iComponents; GLenum eFormat;

//positions of the horizontal and vertical sliders
double hozSliderX = 60; double vertSliderY = Wheight - 60.0;

//target colours
char* colRed = "textures/targetRed.tga";
char* colBlue = "textures/targetBlue.tga";
char* colDull = "textures/targetDull.tga";
char* colGreen = "textures/targetGreen.tga";

char* targetcol[8]; //array to store the colour of each target

GLfloat angle = 0.0; GLfloat skyBoxRot = 0.0; //variables to help rotate the football and
												//the skybox, respectively

//variable declarations for lighting - intensities
GLfloat  Light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat	 AmbLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
//variable declarations for lighting - positions
GLfloat	 AmbPos[] = { 0.0f, 20.0f, 0.0f, 1.0f };
GLfloat	 SpotPos[] = { 0.0f + vectorX, -5.0f + vectorY, 35.0f + vectorZ, 1.0f };
//variable declarations for lighting - specularity
GLfloat	 shine = 128;
GLfloat specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };

//variable to toggle following camera
bool followcam = false;

//sets the perspective view, along with the camera view
void setPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if (followcam == false) {
		gluPerspective(30.0, ((float)Wwidth / (float)Wheight), 45, 600.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		gluLookAt(0.00, 0.00, 100.00,//eye
			0.00, 0.00, 0.00,//centre
			0.00, 1.00, 0.00);//up
	}
	else {
		gluPerspective(30.0, ((float)Wwidth / (float)Wheight), 0.1, 300.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		gluLookAt(0 + vectorX, -5.0 + vectorY, 30 + vectorZ,//eye
			vectorX, vectorY, vectorZ,//centre
			0.00, 1.00, 0.00);//up
	}
}

//handles window reshaping and recalculates the aspect ratio
void reshape(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Calculate aspect ratio of the window
	fAspect = (GLfloat)w / (GLfloat)h;

	// Set the perspective coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//experiment with varying the values below for gluPerspective(...)
	// this sets a field of view of 30 degrees, near and far planes 1.0 and 600
	//the aspect ratio is set to that of the window to maintain the correct ratio
	gluPerspective(30.0f, fAspect, 1.0, 600.0);
	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//sets the Orthographic view, for the sliders and score indicator
void setOrthographicProjection() {
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save the previous matrix which contains the
	//set up for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, Wwidth, 0, Wheight);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -Wheight, 0);
	//set for drawing again
	glMatrixMode(GL_MODELVIEW);
}

//resets Perspective view after drawing in Orthographic
void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//draw, texture and update the position of the horizontal slider
void drawSliderX() {
	glEnable(GL_TEXTURE_2D);
	imagePath = "textures/fillBarHorizontal.tga";
	GLbyte *pBytes0;
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	//Set up texture parameters--------------------------------------
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 0.0);
	glVertex2d(30.0, 30.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2d(30.0, 50.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2d(120.0, 50.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2d(120.0, 30.0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//draw, texture and update the position of the vertical slider
void drawSliderY() {
	glEnable(GL_TEXTURE_2D);

	imagePath = "textures/fillBarVerticalR.tga";
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	//Set up texture parameters--------------------------------------
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_POLYGON);

	glTexCoord2f(1.0, 0.0);
	glVertex2d(Wwidth - 30.0, Wheight - 30.0);
	glTexCoord2f(0.0, 0.0);
	glVertex2d(Wwidth - 50.0, Wheight - 30.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2d(Wwidth - 50.0, Wheight - 120.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2d(Wwidth - 30.0, Wheight - 120.0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//draw the sliders and score indicator in the Orthographic view
void drawOrtho() {
	glColor3f(1.0, 1.0, 1.0);
	
	drawSliderX();
	drawSliderY();
	
	glColor3f(1.0, 1.0, 0);
	glRectd(hozSliderX, 30.0, hozSliderX + 30.0, 50.0);
	glRectd(Wwidth - 30.0, vertSliderY, Wwidth - 50.0, vertSliderY + 30.0);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2d(Wwidth - 120.0, 50.0);
	string scoreStr;
	if ((tries == 0) || (hoops = 0)) {
		scoreStr = "Game Over!";
	} else {
		scoreStr = "Score: " + to_string(score);
	}
	const unsigned char* scoref = reinterpret_cast
		<const unsigned char*>(scoreStr.c_str());
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, scoref);
	glPopMatrix();
}

//handle special keypresses
void processSpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT)
		hozSliderX -= 2;											//move hoz. slider
	if (key == GLUT_KEY_RIGHT)
		hozSliderX += 2;
	if (key == GLUT_KEY_UP)
		vertSliderY -= 2;											//move vert. slider
	if (key == GLUT_KEY_DOWN)
		vertSliderY += 2;
	if (vertSliderY < (Wheight - 120.0))							//prevent sliders from
		vertSliderY = (Wheight - 120.0);							//escaping their movement
	if (vertSliderY > (Wheight - 60.0))								//ranges
		vertSliderY = (Wheight - 60.0);
	if (hozSliderX > 90.0)
		hozSliderX = 90.0;
	if (hozSliderX < 30.0)
		hozSliderX = 30.0;
}

//handle normal keypresses
void processNormalKeys(unsigned char key, int x, int y) {
	switch (key) {
	case 27:	//exit on Esc
		exit(0);
		break;
	case 'z':	//fire the ball on 'z' or Space
	case ' ':	//also calculates theta and phi at the moment of firing
		theta = (M_PI / 2) - atan2(85.0f, ((hozSliderX / (30.0f / 38.0f)) - 76));
		phi = atan2(-((660 - vertSliderY) / (-60.0f / 29.0f)), 85.0f);
		kick = true;
		break;
	case 'f':	//set the camera to follow to the ball
		followcam = true;
		break;
	case 'r':	//restart the game once you run out of tries or targets
		if ((tries == 0) || (hoops == 0)) {
			tries = 10; score = 0;
		}
	}
}

//draw the football
void drawSphere() {

	//load the texture - once again, adapted from gltLoadTGA syntax, from the OpenGL SuperBible
	imagePath = "textures/FootballCompleteMap.tga";
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	//set up the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//create the ball
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	if (tries > 0) {
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricTexture(quadric, true);
		glTranslatef(0 + vectorX, -11.0 + vectorY, 30 + vectorZ);
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluSphere(quadric, 2.0f, 30, 17);
	}
	gluDeleteQuadric(quadric);
}

//draw the grass floor
void drawGrass() {
	imagePath = "textures/grass.tga";
	GLbyte *pBytes0;
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//generate repeating s and t texture coordinates for the grass.
	//Supplied values have been refined via trial and error to produce as seamless
	//an effect as possible.

	//the s parameter
	GLfloat myparamsS[] = { 0.1, 0.0, 0.1, 1.0 };
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, myparamsS);
	glEnable(GL_TEXTURE_GEN_S);

	//the t parameter
	GLfloat myparamsT[] = { 0.0, 0.1, 0.1, 1.0 };
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, myparamsT);
	glEnable(GL_TEXTURE_GEN_T);

	glBegin(GL_QUADS);

	glVertex3f(40.0f, -13.0f, 55.0f);
	glVertex3f(-40.0f, -13.0f, 55.0f);
	glVertex3f(-40.0f, -13.0f, -55.0f);
	glVertex3f(40.0f, -13.0f, -55.0f);

	glEnd();
	glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
}

//reusable function to draw a Wall
void drawWall(
	float x1, float y1, float z1, //accepts the coordinates of each vertex
	float x2, float y2, float z2, //as parameters
	float x3, float y3, float z3,
	float x4, float y4, float z4
) {
	imagePath = "textures/bricks_white.tga";
	GLbyte *pBytes0;
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLfloat myparamsS[] = { 0.04, 0.0, 0.04, 1.0 };
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, myparamsS);
	glEnable(GL_TEXTURE_GEN_S);

	GLfloat myparamsT[] = { 0.0, 0.04, 0.0, 1.0 };
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, myparamsT);
	glEnable(GL_TEXTURE_GEN_T);

	//set up lighting normal
	if ((z1 == z2) && (z2 == z3) && (z3 == z4) && (z4 == -55.0f)) {
		glNormal3f(0.0f, 0.0f, 1.0f);
	}
	if ((x1 == z2) && (x2 == x3) && (x3 == x4) && (x4 == -40.0f)) {
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	if ((x1 == z2) && (x2 == x3) && (x3 == x4) && (x4 == 40.0f)) {
		glNormal3f(0.0f, 1.0f, 0.0f);
	}

	glBegin(GL_QUADS);

	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glVertex3f(x4, y4, z4);

	glEnd();
	glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
}

//reusable function to draw Targets
void drawTarget(char* imagePath,			//texture of the target
	double posX, double posY, double posZ,	//coordinates of the target
	double inner, double outer,				//inner and outer radii
	double xRot, double yRot) {				//rotation of the target
	glPushMatrix();
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, true);
	glTranslatef(posX, posY, posZ);
	glRotatef(xRot, yRot, 0.0, 0.0);
	gluDisk(quadric, inner, outer, 30, 17);
	gluDeleteQuadric(quadric);
	glPopMatrix();

	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LIGHT0); glDisable(GL_LIGHT1); glDisable(GL_LIGHTING);
	glRasterPos3d(posX-1, posY-0.75, posZ);			//display the number of points
	string score;									//in the centre of the target
	if (imagePath == "textures/targetRed.tga") {
		score = "30";
	}
	if (imagePath == "textures/targetGreen.tga") {
		score = "10";
	}
	if (imagePath == "textures/targetBlue.tga") {
		score = "20";
	}
	const unsigned char* scoref = reinterpret_cast
		<const unsigned char*>(score.c_str());
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, scoref);
	glEnable(GL_LIGHT0); glEnable(GL_LIGHT1); glEnable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}

//reusable function to draw flower-type scenery
void drawFlowers(char* imagePath, int num, float x) {//accepts texture (orange or
														//yellow flowers), number of flowers
														//needed, and the x position of the
														//first flower
	GLfloat y1 = 0;  GLfloat y2 = 0; GLfloat z = 0;
	if (imagePath == "textures/orangeFlowerFinal5.tga") {
		y1 = -13.0;		//set height of the flowers
		y2 = -5.0;
		z = -54.8;		//and z-value of the flowers
	}
	if (imagePath == "textures/yellowFlowerFinal.tga") {
		y1 = -13.2;
		y2 = -7.0;
		z = -54.7;
	}
	glPushMatrix();
	glEnable(GL_BLEND);	//enable blending to make transparent texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLbyte *pBytes0;
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBegin(GL_QUADS);
	
	//set up vertexes and texture coords
	glTexCoord2f(1.0*num, 0.05);
	glVertex3f(x, y1, z);
	glTexCoord2f(0.0, 0.05);
	glVertex3f(x+(6.0*num), y1, z);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(x+(6.0*num), y2, z);
	glTexCoord2f(1.0*num, 1.0);
	glVertex3f(x, y2, z);

	glEnd();

	glDisable(GL_BLEND);
	glPopMatrix();
}

//single-use function to draw all the ferns
void drawFerns() {
	GLfloat y1 = -13.0;  GLfloat y2 = 10.0; GLfloat x = -39.9; int num = 6;
	GLfloat z = -30;
	char* imagePath = "textures/palmBranchA.tga";

	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLbyte *pBytes0;
	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//generate quads that are on the x-y plane but on different values of z.
	//generate for the left ferns
	for (int i = 0; i < num; i++) {
		glBegin(GL_QUADS);

		glTexCoord2f(0.0, 0.05);
		glVertex3f(x, y1, z + (12.0 * i));
		glTexCoord2f(1.0, 0.05);
		glVertex3f(x + 6, y1, z + (12.0 * i));
		glTexCoord2f(1.0, 1.0);
		glVertex3f(x + 6, y2, z + (12.0 * i));
		glTexCoord2f(0.0, 1.0);
		glVertex3f(x, y2, z + (12.0 * i));

		glEnd();
	}

	glPopMatrix();
	x = 39.9;	//switch to the right
	glPushMatrix();

	//generate for the right ferns
	for (int i = 0; i < num; i++) {
		glBegin(GL_QUADS);

		glTexCoord2f(0.0, 0.05);
		glVertex3f(x, y1, z + (12.0 * i));
		glTexCoord2f(1.0, 0.05);
		glVertex3f(x - 6, y1, z + (12.0 * i));
		glTexCoord2f(1.0, 1.0);
		glVertex3f(x - 6, y2, z + (12.0 * i));
		glTexCoord2f(0.0, 1.0);
		glVertex3f(x, y2, z + (12.0 * i));

		glEnd();
	}

	glDisable(GL_BLEND);
	glPopMatrix();
}

//single-use function to draw the skybox
void drawSkyBox() {
	imagePath = "textures/stormydays_large.tga"; //reduced resolution version of the original
												//4096 * 3072 to 1024 * 768, to reduce memory
												//strain

	pBytes0 = gltLoadTGA(imagePath, &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
	free(pBytes0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	//draw Skybox
	int zMult = 4;
	glFrontFace(GL_CW);
	glRotatef(skyBoxRot, 0.0f, 1.0f, 0.0f); //rotate the skybox
	glBegin(GL_QUADS);

	//use the texcoords from the cubemap to map the texture onto several different faces
	//of the same cube
	glTexCoord2f(0.5, (2.0 / 3.0));
	glVertex3f(40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.25, (2.0 / 3.0));
	glVertex3f(-40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.25, 1.0);
	glVertex3f(-40.0f * 2, 20.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(40.0f * 2, 20.0f * 2, 20.0f * zMult);

	glEnd();
	glBegin(GL_QUADS);

	glTexCoord2f(0.5, (2.0 / 3.0));
	glVertex3f(40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.25, (2.0 / 3.0));
	glVertex3f(-40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.25, (1.0/3.0));
	glVertex3f(-40.0f * 2, -13.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.5, (1.0/3.0));
	glVertex3f(40.0f * 2, -13.0f * 2, -55.0f * zMult);

	glEnd();
	glBegin(GL_QUADS);

	glTexCoord2f(0.5, (2.0 / 3.0));
	glVertex3f(40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.75, (2.0 / 3.0));
	glVertex3f(40.0f * 2, 20.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.75, (1.0 / 3.0));
	glVertex3f(40.0f * 2, -13.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.5, (1.0 / 3.0));
	glVertex3f(40.0f * 2, -13.0f * 2, -55.0f * zMult);

	glEnd();
	glBegin(GL_QUADS);

	glTexCoord2f(0.25, (2.0 / 3.0));
	glVertex3f(-40.0f * 2, 20.0f * 2, -55.0f * zMult);
	glTexCoord2f(0.0, (2.0 / 3.0));
	glVertex3f(-40.0f * 2, 20.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.0, (1.0 / 3.0));
	glVertex3f(-40.0f * 2, -13.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.25, (1.0 / 3.0));
	glVertex3f(-40.0f * 2, -13.0f * 2, -55.0f * zMult);

	glEnd();
	glBegin(GL_QUADS);

	glTexCoord2f(0.75, (2.0 / 3.0));
	glVertex3f(40.0f * 2, 20.0f * 2, 20.0f * zMult);
	glTexCoord2f(1.0, (2.0 / 3.0));
	glVertex3f(-40.0f * 2, 20.0f * 2, 20.0f * zMult);
	glTexCoord2f(1.0, (1.0 / 3.0));
	glVertex3f(-40.0f * 2, -13.0f * 2, 20.0f * zMult);
	glTexCoord2f(0.75, (1.0 / 3.0));
	glVertex3f(40.0f * 2, -13.0f * 2, 20.0f * zMult);

	glEnd();
	glFrontFace(GL_CCW);
}

//draw the Perspective view, calling all the above functions to set up the scene
void drawPersp() {
	glEnable(GL_TEXTURE_2D);		//enable textures

	glPushMatrix();
	drawSkyBox();					//draw the skybox
	glPopMatrix();
	glPushMatrix();
	drawGrass();					//draw the grass

	drawWall(						// Back face (z = -55.0f)
		40.0f, -13.0f, -55.0f,
		-40.0f, -13.0f, -55.0f,
		-40.0f, 20.0f, -55.0f,
		40.0f, 20.0f, -55.0f);
	drawWall(						// Left face (x = -40.0f)
		-40.0f, 20.0f, 20.0f,
		-40.0f, 20.0f, -55.0f,
		-40.0f, -13.0f, -55.0f,
		-40.0f, -13.0f, 20.0f);
	drawWall(						// Right face (x = 40.0f)
		40.0f, 20.0f, -55.0f,
		40.0f, 20.0f, 20.0f,
		40.0f, -13.0f, 20.0f,
		40.0f, -13.0f, -55.0f);

	glPopMatrix();

	//draw the Flowers
	drawFlowers("textures/orangeFlowerFinal5.tga", 4, -33.0f);
	drawFlowers("textures/yellowFlowerFinal.tga", 3, -30.0f);
	drawFlowers("textures/orangeFlowerFinal5.tga", 1, 6.0f);
	drawFlowers("textures/yellowFlowerFinal.tga", 2, 25.0f);

	drawFerns();					// draw all the ferns

	glPushMatrix();
	drawSphere();					// draw the ball
	glPopMatrix();

	//draw all the Targets
	drawTarget(colDull,
		0, -12.9, 30,
		1.2, 1.8,
		90.0, 1.0);

	drawTarget(targetcol[0],
		-37, -10, -54.9,
		2.1, 2.7,
		0.0, 1.0);

	drawTarget(targetcol[1],
		-30, 10.0, -54.9,
		4.1, 5.1,
		0.0, 1.0);

	drawTarget(targetcol[2],
		-10, 16, -54.9,
		2.1, 2.7,
		0.0, 1.0);

	drawTarget(targetcol[3],
		-2, -8, -54.9,
		3.1, 3.8,
		0.0, 1.0);
	
	drawTarget(targetcol[4],
		10, 13, -54.9,
		3.1, 3.8,
		0.0, 1.0);

	drawTarget(targetcol[5],
		20, -6.5, -54.9,
		4.1, 5.1,
		0.0, 1.0);

	drawTarget(targetcol[6],
		20, 10, -54.9,
		2.1, 2.7,
		0.0, 1.0);

	drawTarget(targetcol[7],
		35, 5, -54.9,
		2.1, 2.7,
		0.0, 1.0);

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity(); //need this
}

//called as soon as a collision is detected, checks the score and updates the variables.
void checkScore(GLfloat x, GLfloat y) {
	/*This function basically checks if the x and y values of the ball at the time of
	collision were within the x and y bounds of any of the eight targets. If it is, then
	the target in question turns grey, and the player gets all of the points allocated to
	that target.*/
	if ((x < -35) && (x > -39) && (y < -8) && (y > -12) && (targetcol[0] != colDull)) {
		score += 30;
		hoops -= 1;
		targetcol[0] = colDull;
	}
	if ((x < -26) && (x > -34) && (y < 14) && (y > 6) && (targetcol[1] != colDull)) {
		score += 10;
		hoops -= 1;
		targetcol[1] = colDull;
	}
	if ((x < -8) && (x > -12) && (y < 18) && (y > 14) && (targetcol[2] != colDull)) {
		score += 30;
		hoops -= 1;
		targetcol[2] = colDull;
	}
	if ((x < 1) && (x > -5) && (y < -5) && (y > -11) && (targetcol[3] != colDull)) {
		score += 20;
		hoops -= 1;
		targetcol[3] = colDull;
	}
	if ((x < 13) && (x > 7) && (y < 16) && (y > 10) && (targetcol[4] != colDull)) {
		score += 20;
		hoops -= 1;
		targetcol[4] = colDull;
	}
	if ((x < 24) && (x > 16) && (y < -2.5) && (y > -10.5) && (targetcol[5] != colDull)) {
		score += 10;
		hoops -= 1;
		targetcol[5] = colDull;
	}
	if ((x < 22) && (x > 18) && (y < 12) && (y > 8) && (targetcol[6] != colDull)) {
		score += 30;
		hoops -= 1;
		targetcol[6] = colDull;
	}
	if ((x < 37) && (x > 33) && (y < 7) && (y > 3) && (targetcol[7] != colDull)) {
		score += 30;
		hoops -= 1;
		targetcol[7] = colDull;
	}
}

//Timer function runs every 10ms
//calculates ball movement and rotation
//checks for collisions
//rotates the skybox
void TimerFunc(int value)
{
	if (kick) {		//if the ball has been kicked
		t += 0.01;	//time the motion
		r = s*t;	//calculate r for the given equation
		vectorX = r*sin(theta)*cos(phi); //calculate the X-position
										//from the given eqn. (Reversed because x is the up vector)
		vectorY = r*sin(phi)*cos(theta); //calculate the Y-position
										//from the given eqn.
		if (vectorY < 0.0f)
			vectorY = -vectorY;
		vectorZ = -r*cos(theta);//calculate the Z-position
								//from the given eqn. (Reversed because x is the up vector and not z)
		angle -= 20.0;		//increment the rotation of the ball
		if (vectorZ < -83) {
			checkScore(vectorX, vectorY-11.0); //check the score
			vectorZ = 0;					   //reset variables
			vectorX = 0;
			vectorY = 0;
			kick = false;
			t = 0;
			angle = 0.0;
			tries -= 1;						   //update the tries taken
			followcam = false;				   //reset the following camera
		}
	}
	skyBoxRot += 0.1;		//rotate the skyBox slowly
	if (skyBoxRot > 360) {	//restart the rotation if the box comes full circle.
		skyBoxRot = 0.0;
	}
	glutPostRedisplay();	//redisplay the screen
	glutTimerFunc(10, TimerFunc, 1);
}

//runs once, at program start
//enable lighting and color materials and set necessary parameters
void setUpTexturesAndLights() {
	glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);	//set shininess
	glMateriali(GL_FRONT, GL_SHININESS, shine);

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbLight);		//ambient light
	glLightfv(GL_LIGHT0, GL_POSITION, AmbPos);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_SPECULAR, Light);		//specular spotlight
	glLightfv(GL_LIGHT1, GL_POSITION, SpotPos);
	glEnable(GL_LIGHT1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//set clear color to black

	glFrontFace(GL_CCW);

	//populate the array of target colours with target colours
	targetcol[0] = colRed;
	targetcol[1] = colGreen;
	targetcol[2] = colRed;
	targetcol[3] = colBlue;
	targetcol[4] = colBlue;
	targetcol[5] = colGreen;
	targetcol[6] = colRed;
	targetcol[7] = colRed;
}

//repositions the lights in real time as necessary, and autogenerates normals on the walls
void positionLights() {
	glLightfv(GL_LIGHT0, GL_POSITION, AmbPos);
	glLightfv(GL_LIGHT1, GL_POSITION, SpotPos);
	glEnable(GL_NORMALIZE);
}

//main display cycle, using all of the top-level functions declared before
void display()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	positionLights();
	setPerspectiveProjection();
	glEnable(GL_DEPTH_TEST);
	drawPersp();
	glDisable(GL_DEPTH_TEST);
	setOrthographicProjection();
	glDisable(GL_LIGHT0); glDisable(GL_LIGHT1); glDisable(GL_LIGHTING);
	drawOrtho();
	glEnable(GL_LIGHT0); glEnable(GL_LIGHT1); glEnable(GL_LIGHTING);
	glPopMatrix();
	resetPerspectiveProjection();
	glutSwapBuffers();
}

//initialise GLUT, set up a window and memory buffers, and register necessary functions
int main(int argc, char* argv[]) {
	//initialise GLUT
	glutInit(&argc, argv);
	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// Set the window size
	glutInitWindowSize(Wwidth, Wheight);
	// Create the window with the title "KickBall by 2015034"
	glutCreateWindow("KickBall by 2015034");
	//bind functions to respond as necessary
	glutSpecialFunc(processSpecialKeys);	//bind special keypresses
	glutKeyboardFunc(processNormalKeys);	//bind normal keypresses
	glutReshapeFunc(reshape);				//bind window reshape handling
	glutDisplayFunc(display);				//bind main display cycle
	glutTimerFunc(10, TimerFunc, 1);		//bind timer function
	setUpTexturesAndLights();				//set up lights and textures
	glutMainLoop();							//start the main cycle
	return 0;
}