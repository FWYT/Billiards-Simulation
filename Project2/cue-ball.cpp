#include "vec3f.h"
#include <iostream>
#include <math.h>
#include "Constants.h"
#include "cue-ball.h"


double calcForce()
{
	double F_top = 2 * ballMass * V0;
	double F_bot = 1 + (ballMass / cueMass) + (5.0 / (2 * pow(ballRadius, 2)))*
		(pow(a, 2) + pow(b, 2)*pow(cos(theta), 2) + pow(c, 2)*pow(sin(theta), 2) - 2 * b*c*cos(theta)*sin(theta));

	return F_top / F_bot;
}

Vec3f linearVelocity(double F)
{
	//return Vec3f(0.0, (-F / ballMass)*cos(theta), (-F / ballMass)*sin(theta));
	return Vec3f(0.0, (-F / ballMass)*cos(theta), (-F / ballMass)*sin(theta));
}

Vec3f angularVelocity(double F)
{
	double MOI = (2.0 / 5)*ballMass*pow(ballRadius, 2);

	return (1 / MOI)*Vec3f(-c*F*sin(theta) + b*F*cos(theta), a*F*sin(theta), -a*F*cos(theta));
}

Vec3f flipYZ(Vec3f v)
{
	double z = v[2];
	double y = v[1];
	return Vec3f(v[0], z, y);
}