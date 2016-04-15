#pragma once
#include <math.h>

static const double ballMass = 0.165; //kg
static const double cueMass = 0.539; //kg
static const double ballRadius = 1.875;
static const double V0 = 2.0; //initial cue velocity
static const double theta = 0.0;

static const double g = 0.5; //gravity
static const double friction = 0.5;



static const double a = 0.0; //horizontal displacement
static const double b = 0.0; //vertical displacement
static const double c = sqrt(pow(ballRadius, 2) - pow(a, 2) - pow(b, 2));