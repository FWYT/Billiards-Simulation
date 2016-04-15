#include "vec3f.h"
#include <iostream>
#include <math.h>

#include "cue-ball.h"
#include "sliding.h"
#include "rolling.h"

#include "Constants.h"

using namespace std;

Vec3f positionR(double t, Vec3f initPos)
{
	double F = calcForce();
	Vec3f initV = flipYZ(linearVelocity(F));

	Vec3f change = (initV*t) - (1.0 / 2)*friction*g*pow(t, 2)*initV.normalize();
	//cout << "init v: " << initV[0] << " " << initV[1] << " " << initV[2] << endl;
	//cout << "rolling t " << t << endl;
	//cout << "initV*t " << (initV*t)[0] << " " << (initV*t)[1] << " " << (initV*t)[0] << endl;
	//cout << "diff: " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[0] << " " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[1] << " " << ((1.0 / 2)*friction*g*pow(t, 2)*initV.normalize())[2] << endl;
	//cout << "change: " << change[0] << " " << change[1] << " " << change[2] << endl;
	return initPos + change;
}

Vec3f linearVelocityR(double t)
{
	double F = calcForce();
	Vec3f initV = linearVelocity(F);

	return initV - friction*g*t*initV.normalize();
}

double angularVelocityR(double t)
{
	double velMag = linearVelocityR(t).magnitude();
	return velMag / ballRadius;
}

double rollDuration(Vec3f initV)
{
	//double F = calcForce();
	//Vec3f initV = linearVelocity(F);

	return initV.magnitude() / (friction*g);
}