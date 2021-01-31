#ifndef ICOSPHERE_H_
#define ICOSPHERE_H_
#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <GL\freeglut.h>


class Icosphere
{

private:
	const float A = .525f;
	const float B = .850f;
	const float C = 0.0f;
	float radius;


	std::vector<std::array<GLfloat, 3>> vertices;
	std::vector<std::array<GLuint, 3>> triangles;

	std::vector<std::array<GLfloat, 2>> textureCoordinates;
	std::vector<std::array<GLuint, 3>> textureFace;

	std::vector<std::array<GLfloat, 3>> FaceNormals;
	std::vector<std::array<GLfloat, 3>> vertexNormals;
	
	int getMidpoint(GLuint v1, GLuint v2, std::map<std::pair<int, int>, int>& isVisited, int &vertexCount);
	void tesellationRecursive(int depth, std::map<std::pair<int, int>, int>& isVisited, int triangleCount, int vertexCount);

	void calculateNormals();

	void textureCoord();//Calculate uv coordinates
	void fixTexture();//Changes u coordinates. To fix interpolation in texture.

	void findFaceNormal(GLfloat* vertex, GLfloat* vertex1, GLfloat* vertex2, GLfloat* normal);
	void vectorNormalize(GLfloat* normal);
	void vectorProduct(GLfloat* v1, GLfloat* v2, GLfloat* v3);

public:
	Icosphere(int level);
	void tesellation(int level);
	void drawSphere(float radius);
};

#endif

