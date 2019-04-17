/**
	The scene class that holds all the scene information.
*/
#pragma once
#include "shapes.h"
#include "camera.h"
#include "light.h"
#include <vector>
#include <string>

class scene {
public:
	scene();
	~scene();

	camera* m_camera;
	std::vector<light> m_lights;
	std::vector<shape*> m_shapes;

private:
	std::string m_directory;
	void set_directory(const std::string& abs_path);

	void init_camera(std::ifstream& ifstream);
	void init_plane(std::ifstream& ifstream);
	void init_sphere(std::ifstream& ifstream);
	void init_mesh(std::ifstream& ifstream);
	void init_light(std::ifstream& ifstream);
};

