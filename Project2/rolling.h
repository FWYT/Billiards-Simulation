#pragma once
Vec3f positionR(double t, Vec3f initPos, int collidedRail, double V0);
Vec3f linearVelocityR(double t, double V0);

double angularVelocityR(double t, double V0);
double rollDuration(Vec3f initV);