#include "vec3f.h"

Vec3f relativeVel(double t);

Vec3f position(Vec3f relV, double t, Vec3f originalPos);
Vec3f linearVelocityS(double t);

Vec3f angularVelocityS(double t);

double angularVelocitySR_perp(double t);

double slideDuration();