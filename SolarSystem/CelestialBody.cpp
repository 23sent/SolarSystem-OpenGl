#include <GL\freeglut.h>
#include <vector>
#include <cmath>
#include "CelestialBody.h"
#include "Icosphere.h"

CelestialBody::CelestialBody(double semiMin, double semiMaj, float orbitPeriod, float rotationPeriod, double r) {
	this->semiMinor = semiMin; //short radius of eliptic orbit
	this->semiMajor = semiMaj; //long radius of eliptic orbit
	this->orbitPeriod = orbitPeriod; //as earth day
	this->rotationPeriod = rotationPeriod; //as earth day
	this->radius = r;
	this->focus = sqrt(pow(semiMajor, 2) - pow(semiMinor, 2)); //"The orbit of a planet is an ellipse with the Sun at one of the two foci."
}

void CelestialBody::draw(float gTime, Icosphere &sphere, bool isTextured, bool isLighting) {
	glPushMatrix();

	if (isTextured)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	orbitAngle = gTime * 2.0 * PI / orbitPeriod; // to calculate planet position
	rotationAngle = gTime * 360 / rotationPeriod; // calculate planet rotation

	glTranslated(semiMajor * cos(orbitAngle) - focus, semiMinor * sin(orbitAngle), 0.0); //move body to current position

	//Create moons if it has
	int moonLength = this->moons.size();
	for (int i = 0; i < moonLength; i++) {
		moons[i].drawOrbit();
		moons[i].draw(gTime, sphere, isTextured, isLighting);
	}

	//create ring if it has
	if (ringInnerRadius > 0) {
		glPushMatrix();
		glRotatef(-20, .0, 1.0, .0);
		this->drawRing();
		glPopMatrix();
	}

	glRotatef(-90, 1.0, .0, .0);
	glRotatef(-rotationAngle, 0.0, 1.0, .0);

	if (isLighting)
		glEnable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, texture);
	sphere.drawSphere(radius);
	glDisable(GL_LIGHTING);

	glPopMatrix();
}

void CelestialBody::drawRing(){
	glPushMatrix();
	glBegin(GL_QUAD_STRIP);
	float theta = 0.0f;
	while(theta < 2 * PI)
	{
		glVertex3f(ringInnerRadius * cos(theta), ringInnerRadius * sin(theta), 0.0f);
		glVertex3f(ringOuterRadius * cos(theta), ringOuterRadius * sin(theta), 0.0f);
		theta += 0.06f;
	}
	glVertex3f(ringInnerRadius * cos(theta), ringInnerRadius * sin(theta), 0.0f);
	glVertex3f(ringOuterRadius * cos(theta), ringOuterRadius * sin(theta), 0.0f);
	glEnd();
	glPopMatrix();
}

void CelestialBody::drawOrbit() {
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	float theta = 0.0f;
	while(theta < 2 * PI)
	{
		glVertex3f(semiMajor * cos(theta) - focus, semiMinor * sin(theta), 0.0f);
		theta += 0.06f;
	}
	glVertex3f(semiMajor * cos(theta) - focus, semiMinor * sin(theta), 0.0f);
	glEnd();

	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

//load bmp textures
void CelestialBody::loadTexture(const char* filename, int width, int height) {
	unsigned char* data;
	int imageSize = width * height * 3;
	unsigned char colorSwap;
	FILE* file;

	fopen_s(&file, filename, "rb");

	if (file == NULL) return;

	data = (unsigned char*)malloc(width * height * 3);
	fread(data, width * height * 3, 1, file);
	fclose(file);


	//swap locations of red and blue
	for (int i = 0; i < imageSize; i += 3)
	{
		colorSwap = data[i];
		data[i] = data[i + 2];
		data[i + 2] = colorSwap;
	}

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);
}