#include "vec3f.h"
#include <iostream>
#include <math.h>
#include "Constants.h"
#include "cue-ball.h"

using namespace std;

double calcForce(double a, double b, double c, double V0)
{
	//cout << a << " " << b << " " << c << endl;
	double F_top = 2 * ballMass * V0;
	double F_bot = 1 + (ballMass / cueMass) + (5.0 / (2 * pow(ballRadius, 2)))*
		(pow(a, 2) + pow(b, 2)*pow(cos(theta), 2) + pow(c, 2)*pow(sin(theta), 2) - 2 * b*c*cos(theta)*sin(theta));

	return F_top / F_bot;
}

Vec3f linearVelocity(double F)
{
	//return Vec3f(0.0, (-F / ballMass)*cos(theta), (-F / ballMass)*sin(theta));
	return Vec3f(0.0, (F / ballMass)*cos(theta), (F / ballMass)*sin(theta));
}

Vec3f angularVelocity(double F, double a, double b, double c)
{
	double MOI = (2.0 / 5)*ballMass*pow(ballRadius, 2);

	return (1 / MOI)*Vec3f(-c*F*sin(theta) + b*F*cos(theta), a*F*sin(theta), -a*F*cos(theta));
}

Vec3f flipYZ(Vec3f v)
{
	//double z = v[2];
	double x = v[0];
	double y = v[1];
	return Vec3f(y, x, v[2]);
}

//rotate position vector to the cue vector
//returns rotation matrix
Vec3f rotateToCue(Vec3f pos, Vec3f cue, Vec3f pt)
{

	pos = pos.normalize();
	pos = Vec3f(1.0, 0.0, 0.0);
	cue = cue.normalize();
	//cout << "pos " << pos << endl;
	//cout << "cue" << cue << endl;
	//cout << "pos.dot(cue) " << pos.dot(cue) << endl;
	//cout << "(pos.magnitude() * cue.magnitude()) " << (pos.magnitude() * cue.magnitude()) << endl;
	double theta = acos(pos.dot(cue) / (pos.magnitude() * cue.magnitude()));
	//cout << "theta " << theta << endl;

	
	//cout << "pt[0] pt[2] " << pt[0] << " " << pt[2] << endl;
	double xt = pt[0] * cos(theta) - pt[2] * sin(theta);
	double yt = pt[0] * sin(theta) + pt[2] * cos(theta);
	//cout << "yt " << yt << endl;

	if (cue[2] < 0)
	{
		yt *= -1;
	}
	
	return Vec3f(xt, 4.6, yt);

}

Vec3f matrixMult(Vec3f* R, Vec3f pos)
{
	Vec3f rPos = Vec3f(0.0, 0.0, 0.0);
	
	rPos[0] = R[0][0] * pos[0] + R[0][1] * pos[1] + R[0][2] * pos[2];
	rPos[1] = R[1][0] * pos[0] + R[1][1] * pos[1] + R[1][2] * pos[2];
	rPos[2] = R[2][0] * pos[0] + R[2][1] * pos[1] + R[2][2] * pos[2];

	return rPos;
}