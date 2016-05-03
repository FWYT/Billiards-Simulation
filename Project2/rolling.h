#pragma once
Vec3f positionR(double t, Vec3f initPos, int collidedRail, double a, double b, double c, double V0);
Vec3f linearVelocityR(double t, double a, double b, double c, double V0);

double angularVelocityR(double t, double a, double b, double c, double V0);
double rollDuration(double a, double b, double c, double V0);