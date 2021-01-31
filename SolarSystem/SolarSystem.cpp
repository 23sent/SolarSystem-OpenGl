//Utku Sagocak
#include <iostream>
#include <GL\freeglut.h>
#include <vector>
#include <cmath>
#include "Icosphere.h"
#include "CelestialBody.h"

//constant to scale planets sizes.
#ifndef KM_FACTOR
#define KM_FACTOR 0.000004
#endif

using namespace std;

/*
Interactive Functions
	Keyboard
	l -> Enable/Disable Lighting
	t -> Enable/Disble Texture
	o -> Enable/Disable drawOrbit
	- -> decrease time speed
	+ -> increase time speed
	0 -> reset camera rotation/position
	w, s -> change X axis rotation of camera around sun
	a, d -> change Z axis rotation of camera around sun
	
	Mouse
	left click and mouse motion-> change view angle
	right click -> glut menu, to change light color, speed or texture
	scroll up/down -> zoom in/out to sun (move in z axis)

Distance and Sizes
	Orbitals distances is life-size as AU (astronomical unit). Planets sizes also proportional.
	
	But the sun's size a bit tricky. I could not do sun as big as in life. 
	Because when I tried enlarging sun, other planets was absorbed by sun. 
	Then I tried enlarge orbits with sun, but this time the planets became small and impossible to see.
	So, I scaled sun's size different from other distances.

Orbital and Sphere
	Celestial bodies has elliptic orbitals like real life, and sun at one of the two focus. 
	I preferred Icospheres (from last homework) to render planets.

Sources
	Texture images from: https://www.solarsystemscope.com/textures/
*/

typedef enum {
	RED_LIGHT,
	WHITE_LIGHT,
	BLUE_LIGHT,
	NO_LIGHT
}LightType;

GLfloat white_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat white_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat white_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat red_ambient[] = { 0.3f, 0.01f, 0.01f, 1.0f };
GLfloat red_diffuse[] = { 1.0f, 0.1f, 0.1f, 1.0f };
GLfloat red_specular[] = { 1.0f, 0.2f, 0.2f, 1.0f };

GLfloat blue_ambient[] = { 0.01f, 0.01f, 0.3f, 1.0f };
GLfloat blue_diffuse[] = { 0.1f, 0.1f, 1.0f, 1.0f };
GLfloat blue_specular[] = { 0.2f, 0.2f, 2.0f, 1.0f };

GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

float universalTime = 70.5f; //time as earth day. To calculate planets positions.
float timeIncrement = .002f;

bool isTextured = true;
bool isLighting = true;
bool isMouseDown = false;
bool drawOrbits = true;
int mouseX, mouseY;
float cameraAngleX, cameraAngleY, cameraRotationX, cameraRotationZ, cameraPositionZ = 0.1f;

vector<CelestialBody> solarSystem; //a vectore that puts planets and sun
CelestialBody universe(0.0f, 0.0f, 0.1f, 0.1f, 100.5000f); // this is the outer wall of my solar system. To display wilkway texture.

//I used icosphere's to model planets, sun and outer wall.
Icosphere planet_sphere(3); //this one uses to draw planets. Subdiveded 3 times.
Icosphere sun_sphere(5); //this one uses to draw sun and universe. Subdiveded 5 times.

//starting process
void init(); 
//creates and set planets, moon, outer wall and sun
void createSolarSystem();
//render and display
void display(); 
//glut funcs
void timer(int value);
void reshape(int x, int y);
void press(unsigned char key, int x, int y); //keyboard input
//righ-click/keyboard input setting functions
void resetCameraRotation();
void changeLightColor(int color);
void disableTexture(int value);
void mainMenu(int value);

