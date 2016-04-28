#pragma once
Vec3f positionR(double t, Vec3f initPos, int collidedRail);
Vec3f linearVelocityR(double t);

double angularVelocityR(double t);
double rollDuration(Vec3f initV);