#include "vec3f.h"

double calcForce();
Vec3f linearVelocity(double F);
Vec3f angularVelocity(double F);
Vec3f flipYZ(Vec3f v);
Vec3f rotateToCue(Vec3f pos, Vec3f cue, Vec3f pt);
Vec3f matrixMult(Vec3f* R, Vec3f pos);