//mouse rotation
void mouseMotion(int x, int y);
void mouseClick(int button, int state, int x, int y);

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(1250, 750);
	glutCreateWindow("Solar System");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GL_DEPTH);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(press);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);

	glutTimerFunc(40, timer, 0);

	glutMainLoop();
}
void init() {
	int light_menu, texture_menu;

	//create right-click menu
	light_menu = glutCreateMenu(changeLightColor);
	glutAddMenuEntry("RED LIGHT", RED_LIGHT);
	glutAddMenuEntry("BLUE LIGHT", BLUE_LIGHT);
	glutAddMenuEntry("WHITE LIGHT", WHITE_LIGHT);
	glutAddMenuEntry("ON\\OF LIGHT", NO_LIGHT);

	texture_menu = glutCreateMenu(disableTexture);
	glutAddMenuEntry("Disable Texture", 0);
	glutAddMenuEntry("Enable Texture", 1);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("Light", light_menu);
	glutAddSubMenu("Texture", texture_menu);
	glutAddMenuEntry("Reset Rotation", 0);
	glutAddMenuEntry("Speed Up", 1);
	glutAddMenuEntry("Speed Down", -1);


	glutAttachMenu(GLUT_RIGHT_BUTTON);

	resetCameraRotation(); //reset camera position
	createSolarSystem();

	glLightfv(GL_LIGHT0, GL_AMBIENT, white_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_specular);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}
void createSolarSystem() {
	//create planets and sun and load textures

	//semi-minor-axis (AU), semi-major-axis (AU), orbitPeriod (earth day), rotationPeriod (earth day), radius (KM)  <- parameters
	CelestialBody sun(0.0, 0.0, 0.1f, 25.0f, 696340 * KM_FACTOR * 0.095); // "I scaled sun's size different from other distances."
	sun.loadTexture(".\\Texture\\sun.bmp", 2048, 1024);

	CelestialBody mercury(0.38700f, 0.38700f, 88.0f, 58.6f, 2440.0 * KM_FACTOR);
	mercury.loadTexture(".\\Texture\\mercury.bmp", 1024, 512);

	CelestialBody venus(0.72298f, 0.72300f, 224.7f, -243.0f, 6052.0 * KM_FACTOR);
	venus.loadTexture(".\\Texture\\venus_atmosphere.bmp", 1024, 512);

	CelestialBody earth(0.99986f, 1.0f, 365.25f, .99f, 6371.0 * KM_FACTOR);
	earth.loadTexture(".\\Texture\\earth.bmp", 1024, 512);

	CelestialBody mars(1.51740f, 1.52400f, 687.0f, 1.03f, 3390 * KM_FACTOR);
	mars.loadTexture(".\\Texture\\mars.bmp", 1024, 512);

	CelestialBody jupiter(5.19820f, 5.20440f, 4331.0f, 0.41f, 69911 * KM_FACTOR);
	jupiter.loadTexture(".\\Texture\\jupiter.bmp", 1024, 512);

	CelestialBody saturn(9.56730f, 9.58260f, 10747.0f, 0.45f, 58232 * KM_FACTOR);
	saturn.loadTexture(".\\Texture\\saturn.bmp", 1024, 512);
	saturn.setRingRadius(92000 * KM_FACTOR, 136775 * KM_FACTOR);

	CelestialBody uranus(19.19770f, 19.21840f, 30589.0f, 0.72f, 25362 * KM_FACTOR);
	uranus.loadTexture(".\\Texture\\uranus.bmp", 1024, 512);

	CelestialBody neptune(30.10870f, 30.11000f, 59800.0f, 0.67f, 24622 * KM_FACTOR);
	neptune.loadTexture(".\\Texture\\neptune.bmp", 1024, 512);

	//create moon and add to earth
	CelestialBody moon(0.0025695553 + (6371.0 + 1737.0) * KM_FACTOR, 0.0025695553 + (6371.0 + 1737.0) * KM_FACTOR, 27.3f, 27.3f, 1737.0 * KM_FACTOR);
	moon.loadTexture(".\\Texture\\moon.bmp", 1024, 512);
	earth.addMoon(moon);

	solarSystem.push_back(sun);
	solarSystem.push_back(mercury);
	solarSystem.push_back(venus);
	solarSystem.push_back(earth);
	solarSystem.push_back(mars);
	solarSystem.push_back(jupiter);
	solarSystem.push_back(saturn);
	solarSystem.push_back(uranus);
	solarSystem.push_back(neptune);

	//load milkway texture
	universe.loadTexture(".\\Texture\\stars.bmp", 2048, 1024);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set camera
	glRotatef(cameraAngleX, 1.0, .0, .0);
	glRotatef(cameraAngleY, .0, 1.0, .0);
	glTranslatef(.0f, .0f, cameraPositionZ);

	glRotatef(cameraRotationX, 1, 0, 0);
	glRotatef(cameraRotationZ, 0, 0, 1);


	//set light position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	int length = solarSystem.size();

	//first create sun and draw orbits, because they will be not lighting
	solarSystem[0].draw(universalTime, sun_sphere, isTextured, false);

	if (drawOrbits) {
		for (int i = 1; i < length; i++) {
			solarSystem[i].drawOrbit();
		}
	}

	//create planets
	for (int i = 1; i < length; i++) {
		solarSystem[i].draw(universalTime, planet_sphere, isTextured, isLighting);
	}

	universe.draw(0, sun_sphere, true, false);

	glutSwapBuffers();
}

