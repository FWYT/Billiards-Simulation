#include "vec3f.h"

double calcForce(double a, double b, double c, double V0);
Vec3f linearVelocity(double F);
Vec3f angularVelocity(double F, double a, double b, double c);
Vec3f flipYZ(Vec3f v);
Vec3f rotateToCue(Vec3f pos, Vec3f cue, Vec3f pt);
Vec3f matrixMult(Vec3f* R, Vec3f pos);