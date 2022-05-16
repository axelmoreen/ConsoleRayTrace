// ConsoleRayTrace.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "Objects3d.h"
#include <sstream>
#include <vector>
#include <windows.h>
#include <chrono>
#include<thread>
#include <functional>

using namespace std;
using namespace chrono;

const char characters[12] = {
	' ', '.', ',', '-', '~', ':',';','!', '*', '#', '$', '@'
};
const float eps = 0.001;
vector<Obj3D> scene;
float t = 0;
int columns = 1;
int rows = 1;

milliseconds start = duration_cast<milliseconds>(
	system_clock::now().time_since_epoch()
	);

Obj3D LoadObjFile(std::string fname) {
	ifstream objFile(fname);
	string line;

	string** vLines = new string * [5000000];
	string** vtLines = new string * [5000000];
	string** vnLines = new string * [5000000];
	string** fLines = new string * [5000000];

	int vnum = 0;
	int vtnum = 0;
	int vnnum = 0;
	int fnum = 0;

	while (getline(objFile, line)) {
		if (line.length() == 0) continue;
		//cout << "Proc line: " << line << endl;
		string* words = new string[6];
		
		
		int strPos = 0;
		int ind = 0;
		int substrStart = 0;
		while (strPos < line.length() && ind < 6) {
			char currChar = line.at(strPos);
			if (line.at(strPos) == ' ') {
				words[ind] = line.substr(substrStart, strPos - substrStart);
				ind++;
				substrStart = strPos+1;
			}
			strPos++;
		}
		if (ind < 6) {
			words[ind] = line.substr(substrStart, line.length() - substrStart);
		}

		if (words[0].compare("v") == 0) {
			vLines[vnum++] = words;
		}
		else if (words[0].compare("vt") == 0) {
			vtLines[vtnum++] = words;
		}
		else if (words[0].compare("vn") == 0) {
			vnLines[vnnum++] = words;
		}
		else if (words[0].compare("f") == 0) {
			fLines[fnum++] = words;
		}
		else {
			// skip other attributes for now
		}
	}

	Obj3D myObj(vnum, vtnum, vnnum, fnum);

	for (int i = 0; i < vnum; i++) {
		string* words = vLines[i];
		//arbitrary scale of 1/2
		Vec3 v(stof(words[1]), stof(words[2]), stof(words[3]));
		myObj.verts[i] = v;
	}

	for (int i = 0; i < vtnum; i++) {
		string* words = vtLines[i];
		Vec2 v(stof(words[1]), stof(words[2]));
		myObj.uvs[i] = v;
	}

	for (int i = 0; i < vnnum; i++) {
		string* words = vnLines[i];
		Vec3 v(stof(words[1]), stof(words[2]), stof(words[3]));
		myObj.norms[i] = v;
	}

	for (int i = 0; i < fnum; i++) {
		string* words = fLines[i];
		int nGon = 3;
		Face face;
		for (int j = 1; j < nGon + 1; j++) {
			string word = words[j];
			size_t pos = 0;
			string token;
			int k = 0;
			while ((pos = word.find("/")) != string::npos) {
				token = word.substr(0, pos);
				if (k == 0) {
					face.verts[j - 1] = stoi(token);
				}
				else if (k == 1 && sizeof(token) > 0) {
					face.uvs[j - 1] = stoi(token);
				}
				k++;
				word.erase(0, pos + 1);
			}
			face.norms[j - 1] = stoi(token);
		}

		myObj.faces[i] = face;
	}

	return myObj;
}

float PixelToScreenCoord(int pixelNum, int totalPixels)
{
	return 2 * (float)(pixelNum - totalPixels / 2) / (float)totalPixels;
}

char CharacterFromLuminance(float luminance) {
	int index = max(min(floor(luminance * sizeof(characters)), sizeof(characters) - 1), 0);
	return characters[index];
}

struct Ray {
	Vec3 origin;
	Vec3 dir;
};


Vec3 Avg(Vec3 a, Vec3 b, Vec3 c) {
	float x = (a.x + b.x + c.x) / 3;
	float y = (a.y + b.y + c.y) / 3;
	float z = (a.z + b.z + c.z) / 3;

	Vec3 ans(x, y, z);
	return ans;
}

void Cross(Vec3 a, Vec3 b, Vec3& out)
{
	/*Vec3 ans(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	return ans;*/

	out.x = a.y * b.z - a.z * b.y;
	out.y = a.z * b.x - a.x * b.z;
	out.z = a.x * b.y - a.y * b.x;
}

Vec3 RotX(Vec3 vec, float theta) {
	Vec3 out(
		vec.x,
		cos(theta) * vec.y - sin(theta) * vec.z,
		sin(theta) * vec.y + cos(theta) * vec.z
	);

	return out;
}
Vec3 RotY(Vec3 vec, float theta) {
	Vec3 out(
		cos(theta) * vec.x + sin(theta) * vec.z,
		vec.y,
		-sin(theta) * vec.x + cos(theta) * vec.z
	);
	return out;
}

Vec3 RotZ(Vec3 vec, float theta) {
	Vec3 out(
		cos(theta) * vec.x - sin(theta) * vec.y,
		sin(theta) * vec.x + cos(theta) * vec.y,
		vec.z
	);
	return out;
}

