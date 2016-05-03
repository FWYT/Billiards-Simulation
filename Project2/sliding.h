#include "vec3f.h"

Vec3f relativeVel(double t, double a, double b, double c, double V0);

Vec3f position(Vec3f relV, double t, Vec3f originalPos, int railCollided, double a, double b, double c, double V0);
Vec3f linearVelocityS(double t, double a, double b, double c, double V0);

Vec3f angularVelocityS(double t, double a, double b, double c, double V0);

double angularVelocitySR_perp(double t, double a, double b, double c, double V0);

double slideDuration(double a, double b, double c, double V0);