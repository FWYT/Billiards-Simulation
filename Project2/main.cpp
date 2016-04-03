#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include <gl\glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>

#include "ppm_canvas.h"
#include "vec3f.h"

#include <iostream>
#include <math.h>

GLuint woodTex;
canvas_t skin;
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


void drawTable()
{
	glPushMatrix();

	//draw the pool table
	glColor3f(0.3, 1.0, 0.5);
	glScalef(200, 5, 100);
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

	
	//left bottom 
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
	gluLookAt(0, 200, 0, 0, 0, 0, 0, 0, -1.0);
	glLineWidth(1.5);

	drawTable();

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

	//Light 0
	GLfloat lightColor0[] = { 1.0f, 1.0f, 0.7f, 1.0f };
	GLfloat lightPos0[] = { 0.0, 200.0, -100.0, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glEnable(GL_LIGHT0);

	glutDisplayFunc(callback_display);
	


	//hand control to glut
	glutMainLoop();

	return 0;
}