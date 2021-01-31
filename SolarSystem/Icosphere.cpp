#include <iostream>
#include <vector>
#include <array>
#include <map>
#include "Icosphere.h"

#ifndef PI
#define PI 3.1415926535
#endif

Icosphere::Icosphere(int level) {
	this->vertices =
	{
		{-A,C,B}, {A,C,B}, {-A,C,-B}, {A,C,-B},
		{C,B,A}, {C,B,-A}, {C,-B,A}, {C,-B,-A},
		{B,A,C}, {-B,A, C}, {B,-A,C}, {-B,-A, C}
	};

	this->triangles =
	{
		{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
		{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
		{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
		{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
	};
	this->tesellation(level);
	this->calculateNormals();
	this->textureCoord();
}

void Icosphere::drawSphere(float radius) {
	int length = triangles.size();
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < length; i++) {

		glTexCoord2f(textureCoordinates[textureFace[i][0]][0], textureCoordinates[textureFace[i][0]][1]);
		glNormal3fv(&vertexNormals[triangles[i][0]][0]); //per-vertex normal
		glVertex3f(vertices[triangles[i][0]][0] * radius, vertices[triangles[i][0]][1] * radius, vertices[triangles[i][0]][2] * radius);

		glTexCoord2f(textureCoordinates[textureFace[i][1]][0], textureCoordinates[textureFace[i][1]][1]);
		glNormal3fv(&vertexNormals[triangles[i][1]][0]); //per-vertex normal
		glVertex3f(vertices[triangles[i][1]][0] * radius, vertices[triangles[i][1]][1] * radius, vertices[triangles[i][1]][2] * radius);

		glTexCoord2f(textureCoordinates[textureFace[i][2]][0], textureCoordinates[textureFace[i][2]][1]);
		glNormal3fv(&vertexNormals[triangles[i][2]][0]); //per-vertex normal
		glVertex3f(vertices[triangles[i][2]][0] * radius, vertices[triangles[i][2]][1] * radius, vertices[triangles[i][2]][2] * radius);
	}
	glEnd();
}

int Icosphere::getMidpoint(GLuint v1, GLuint v2, std::map<std::pair<int, int>, int>& isVisited, int &vertexCount) {
	std::pair<int, int> midPointKey;
	//creates a int pair with vertex indices
	if (v1 > v2)
		midPointKey = std::make_pair(v2, v1);
	else
		midPointKey = std::make_pair(v1, v2);

	//check the pair is in map.
	if (isVisited.find(midPointKey) == isVisited.end()) {
		//calculate midpoint
		vertices.push_back({
			   (vertices[v1][0] + vertices[v2][0]) / 2,
			   (vertices[v1][1] + vertices[v2][1]) / 2,
			   (vertices[v1][2] + vertices[v2][2]) / 2
			});
		//normalize midpoint according to center and radius of sphere
		this->vectorNormalize(&vertices[vertexCount][0]);
		//add midpoint to visited pairs
		isVisited.insert(make_pair(midPointKey, vertexCount));

		//return midpoint index
		vertexCount++;
		return vertexCount - 1;
	}
	else {
		//if midpoint calculated before
		return isVisited[midPointKey];
	}
}

void Icosphere::tesellation(int level) {
	std::map<std::pair<int, int>, int> isVisited;

	int triangleCount = triangles.size();
	int vertexCount = vertices.size();
	int factor = pow(4, level - 1);

	//Calculates new size of face and vertices. 
	triangles.reserve(triangleCount * factor); //each triangle is divided by four for every level.
	vertices.reserve(vertexCount + (triangleCount * 3 / 2) * (1 - factor) / -3); //and vertices increase 3*FACECOUNT/2 for every level.

	this->tesellationRecursive(level, isVisited, triangleCount, vertexCount);
}

void Icosphere::tesellationRecursive(int level, std::map<std::pair<int, int>, int>& isVisited, int triangleCount, int vertexCount) {
	if (level <= 1) return;
	GLuint v12, v02, v01;

	for (int i = 0; i < triangleCount; i++) {
		v01 = this->getMidpoint(triangles[i][0], triangles[i][1], isVisited, vertexCount);
		v12 = this->getMidpoint(triangles[i][1], triangles[i][2], isVisited, vertexCount);
		v02 = this->getMidpoint(triangles[i][0], triangles[i][2], isVisited, vertexCount);;

		//adds new triangles
		triangles.push_back({ triangles[i][0], v01, v02 });
		triangles.push_back({ v01, triangles[i][1], v12 });
		triangles.push_back({ v02, v12, triangles[i][2] });

		//changes original triangle as new triangle
		triangles[i][2] = v01; triangles[i][0] = v12; triangles[i][1] = v02;

	}
	triangleCount = triangleCount * 4;
	this->tesellationRecursive(level - 1, isVisited, triangleCount, vertexCount);
}

void Icosphere::calculateNormals() {
	int length = triangles.size();
	FaceNormals.clear();
	vertexNormals.clear();
	FaceNormals.resize(length);
	vertexNormals.resize(vertices.size());

	for (int i = 0; i < length; i++) {
		//first calculate face normals according to two side
		findFaceNormal(
			&vertices[triangles[i][0]][0],
			&vertices[triangles[i][1]][0],
			&vertices[triangles[i][2]][0],
			&FaceNormals[i][0]);
		FaceNormals[i][0] *= -1; 
		FaceNormals[i][1] *= -1;
		FaceNormals[i][2] *= -1;

		//then calculate vertex normals according to face normals
		for (int j = 0; j < 3; j++) {
			vertexNormals[triangles[i][j]][0] += FaceNormals[i][0];
			vertexNormals[triangles[i][j]][1] += FaceNormals[i][1];
			vertexNormals[triangles[i][j]][2] += FaceNormals[i][2];
		}
	}
}

void Icosphere::textureCoord() {
	float u;
	float v;

	int l = vertices.size();
	for (int i = 0; i < l; i++) {
		u = (atan2(vertices[i][0], vertices[i][2]) + PI) / (PI * 2.0f);
		v = (acos(vertices[i][1]) + PI) / PI - 1.0f;
		textureCoordinates.push_back({ u, v });
	}
	fixTexture();
}

//Changes u coordinates. To fix interpolation in texture.
void Icosphere::fixTexture() {
	GLfloat normal[3];
	textureFace = triangles;

	int length = triangles.size();
	for (int i = 0; i < length; i++) {
		GLfloat texA[] = { textureCoordinates[triangles[i][0]][0],  textureCoordinates[triangles[i][0]][1], 0 };
		GLfloat texB[] = { textureCoordinates[triangles[i][1]][0],  textureCoordinates[triangles[i][1]][1], 0 };
		GLfloat texC[] = { textureCoordinates[triangles[i][2]][0],  textureCoordinates[triangles[i][2]][1], 0 };
		findFaceNormal(texA, texB, texC, normal);
		if (normal[2] < 0)
		{
			if (texA[0] < 0.25) {
				textureCoordinates.push_back({ 1.0f, texA[1] });
				textureFace[i][0] = textureCoordinates.size() - 1;
			}
			if (texB[0] < 0.25) {
				textureCoordinates.push_back({ 1.0f, texB[1] });
				textureFace[i][1] = textureCoordinates.size() - 1;
			}
			if (texC[0] < 0.25) {
				textureCoordinates.push_back({ 1.0f, texC[1] });
				textureFace[i][2] = textureCoordinates.size() - 1;
			}
		}

	}
}

void Icosphere::vectorNormalize(GLfloat* normal) {
	float length = sqrt(pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2));
	if (length > 0.000001f) {
		normal[0] = normal[0] / length ;
		normal[1] = normal[1] / length ;
		normal[2] = normal[2] / length ;
	}
}

void Icosphere::vectorProduct(GLfloat* v1, GLfloat* v2, GLfloat* n) {
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v2[0] * v1[2] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

void Icosphere::findFaceNormal(GLfloat* v1, GLfloat* v2, GLfloat* v3, GLfloat* n) {
	GLfloat vertex1[3] = {
		v2[0] - v1[0], 
		v2[1] - v1[1], 
		v2[2] - v1[2]
	};
	GLfloat vertex2[3] = {
		v3[0] - v1[0], 
		v3[1] - v1[1], 
		v3[2] - v1[2]
	};
	vectorProduct(vertex1, vertex2, n);
	vectorNormalize(n);
}