#ifdef _WIN32 
#define _CRT_SECURE_NO_DEPRECATE 
#endif

//#include <vld.h>

#include <stdlib.h>


#include <stdio.h>

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
#include "rolling.h"

using namespace std;

int id;
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
//bool pocketed[16];
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
//Vec3f ballvec[16];
//int playerballid[2];
bool collid[16][16] = { {0} };

//model data
Vec3f ballpos[16]; //ball position
Vec3f ballVel[16]; //ball velocity
Vec3f angVel[16]; //angular velocity
Vec3f cueVector;
Vec3f cuePos;
double V0;

void drawBall();

//Vec3f ballVel[16];

double timer = 0.0;
double ts = 0;
double tr = 0;
//bool tsDone = false;
//bool trDone = false;
//Vec3f timePos[20];
Vec3f curPos;
double delta = 0.2;
bool first = true;
//double diffZ;
//double diffX;
bool cueDone = false;
//Vec3f prevBallVel;
int valid[16] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

int collided[16][2] = { {0.0,0.0} }; //rail collisions

/*
Vec3f proj(Vec3f v1, Vec3f v2) {
	//projection v1 to v2:
	// a1 = (a dot b)/(b dot b)   * b 
	return (((v1.dot(v2)) / (v2.dot(v2)))* v2);
}*/

bool noneMove()
{
	for (int i = 0; i < 16; i++)
	{
		if (ballVel[i][0] != 0.0 || ballVel[i][1] != 0.0 || ballVel[i][2] != 0.0)
		{
			return false;
		}
	}
	return true;
}

