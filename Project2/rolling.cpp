#include "vec3f.h"
#include <iostream>
#include <math.h>

#include "cue-ball.h"
#include "sliding.h"
#include "rolling.h"

#include "Constants.h"

using namespace std;

Vec3f positionR(double t, Vec3f initPos, int collidedRail, double a, double b, double c, double V0)
{
	double F = calcForce(a, b, c, V0);
	Vec3f initV = flipYZ(linearVelocity(F));

	double loss;
	if (collidedRail)
	{
		loss = F*collisionLoss;
	}
	else
	{
		loss = 0;
	}

	if (collidedRail)
	{
		//cout << "COLLIDED" << endl;
	}
	Vec3f change = (pow(-1, collidedRail)*initV*t) - pow(-1, collidedRail)*(1.0 / 2)*friction*g*pow(t, 2)*initV.normalize();
	change[0] += loss;
	//cout << "init v: " << initV[0] << " " << initV[1] << " " << initV[2] << endl;
	//cout << "rolling t " << t << endl;
	//cout << "initV*t " << (initV*t)[0] << " " << (initV*t)[1] << " " << (initV*t)[0] << endl;
	//cout << "diff: " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[0] << " " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[1] << " " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[2] << endl;
	//cout << "change: " << change[0] << " " << change[1] << " " << change[2] << endl;
	return initPos + change;
}

Vec3f linearVelocityR(double t, double a, double b, double c, double V0)
{
	double F = calcForce(a, b, c, V0);
	Vec3f initV = linearVelocity(F);

	return initV - frictionR*g*t*initV.normalize();
}

double angularVelocityR(double t, double a, double b, double c, double V0)
{
	double velMag = linearVelocityR(t, a, b, c, V0).magnitude();
	return velMag / ballRadius;
}

double rollDuration(double a, double b, double c, double V0)
{
	double F = calcForce(a, b, c, V0);
	Vec3f initV = linearVelocity(F);

	return initV.magnitude() / (frictionR*g);
}