void resetCameraRotation() {
	cameraAngleX = 0.0f;
	cameraAngleY = -20.0f;
	cameraRotationX = -80.0f;
	cameraRotationZ = 130.0f;
	cameraPositionZ = -2.0f;
}

void disableTexture(int value) {
	isTextured = value;
}

void changeLightColor(int color) {
	if (color == RED_LIGHT) {
		glLightfv(GL_LIGHT0, GL_AMBIENT, red_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, red_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, red_specular);
	}
	else if (color == BLUE_LIGHT) {
		glLightfv(GL_LIGHT0, GL_AMBIENT, blue_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, red_specular);
	}
	else if (color == WHITE_LIGHT) {
		glLightfv(GL_LIGHT0, GL_AMBIENT, white_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_specular);
	}
	else if (color == NO_LIGHT) {
		isLighting = !isLighting;
	}

}
void mainMenu(int value) {
	if (value == 0) {
		resetCameraRotation();
		return;
	}
	timeIncrement += value/10.0f;
}

void timer(int value) {
	universalTime += timeIncrement;
	glutPostRedisplay();
	glutTimerFunc(30, timer, 0);
}
void reshape(int x, int y) {
	if (x == 0 || y == 0) return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.1, 500.0);

	glViewport(0, 0, x, y);
}

void press(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		cameraRotationX += 1.0f;
		break;
	case 's':
		cameraRotationX -= 1.0f;
		break;
	case 'd':
		cameraRotationZ -= 1.0f;
		break;
	case 'a':
		cameraRotationZ += 1.0f;
		break;
	case 't':
		isTextured = !isTextured;
		break;
	case '+':
		timeIncrement += .1f;
		break;
	case '-':
		timeIncrement -= .1f;
		break;
	case 'l':
		isLighting = !isLighting;
		break;
	case '0':
		resetCameraRotation();
		break;
	case 'o':
		drawOrbits = !drawOrbits;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void mouseMotion(int x, int y) {
	if (isMouseDown) {
		cameraAngleX += (y - mouseY) / 10.0f;
		cameraAngleY += (x - mouseX) / 10.0f;
	}
	mouseX = x;
	mouseY = y;
	glutPostRedisplay();
}
void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseX = x;
			mouseY = y;
			isMouseDown = true;
		}
		else {
			isMouseDown = false;
		}
	}
	else if (button == 4) {
		cameraPositionZ -= .1f;
	}
	else if (button == 3) {
		cameraPositionZ += .1f;
	}
}