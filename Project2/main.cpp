#ifdef _WIN32 
#define _CRT_SECURE_NO_DEPRECATE 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include <GL/glut.h>
#include <gl\glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>

#include "ppm_canvas.h"
#include "vec3f.h"

#include <iostream>
#include <math.h>

#include "Constants.h"
#include "cue-ball.h"
#include "sliding.h"

using namespace std;

GLUquadricObj *sphere = NULL;
GLuint woodTex;
GLuint ballTex[15];
GLint viewport[4];
bool hits = false;
bool startmove = false;
canvas_t skin;
canvas_t skin2[15];
float zoomfactor = 1.0;
float width = 1000;
float height = 1000;
GLfloat upX = 0.0;
GLfloat upY = 0.5;
GLfloat upZ = -0.5;
float defx = 0;
float defy = 80.0;
float defz = 160.0;
bool viewpt = 0; // 0 = first person 1 = top view
GLint mode;
int rightclick = 0;
int leftclick = 0;
bool pocketed[16];
int playerturn = 0;
GLint pointernum, pointerx, pointery;
float temppointerx = 0;
float temppointery = 0;
GLdouble realx = 0.0;
GLdouble realy = 0.0;
GLdouble realz = 0.0;
GLdouble modelview[16];
GLdouble projection[16];
GLfloat z_cursor;
Vec3f ballvec[16];
int playerballid[2];
bool collid[16][16];

//model data
Vec3f ballpos[16]; //ball position
Vec3f ballVel[16]; //ball velocity
Vec3f cueVector;
Vec3f cuePos;
//double a = cueVector[0]; //horizontal displacement from center
//double b = cueVector[2]; //vertical displacement from center
//double ballRadius = 1.875;
//double c = sqrt(pow(ballRadius, 2) - pow(a, 2) - pow(b, 2));
//double ballMass = 0.165; //kg
//double cueMass = 0.539; //kg
//double cueV0 = 2; //cue initial velocity m/s
//double theta = 0; //cue angle
//double t = 0; //time
//double g = 0.5; //gravity

//double friction = 0.5;

//state var for sliding
/*Vec3f r_s[16]; //pos
Vec3f v_s[16]; //vel 
Vec3f a_s[16]; //angular
Vec3f vel0;
Vec3f ang0 = Vec3f(0.0, 0.0, 0.0);
Vec3f u0 = vel0 + (ballRadius*Vec3f(0, 0, 1.0)).cross(ang0);*/

void drawBall();


Vec3f proj(Vec3f v1, Vec3f v2) {
	//projection v1 to v2:
	// a1 = (a dot b)/(b dot b)   * b 
	return (((v1.dot(v2)) / (v2.dot(v2)))* v2);
}

void animate(int value)
{
	if (startmove)
	{		
		//set up initial values from cue ball impact
		double F = calcForce();
		cout << "F " << F << endl;
		Vec3f initV = linearVelocity(F);
		Vec3f initA = angularVelocity(F);

		//sliding movement
		double t = slideDuration();

		Vec3f initU = relativeVel(0);

		Vec3f curPos = ballpos[0];

		Vec3f newPos = position(initU, t, curPos);
		ballpos[0] = newPos;


		startmove = false;
	}
}


void drawBall()
{
	//cue ball
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2]);
	gluSphere(sphere, 1.875, 10, 10); //diameter = 2.25in -> 0.1875ft/2 ->0.09375*20 =>1.875
	glPopMatrix();

	

	/*for (int i = 0; i < 15; i++)
	{
		string file = "Ball" + to_string(i + 1) + ".ppm";
		char tmp[11];
		strcpy(tmp, file.c_str());
		ppmLoadCanvas(tmp, &skin2[i]);
		glGenTextures(1, &ballTex[i]);
		glBindTexture(GL_TEXTURE_2D, ballTex[i]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, skin2[i].width, skin2[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin2[i].pixels);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ballTex[i]);
		glTranslatef(ballpos[i+1].v[0], ballpos[i+1].v[1], ballpos[i+1].v[2]);
		glRotatef(90, 0.0, 0.0, 1.0);
		gluSphere(sphere, 1.875, 10, 10);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}*/
}


