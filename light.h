#include <vector>
#include <cstring>
#include <iostream>
#include "glut.h"
using namespace std;

class light
{
public:
	float position[4];
	float specular[4];
	float diffuse[4];
	float ambient[4];
	
	light();
	light(char* light_str);
};

class environment_light
{
public:
	float environment_ambient[3];

	environment_light(char* light_str);
};

class lights
{
public:
	vector<light> light_list;
	vector<environment_light> env_light;

	lights();
	lights(const char* light_file);
	~lights();

	void lighting();
	void environment_lighting();
};