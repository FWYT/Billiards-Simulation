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

using namespace std;

GLUquadricObj *sphere = NULL;
GLuint woodTex;
GLuint ballTex;
canvas_t skin;
canvas_t skin2;
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
Vec3f ballpos[16];
GLint mode;
int rightclick = 0;
GLint pointernum, pointerx, pointery;



void drawBall()
{
	//cue ball
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2]);
	gluSphere(sphere, 1.875, 10, 10); //diameter = 2.25in -> 0.1875ft/2 ->0.09375*20 =>1.875
	glPopMatrix();

	for (int i = 0; i < 15; i++)
	{
		string file = "Ball" + to_string(i + 1) + ".ppm";
		char tmp[11];
		strcpy(tmp, file.c_str());
		ppmLoadCanvas(tmp, &skin2);
		glGenTextures(1, &ballTex);
		glBindTexture(GL_TEXTURE_2D, ballTex);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, skin2.width, skin2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin2.pixels);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ballTex);
		glTranslatef(ballpos[i+1].v[0], ballpos[i+1].v[1], ballpos[i+1].v[2]);
		glRotatef(90, 0.0, 0.0, 1.0);
		gluSphere(sphere, 1.875, 10, 10);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
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

	/*if (!viewpt) {
		gluLookAt(defx, defy, defz, ballpos[0].v[0], ballpos[0].v[1], ballpos[0].v[2], upX, upY, upZ);
	}//deflookatx,deflookaty, deflookatz,
	else { gluLookAt(0, 200, 0, 0, 0, 0, 0, 0, -1.0); }
	//	  glPopMatrix();*/
	gluLookAt(0, 250, 0, 0, 0, 0, 0.0, 0, -1.0);
	glLineWidth(1.5);

	drawTable();
	drawBall();

	glPopMatrix();
	glutSwapBuffers();
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


	/*ppmLoadCanvas("Ball1.ppm", &skin2);
	glGenTextures(1, &ballTex);
	glBindTexture(GL_TEXTURE_2D, ballTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, skin2.width, skin2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin2.pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);*/

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

	for (int i = 1; i < 16; i++)
	{
		ballpos[i].v[1] = 4.6;
		int neg = (rand() % 2) + 1;
		ballpos[i].v[2] = (rand()%45 *(pow(-1, neg)))+1;
		ballpos[i].v[0] = rand() % 95 * (pow(-1, neg));

		/*cout << "neg: " << neg << endl;
		cout << "2: " << ballpos[i].v[2] << endl;
		cout << "0: " << ballpos[i].v[0] << endl;*/
	}



	glClearColor(0, 0, 0, 0);

	glutDisplayFunc(callback_display);
	


	//hand control to glut
	glutMainLoop();

	return 0;
}