void drawTable()
{
	glPushMatrix();

	//draw the pool table
	glColor3f(0.3, 0.7, 0.5);
	glScalef(200, 5, 100); //10ft*20, 5ft*20 => scale = 20
	glutSolidCube(1.0);
	glPopMatrix();

	//head string
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(50, 2.6, -50);
	glVertex3f(50, 2.6, 50);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTex);

	
	//top right
	glBegin(GL_QUADS);
	
	//1 
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, 10.0, -50.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, 10.0, -50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(5.0, 10.0, -60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, 10.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);

	//2
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(5.0, 10.0, -60.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(95.0, 10.0, -60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, -5.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, -5.0, -60.0);
	glTexCoord3f(0.0, 1.0, 0.0);

	//3
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(95.0, -5.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, -5.0, -60.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, -5.0, -50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(95.0, -5.0, -50.0);
	glTexCoord3f(1.0, 0.0, 0.0);

	//4
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, -5.0, -50.0);
	glVertex3f(95.0, -5.0, -50.0);
	glVertex3f(95.0, 10.0, -50.0);
	glVertex3f(5.0, 10.0, -50.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTex);

	//left top
	//1
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, 10.0, -50.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, 10.0, -50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-5.0, 10.0, -60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, 10.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);

	//2
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-5.0, 10.0, -60.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-95.0, 10.0, -60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, -5.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-5.0, -5.0, -60.0);
	glTexCoord3f(0.0, 1.0, 0.0);

	//3
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-95.0, -5.0, -60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-5.0, -5.0, -60.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, -5.0, -50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-95.0, -5.0, -50.0);
	glTexCoord3f(1.0, 0.0, 0.0);

	//4
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, -5.0, -50.0);
	glVertex3f(-95.0, -5.0, -50.0);
	glVertex3f(-95.0, 10.0, -50.0);
	glVertex3f(-5.0, 10.0, -50.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTex);

	//right bottom
	//1
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, 10.0, 50.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, 10.0, 50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(5.0, 10.0, 60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, 10.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);

	//2
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, 10.0, 60.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(95.0, 10.0, 60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(95.0, -5.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, -5.0, 60.0);
	glTexCoord3f(0.0, 1.0, 0.0);

	//3
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(95.0, -5.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, -5.0, 60.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(5.0, -5.0, 50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(95.0, -5.0, 50.0);
	glTexCoord3f(1.0, 0.0, 0.0);

	//4
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(5.0, -5.0, 50.0);
	glVertex3f(5.0, 10.0, 50.0);
	glVertex3f(95.0, 10.0, 50.0);
	glVertex3f(95.0, -5.0, 50.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, woodTex);

	//right top
	//1
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, 10.0, 50.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, 10.0, 50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-5.0, 10.0, 60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, 10.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);

	//2
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, 10.0, 60.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-95.0, 10.0, 60.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-95.0, -5.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-5.0, -5.0, 60.0);
	glTexCoord3f(0.0, 1.0, 0.0);

	//3
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-95.0, -5.0, 60.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-5.0, -5.0, 60.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-5.0, -5.0, 50.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-95.0, -5.0, 50.0);
	glTexCoord3f(1.0, 0.0, 0.0);

	//4
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-5.0, -5.0, 50.0);
	glVertex3f(-5.0, 10.0, 50.0);
	glVertex3f(-95.0, 10.0, 50.0);
	glVertex3f(-95.0, -5.0, 50.0);
	glColor3f(1.0, 1.0, 1.0);

	// bottom
	//1
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(-1.0, 0.0, .0);
	glVertex3f(100.0, 10.0, -45.0);
	glVertex3f(100.0, 10.0, 45.0);
	glVertex3f(100.0, -5.0, 45.0);
	glVertex3f(100.0, -5.0, -45.0);
	glColor3f(1.0, 1.0, 1.0);

	//2
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(100.0, 10.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(110.0, 10.0, -45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(110.0, 10.0, 45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(100.0, 10.0, 45.0);
	glTexCoord3f(1.0, 1.0, 0.0);

	//3
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(110.0, 10.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(110.0, -5.0, -45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(110.0, -5.0, 45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(110.0, 10.0, 45.0);
	glTexCoord3f(1.0, 1.0, 0.0);

	//4
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(100.0, -5.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(110.0, -5.0, -45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(110.0, -5.0, 45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(100.0, -5.0, 45.0);
	glTexCoord3f(1.0, 1.0, 0.0);

	//top
	//1
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-100.0, 10.0, -45.0);
	glVertex3f(-100.0, -5.0, -45.0);
	glVertex3f(-100.0, -5.0, 45.0);
	glVertex3f(-100.0, 10.0, 45.0);
	glColor3f(1.0, 1.0, 1.0);

	//2
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-100.0, 10.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-110.0, 10.0, -45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-110.0, 10.0, 45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-100.0, 10.0, 45.0);
	glTexCoord3f(1.0, 1.0, 0.0);

	//3
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-110.0, 10.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-110.0, -5.0, -45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(-110.0, -5.0, -45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-110.0, 10.0, 45.0);
	glTexCoord3f(1.0, 1.0, 0.0);

	//4
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-100.0, -5.0, -45.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(-110.0, -5.0, -45.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(-110.0, -5.0, 45.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(-100.0, -5.0, 45.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	//draw the pockets
	glColor3f(1.0, 1.0, 1.0);

	//right top
	GLUquadricObj * quadObj = gluNewQuadric();
	glPushMatrix();
	glTranslatef(103, -8, -52.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7, 0.7, 0.7);
	glTranslatef(104, -7, -53.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();

	//right bottom
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(104, -8, 51.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7, 0.7, 0.7);
	glTranslatef(106, -7, 53.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();

	//left top
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(-103, -8, -52.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7, 0.7, 0.7);
	glTranslatef(-104, -7, -53.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();

	//left bottom
	glPushMatrix();
	glTranslatef(-103, -8, 51.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);
	glTranslatef(-105, -7, 53.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();

	//middle top
	glPushMatrix();
	glTranslatef(0.0, -8, -55.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7, 0.7, 0.7);
	glTranslatef(0, -7, -56.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();

	//middle bottom
	glPushMatrix();
	glTranslatef(0.0, -8, 55.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadObj, 8, 8, 10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9, 0.9, 0.9);
	glTranslatef(0, -7, 57.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluDisk(quadObj, 2, 7, 20, 20);
	glPopMatrix();
}

void callback_display()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(60 * zoomfactor, (float)width / height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//  clear stuff on the canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	gluLookAt(0, 250, 0, 0, 0, 0, 0.0, 0, -1.0);
	glLineWidth(1.5);

	if (hits) //left clicking
	{
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2]); //set one point on the cueball

		glVertex3f(realx, 4.6, realy); //point of mouse

		//save cue to ball vector
		cueVector = Vec3f(ballpos[0].v[0] - realx, 0.0, ballpos[0].v[2] - realy);
		cuePos = Vec3f(realx, 4.6, realy);
	

		Vec3f oppopt(ballpos[0].v[0] - realx, 0.0, ballpos[0].v[2] - realy); //distance from cueball to mouse
		Vec3f originpt(ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2]);

		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2]);

		//dotted line 
		int fk = 0;
		bool shotlinestop = false;
		oppopt = oppopt / (oppopt.magnitude()) * 3;

		

		while (!shotlinestop)
		{
			glVertex3f(originpt.v[0]+fk*oppopt.v[0], 4.6, originpt.v[2] + fk*oppopt.v[2]);
			
			for (int i = 1; i < 16; i++) //test line collision with balls
			{
				if (pow((originpt.v[0] + fk*oppopt.v[0] - ballpos[i].v[0]), 2) +
					pow((originpt.v[2] + fk*oppopt.v[2] - ballpos[i].v[2]), 2) < 4)
				{
					shotlinestop = true;
				}
			}

			//test collision with rails
			if (originpt.v[2] + fk*oppopt.v[2] > 50 || originpt.v[2] + fk*oppopt.v[2] < -50 || 
				originpt.v[0] + fk*oppopt.v[0] < -100 || originpt.v[0] + fk*oppopt.v[0] > 100) {
				shotlinestop = true;
			}
			fk++;
			
		}
		glEnd();

	}

	drawTable();
	drawBall();

	glPopMatrix();
	glutSwapBuffers();
}

void callback_mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		leftclick = 1;
		temppointerx = x;
		temppointery = y;
		viewpt = 1;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftclick = 0;
		if (hits)
		{
			hits = false;
			startmove = true;
			
		}
	}

}

void callback_motion(int x, int y) {
	if (leftclick)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		glReadPixels((float)x, (float)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z_cursor);
		gluUnProject((float)x, (float)y, z_cursor, modelview, projection, viewport, &realx, &realy, &realz);

		hits = true;
	}
}

void cb_idle() {
	if (startmove) 
	{ 
		glutTimerFunc(100, animate, 0); 
	}
	glutPostRedisplay();

}

int main(int argc, char* argv[])
{
	//open a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Billiards  Simulation");

	ppmLoadCanvas("wood.ppm", &skin);
	glGenTextures(1, &woodTex);
	glBindTexture(GL_TEXTURE_2D, woodTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, skin.width, skin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin.pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLoadIdentity();
	//second part
	mode = GL_RENDER;
	glGetIntegerv(GL_RENDER_MODE, &mode);
	glMatrixMode(GL_MODELVIEW);

	GLfloat mat_specular[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 10.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	//Light 0
	GLfloat lightColor0[] = { 1.0f, 1.0f, 0.7f, 1.0f };
	GLfloat lightPos0[] = { 0.0, 200.0, -100.0, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glEnable(GL_LIGHT0);

	//sphere for balls
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	//   gluQuadricOrientation( sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_TRUE);

	/*int accu = 1;
	for (int temp2 = 1; temp2 <= 5; temp2++) {
		for (int intemp = 0;intemp <temp2;intemp++) {
			ballpos[accu].v[2] = -50.0 - (3.46411*(temp2 - 1));
			iniballpos[accu].v[2] = -50.0 - (3.46411*(temp2 - 1));
			accu++;
		}
	}*/

	ballpos[0].v[1] = 4.6;
	ballpos[0].v[2] = 0.0;
	ballpos[0].v[0] = 0.0;

	//r_s[0] = ballpos[0];

	for (int i = 1; i < 16; i++)
	{
		ballpos[i].v[1] = 4.6;
		int neg = (rand() % 2) + 1;
		ballpos[i].v[2] = (rand()%45 *(pow(-1, neg)))+1;
		ballpos[i].v[0] = rand() % 95 * (pow(-1, neg))+15;

	}



	glClearColor(0, 0, 0, 0);

	glutDisplayFunc(callback_display);
	glutMouseFunc(callback_mouse);
	glutMotionFunc(callback_motion);
	glutIdleFunc(cb_idle);
	


	//hand control to glut
	glutMainLoop();

	return 0;
}