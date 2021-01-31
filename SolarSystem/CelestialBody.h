#ifndef CELESTIALBODY_H_
#define CELESTIALBODY_H_
#include <GL\freeglut.h>
#include <vector>
#include <cmath>
#include "Icosphere.h"


#ifndef PI
#define PI 3.1415926535
#endif

//Class to create planets, sun, moon and outer wall of solar system

class CelestialBody
{
private:
	double semiMinor; //short radius of eliptic orbit
	double semiMajor; //long radius of eliptic orbit
	float orbitPeriod;
	float rotationPeriod;
	double radius;
	double rotationAngle = 0.0;
	double orbitAngle = 0.0;
	float ringInnerRadius = 0.0f;
	float ringOuterRadius = 0.0f;
	double focus; // distance between focus and center of eliptic orbit. To calculate planet's positon according to sun
	GLuint texture = 0;

	std::vector<CelestialBody> moons;

public:
	CelestialBody(double semiMin, double semiMaj, float orbitPeriod, float rotationPeriod, double r);

	void draw(float gTime, Icosphere &sphere, bool isTextured, bool isLighting);
	void loadTexture(const char* filename, int width, int height);

	void drawRing();
	void drawOrbit();

	//setters
	void setRingRadius(float ringI, float ringO) { this->ringInnerRadius = ringI; this->ringOuterRadius = ringO; };
	void addMoon(CelestialBody moon) { moons.push_back(moon); };
};

#endif