//sX, sY [-1, 1]
Ray CameraToRay(float sX, float sY) {
	Vec3 zero(0, 0, 0);
	Vec3 camWindow(0.0, 3.0, 10.5);
	float scaleX = 4;
	float scaleY = 2.2;

	float rotX = 0.0;
	float rotY = t / 2;
	float rotZ = 0;

	Vec3 pix(scaleX * sX, -scaleY * sY, 0);
	Vec3 pixCam = (camWindow)+pix;

	Vec3 transPix = RotX(pixCam, rotX);
	transPix = RotY(transPix, rotY);
	transPix = RotZ(transPix, rotZ);

	Vec3 transCam = RotX(camWindow, rotX);
	transCam = RotY(transCam, rotY);
	transCam = RotZ(transCam, rotZ);


	Ray ray;
	ray.origin = transPix;
	ray.dir = zero - transCam;
	ray.dir.Normalize();
	return ray;
}


Vec3 edge1, edge2, s, q, h, dir2;
float a, f, u, v, t2;
// see: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool TriangleIntersection(Ray ray, Vec3 v1, Vec3 v2, Vec3 v3, Vec3& intersectPoint) {
	edge1 = v2 - v1;
	edge2 = v3 - v1;

	Cross(ray.dir, edge2, h);
	a = edge1.Dot(h);
	if (a > -eps && a < eps) {
		return false;
	}

	f = 1 / a;
	s = ray.origin - v1;
	u = f * s.Dot(h);
	if (u < 0.0 || u > 1.0) {
		return false;
	}

	Cross(s, edge1, q);
	v = f * ray.dir.Dot(q);
	if (v < 0.0 || u + v > 1.0) {
		return false;
	}

	t2 = f * edge2.Dot(q);
	if (t2 > eps) {
		dir2 = ray.dir;
		dir2.ScalarMultiply(t2);
		intersectPoint = ray.origin + dir2;
		return true;
	}
	return false;
}

bool TraceRay(Ray ray, int& objectId, Face& face, Vec3& intersection) {
	bool hit = false;
	float minDepth = FLT_MAX;
	int minObject = -1;
	Face minFace;
	Vec3 minIntersect;

	for (int j = 0; j < scene.size(); j++) {
		Obj3D* obj = &(scene[j]);
		Vec3 intersect;

		for (int i = 0; i < (*obj).fnum; i++) {
			Face* face_ = &((*obj).faces[i]);
			if (TriangleIntersection(ray, (*obj).verts[(*face_).verts[0] - 1],
				(*obj).verts[(*face_).verts[1] - 1], (*obj).verts[(*face_).verts[2] - 1], intersect)) {
				hit = true;
				float depth = (ray.origin - intersect).Mag(); // depth culling
				if (depth < minDepth) {
					minDepth = depth;
					minIntersect = intersect;
					minFace = *face_;
					minObject = j;
				}

			}
		}
	}

	if (hit) {
		face = minFace;
		intersection = minIntersect;
		objectId = minObject;
	}

	return hit;
}

void RenderScreen(int columns, int rows) {
	int len = columns * rows;
	string out;
	out.resize(len);

	Vec3 lightRay(0.5, 2, 0); // points to center
	Face face;
	Vec3 intersect;
	int objectId = 0;

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			float sX = PixelToScreenCoord(x, columns);
			float sY = PixelToScreenCoord(y, rows);
			Ray rayCast = CameraToRay(sX, sY);

			float lum = 0;
			if (TraceRay(rayCast, objectId, face, intersect)) {
				// average normals
				Obj3D* obj__ = &(scene[objectId]);
				Vec3 n1 = (*obj__).norms[face.norms[0] - 1];
				Vec3 n2 = (*obj__).norms[face.norms[1] - 1];
				Vec3 n3 = (*obj__).norms[face.norms[2] - 1];
				//cout << n1.x << ", " << n1.y << ", " << n1.z << ", ";
				Vec3 avg = Avg(n1, n2, n3);
				//cout << avg.x << ", " << avg.y << ", " << avg.z <<", ";
				lum = lightRay.Dot(avg);
				lum = lum + 2;
				lum = lum / 4;
				//cout << lum << endl; 
				//lum = 1;
			}
			lum = max(0, min(lum, 1));

			char put = CharacterFromLuminance(lum);

			out[y * (columns)+x] = put;
		}

	}
	cout << out;
}

void RunTimer(std::function<void(void)> func, unsigned int interval)
{
	std::thread([func, interval]() {
		while (true)
		{
			func();
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
		}).detach();
}


void ClearScreen(int numChars)
{
	for (int i = 0; i < numChars; i++) {
		cout << "\b";
	}
}

void GraphicsLoop() {
	t = (float)(duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		) - start).count() / 1000.0;
	ClearScreen(columns * rows);
	//cout << "new frame";
	RenderScreen(columns, rows);
}

int main()
{
	Obj3D myObj = LoadObjFile("../Models/Suzanne.obj");
	scene.push_back(myObj);
	bool test = false;
	if (test) {
		cout << myObj.vnum << endl;
		cout << myObj.verts[0].x << ", " << myObj.verts[0].y << ", " << myObj.verts[0].z << endl;
		cout << myObj.fnum << endl;
		cout << myObj.faces[0].uvs[0] << endl;
	}
	else {


		CONSOLE_SCREEN_BUFFER_INFO csbi;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = max(csbi.srWindow.Right - csbi.srWindow.Left + 1, 1);
		rows = max(csbi.srWindow.Bottom - csbi.srWindow.Top + 1, 1);

		float dt = 1000 / 24;
		RunTimer(GraphicsLoop, round(dt));
	}

	while (true);

}