void animate(int value)
{
	if (startmove)
	{	
		//cout << "cueVector Mag/30 " << cueVector.magnitude() / 20.0 << endl;
		V0 = ((cueVector.magnitude() / 20.0) < 5.0) ? cueVector.magnitude() / 20.0 : 5.0;
		//cout << "V0 " << V0 << endl;
		//V0 = 3.0;
		//cout << "start" << endl;
		if (ts == 0 && tr == 0 && !cueDone)
		{
			ts = slideDuration(V0);
			tr = slideDuration(V0);
			curPos = ballpos[0];
			
		}

		

		if (timer <= ts)
		{
			//prevBallVel = ballVel[0];
			ballVel[0] = flipYZ(linearVelocityS(timer, V0));
			//angVel[0] = flipYZ(angularVelocityS(timer, V0));
			//cout << "angVel " << angVel[0] << endl;
			
		}

		if (timer > ts && timer <= ts+tr)
		{
			//prevBallVel = ballVel[0];
			ballVel[0] = flipYZ(linearVelocityR(timer, V0));
			//angVel[0] = flipYZ(angularVelocityS(timer));
		}
		
		
		//rotate velocity to the cue vector direction
		ballVel[0] += angVel[0];
		//cout << "with Ang " << ballVel[0] << endl;
		ballVel[0] = rotateToCue(ballVel[0], cueVector, ballVel[0]);
		//cout << "ballVel " << ballVel[0] << endl;
		//cout << "ball pos " << ballpos[0] << endl;

		


		
		//ball-ball collisions
		//test ball collisions
		double dist;
		for (int ball1 = 0; ball1 < 16; ball1++)
		{
			for (int ball2 = 0; ball2 < 3; ball2++)
			{
				if (ball1 != ball2)
				{
					dist = sqrt(pow(ballpos[ball1][0] - ballpos[ball2][0], 2)
							  + pow(ballpos[ball1][2] - ballpos[ball2][2], 2));

					if (dist < ballRadius * 2)
					{
						if (!collid[ball2][ball1]) //prevent double
						{
							collid[ball1][ball2] = 1;
						}
					}
				}
			}
		}
		
		//do collisions
		for (int i = 0; i < 16; i++)
		{
			if (valid[i])
			{
				for (int j = 0; j < 3; j++)
				{
					if (collid[i][j])
					{
						Vec3f col = ballpos[i] - ballpos[j];
						double len = col.magnitude();

						if (len == 0.0)
						{
							col = Vec3f(1.0, 0.0, 0.0);
							len = 1.0;
						}

						col = col / len;
						double aci = ballVel[i].dot(col);
						double bci = ballVel[j].dot(col);

						double acf = bci;
						double bcf = aci;

						ballVel[i] += (acf - aci)*col;
						ballVel[j] += (bcf - bci)*col;
					}
				}
			}
		}
		
		//pocket collisions
		for (int i = 0; i < 16; i++)
		{
			if (valid[i])
			{
				//right top pocket
				if (ballpos[i][0] > 96 && ballpos[i][2] < -46)
				{
					cout << i << "pocketed right top" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand()%3+97;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + -53;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}

				//right bottom pocket
				if (ballpos[i][0] > 96 && ballpos[i][2] > 46)
				{
					cout << i << "pocketed right bottom" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand() % 3 + 98;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + 49;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}

				//left top pocket
				if (ballpos[i][0] < -96 && ballpos[i][2] < -46)
				{
					cout << i << "pocketed left top" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand() % 3 + -102;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + -52;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}

				//left bottom pocket
				if (ballpos[i][0] < -96 && ballpos[i][2] > 46)
				{
					cout << i << "pocketed left bottom" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand() % 3 + -102;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + 49;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}

				//top side pocket
				if (ballpos[i][0] < 3 && ballpos[i][0] > -3 && ballpos[i][2] < -48)
				{
					cout << i << "pocketed top side" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand() % 6 + -3;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + -53;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}

				//bottom side pocket
				if (ballpos[i][0] < 3 && ballpos[i][0] > -3 && ballpos[i][2] > 48)
				{
					cout << i << "pocketed bottom side" << endl;
					if (i != 0)
					{
						ballpos[i][0] = rand() % 6 + -3;
						ballpos[i][1] = 4.6;
						ballpos[i][2] = rand() % 3 + 50;
						ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						valid[i] = 0;
					}
				}
			}

		}
		
		
		//rail collisions
		for (int i = 0; i < 16; i++)
		{
			if (valid[i])
			{
				//right wall
				if (ballpos[i][0] > 98)
				{
					//if (i==0) cout << "collidedX1" << endl;
					collided[i][0]++;
					//countX++;
					//cout << "collidedX " << collided[0][0] << endl;
				}
				//left rail
				if (ballpos[i][0] < -98)
				{
					//if (i == 0) cout << "collidedX2" << endl;
					collided[i][0]++;
					//countX++;
					//cout << "collidedX " << collided[0][0] << endl;
				}
				//top rail
				if (ballpos[i][2] < -48)
				{
					//if (i == 0) cout << "collidedZ1" << endl;
					collided[i][1]++;
					//countZ++;
					//cout << "collidedZ " << collided[0][1] << endl;

				}
				//bottom rail
				if (ballpos[i][2] > 48)
				{
					//if (i == 0) cout << "collidedZ2" << endl;
					collided[i][1]++;
					//countZ++;
					//cout << "collidedZ " << collided[0][1] << endl;
				}
			}
		}

		
		for (int i = 0; i < 16; i++)
		{
			if (valid[i])
			{
				ballVel[i][0] *= pow(-1, collided[i][0]);
				//ballVel[i][0] *= pow(-1, countX);
				ballVel[i][2] *= pow(-1, collided[i][1]);
				//ballVel[i][2] *= pow(-1, countZ);
			}
		}
		//cout << ballVel[0][2] << endl;

		
		
		
		//update position
		for (int i = 0; i < 16; i++)
		{
			//cout << i << ": " << valid[i] << endl;
			if (valid[i])
			{
				if (i == 0)
				{
					ballpos[i] = ballpos[i] + ballVel[i] * delta;
					ballpos[i][1] = 4.6;
				}
				else
				{
					ballpos[i] = ballpos[i] + ballVel[i] * delta;
					ballpos[i][1] = 4.6;

					//ball is moving, subtract friction every time step
					if (ballVel[i][0] != 0.0 || ballVel[i][1] != 0.0 || ballVel[i][2] != 0.0)
					{
						Vec3f old = ballVel[i];
						ballVel[i] -= delta*friction*g*ballVel[i].normalize();
						if ((old[0]<0.0 && ballVel[i][0]>0.0) || (old[0]>0.0 && ballVel[i][0]<0.0) ||
							(old[2]<0.0 && ballVel[i][2]>0.0) || (old[2]>0.0 && ballVel[i][2] < 0.0))
						{
							ballVel[i] = Vec3f(0.0, 0.0, 0.0);
						}
					}
				}
			}
		}

		//cout << "ballVel 1 " << ballVel[1] << endl;
		
		timer += delta;
		//cout << "\n" << endl;

		//set ball-ball collisions to zero
		memset(collid, 0, sizeof(collid));
		
		for (int i = 1; i <16; i++)
		{
			collided[i][0] = 0;
			collided[i][1] = 0;
		}
		
		//cueball done moving
		if (timer > ts + tr)
		{
			//cout << " cueball done " << endl;
			//set rail collision and ball velocities to zero
			//memset(collided, 0, sizeof(collided));
			//memset(ballVel, 0, sizeof(ballVel));
			collided[0][0] = 0;
			collided[0][1] = 0;
			ballVel[0] = Vec3f(0.0, 0.0, 0.0);
			//timer = 0;
			ts = 0;
			tr = 0;
			cueDone = true;
			//startmove = false;
					
		}

		//cout << "none move " << noneMove() << endl;
		//all balls done moving
		if (noneMove())
		{
			//cout << "no move" << endl;
			
			startmove = false;
			timer = 0;
			cueDone = false;
			//cout << "\n" << endl;
			//countZ = 0;
			//countX = 0;
		}
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

	

	//for (int i = 0; i < 15; i++)
	for (int i = 0; i < 15; i++)
	{
		if (first)
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

			
/*
			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, ballTex[i]);
			glTranslatef(ballpos[i+1].v[0], ballpos[i+1].v[1], ballpos[i+1].v[2]);
			glRotatef(90, 0.0, 0.0, 1.0);
			gluSphere(sphere, 1.875, 10, 10);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		*/
		}
		/*glColor3f(1.0, 0.0, 1.0);
		glPushMatrix();
		glTranslatef(ballpos[i+1].v[0], ballpos[i+1].v[1], ballpos[i+1].v[2]);
		gluSphere(sphere, 1.875, 10, 10); //diameter = 2.25in -> 0.1875ft/2 ->0.09375*20 =>1.875
		glPopMatrix();*/
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ballTex[i]);
		glTranslatef(ballpos[i + 1].v[0], ballpos[i + 1].v[1], ballpos[i + 1].v[2]);
		glRotatef(90, 0.0, 0.0, 1.0);
		gluSphere(sphere, 1.875, 10, 10);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	if (first)
	{
		first = false;
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
	if (leftclick && !startmove)
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

void callback_keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 27: // Escape key
		glutDestroyWindow(id);

		exit(0);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	//open a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	id = glutCreateWindow("Billiards  Simulation");

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

	/*ballpos[1][0] = 10.0;
	ballpos[1][1] = 4.6;
	ballpos[1][2] = 0.0;

	ballpos[2][0] = 30.0;
	ballpos[2][1] = 4.6;
	ballpos[2][2] = 0.0;*/

	//r_s[0] = ballpos[0];

	for (int i = 1; i < 16; i++)
	{
		ballpos[i].v[1] = 4.6;
		int neg = (rand() % 2) + 1;
		ballpos[i].v[2] = (rand()%40 *(pow(-1, neg)))+1;
		ballpos[i].v[0] = rand() % 90 * (pow(-1, neg))+15;

	}



	glClearColor(0, 0, 0, 0);

	glutDisplayFunc(callback_display);
	glutMouseFunc(callback_mouse);
	glutMotionFunc(callback_motion);
	glutKeyboardFunc(callback_keyboard);
	glutIdleFunc(cb_idle);
	


	//hand control to glut
	glutMainLoop();
	free(collid);
	free(collided);
	free(ballpos);
	free(ballVel);
	free(ballTex);
	free(skin2);

	return 0;
}