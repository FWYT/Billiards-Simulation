#include "vec3f.h"
#include <iostream>
#include <math.h>

#include "cue-ball.h"
#include "sliding.h"

#include "Constants.h"

using namespace std;

Vec3f relativeVel(double t)
{
	double f = calcForce();
	Vec3f initV = flipYZ(linearVelocity(f));
	//cout << "init v slide: " << initV[0] << " " << initV[1] << " " << initV[2] << endl;

	Vec3f initA = flipYZ(angularVelocity(f));

	Vec3f initU = initV + (ballRadius*Vec3f(0, 0, 1)).cross(initA);
	//cout << "init u slide: " << initU[0] << " " << initU[1] << " " << initU[2] << endl;
	
	if (t == 0)
	{
		return initU;
	}

	return initU - (7.0 / 2)*friction*g*t*initU.normalize();
}

Vec3f position(Vec3f relV, double t, Vec3f originalPos)
{
	double r_B[2];
	r_B[0] = V0*t - (1.0 / 2)*friction*g*pow(t, 2)*relV.normalize()[0];
	r_B[1] = -(1.0 / 2)*friction*g*pow(t, 2)*relV.normalize()[1];

	/*cout << "sliding t " << t << endl;
	cout << "V0 " << V0 << endl;
	cout << "V0*t " << V0*t << endl;
	cout << "R_B[0] " << r_B[0] << endl;
	cout << "R_B[1] " << r_B[1] << endl;*/

	double r_T[2];
	r_T[0] = cos(theta)*r_B[0] - sin(theta)*r_B[1];
	r_T[1] = sin(theta)*r_B[0] + cos(theta)*r_B[1];

	/*cout << "R_T[0] " << r_T[0] << endl;
	cout << "R_T[1] " << r_T[1] << endl;*/

	return originalPos + Vec3f(r_T[0], 0.0, r_T[1]);
}

Vec3f linearVelocityS(double t)
{
	double f = calcForce();
	Vec3f initV = linearVelocity(f);
	Vec3f relV0 = relativeVel(0);

	return initV - friction*g*t*relV0.normalize();
}

Vec3f angularVelocityS(double t)
{
	double f = calcForce();
	Vec3f ang0 = angularVelocity(f);
	Vec3f relV0 = relativeVel(0);

	return ang0 - ((5 * friction*g) / (2 * ballRadius))*t*(Vec3f(0, 0, 1).cross(relV0.normalize()));
}

double angularVelocitySR_perp(double t)
{
	double f = calcForce();
	Vec3f ang0 = angularVelocity(f);

	return ang0[1] - ((5 * friction*g) / (2 * ballRadius))*t;
}

double slideDuration()
{
	Vec3f relV0 = relativeVel(0);

	return (2.0 * relV0.magnitude()) / (7.0 * friction*g);
}
