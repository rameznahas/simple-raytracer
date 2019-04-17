#include "scene.h"
#include "glm/glm/glm.hpp"
#include <fstream>
#include <string>
#include <iostream>

/**
	Default constructor.

	Asks for the scene file path and saved the information parsed from the file.
*/
scene::scene() {
	std::cout << std::endl << "Scene file path (absolute path only): ";
	std::string scene_file;
	std::getline(std::cin, scene_file);

	std::ifstream file(scene_file);

	while (!file) {
		std::cout << "Incorrect path. Try again." << std::endl;
		std::cout << std::endl << "Scene file path (absolute path only): ";
		std::getline(std::cin, scene_file);
		file.open(scene_file);
	}
	
	set_directory(scene_file);

	int count;
	file >> count;

	std::string object_type;
	while (file >> object_type) {
		if (object_type == "camera") init_camera(file);
		else if (object_type == "plane") init_plane(file);
		else if (object_type == "sphere") init_sphere(file);
		else if (object_type == "mesh") init_mesh(file);
		else init_light(file);
	}
}

scene::~scene() {
	if (m_camera) {
		delete m_camera;
		m_camera = nullptr;
	}

	for (unsigned int i = 0; i < m_shapes.size(); i++) {
		delete m_shapes[i];
		m_shapes[i] = nullptr;
	}
}

/**
	Sets m_directory to be used for obj loading later on.
*/
void scene::set_directory(const std::string& abs_path) {
	m_directory = "";

	char sep = '/';

#ifdef _WIN32
	sep = '\\';
#endif

	size_t i = abs_path.rfind(sep, abs_path.length());
	if (i != std::string::npos) {
		m_directory = abs_path.substr(0, i) + sep;
	}
}

/**
	Initializes m_camera.

	@param ifstream the reference to the input file stream.
*/
void scene::init_camera(std::ifstream& ifstream) {
	std::string attribute;
	ifstream >> attribute;
	
	glm::vec3 position;
	ifstream >> position.x >> position.y >> position.z;

	ifstream >> attribute;

	float fov;
	ifstream >> fov;

	ifstream >> attribute;

	float focal_length;
	ifstream >> focal_length;

	ifstream >> attribute;

	float aspect_ratio;
	ifstream >> aspect_ratio;

	m_camera = new camera(position, fov, focal_length, aspect_ratio);
}

/**
	Initializes a plane and adds it to m_shapes.

	@param ifstream the reference to the input file stream.
*/
void scene::init_plane(std::ifstream& ifstream) {
	std::string attribute;
	ifstream >> attribute;

	glm::vec3 normal;
	ifstream >> normal.x >> normal.y >> normal.z;

	ifstream >> attribute;

	glm::vec3 point;
	ifstream >> point.x >> point.y >> point.z;

	ifstream >> attribute;

	glm::vec3 ambient;
	ifstream >> ambient.x >> ambient.y >> ambient.z;

	ifstream >> attribute;

	glm::vec3 diffuse;
	ifstream >> diffuse.x >> diffuse.y >> diffuse.z;

	ifstream >> attribute;

	glm::vec3 specular;
	ifstream >> specular.x >> specular.y >> specular.z;

	ifstream >> attribute;

	float shi;
	ifstream >> shi;

	shape::material mat(ambient, diffuse, specular, shi);
	m_shapes.push_back(new plane(normal, point, mat));
}

/**
	Initializes a sphere and adds it to m_shapes.

	@param ifstream the reference to the input file stream.
*/
void scene::init_sphere(std::ifstream& ifstream) {
	std::string attribute;
	ifstream >> attribute;

	glm::vec3 center;
	ifstream >> center.x >> center.y >> center.z;

	ifstream >> attribute;

	float radius;
	ifstream >> radius;

	ifstream >> attribute;

	glm::vec3 ambient;
	ifstream >> ambient.x >> ambient.y >> ambient.z;

	ifstream >> attribute;

	glm::vec3 diffuse;
	ifstream >> diffuse.x >> diffuse.y >> diffuse.z;

	ifstream >> attribute;

	glm::vec3 specular;
	ifstream >> specular.x >> specular.y >> specular.z;

	ifstream >> attribute;

	float shi;
	ifstream >> shi;

	shape::material mat(ambient, diffuse, specular, shi);
	m_shapes.push_back(new sphere(center, radius, mat));
}

/**
	Initializes a mesh and adds it to m_shapes.

	@param ifstream the reference to the input file stream.
*/
void scene::init_mesh(std::ifstream& ifstream) {
	std::string attribute;
	ifstream >> attribute;

	std::string file_name;
	ifstream >> file_name;

	ifstream >> attribute;

	glm::vec3 ambient;
	ifstream >> ambient.x >> ambient.y >> ambient.z;

	ifstream >> attribute;

	glm::vec3 diffuse;
	ifstream >> diffuse.x >> diffuse.y >> diffuse.z;

	ifstream >> attribute;

	glm::vec3 specular;
	ifstream >> specular.x >> specular.y >> specular.z;

	ifstream >> attribute;

	float shi;
	ifstream >> shi;

	shape::material mat(ambient, diffuse, specular, shi);
	m_shapes.push_back(new mesh((m_directory + file_name).c_str(), mat));
}

/**
	Initializes a light and adds it to m_lights.

	@param ifstream the reference to the input file stream.
*/
void scene::init_light(std::ifstream& ifstream) {
	std::string attribute;
	ifstream >> attribute;

	glm::vec3 position;
	ifstream >> position.x >> position.y >> position.z;

	ifstream >> attribute;

	glm::vec3 diffuse;
	ifstream >> diffuse.x >> diffuse.y >> diffuse.z;

	ifstream >> attribute;

	glm::vec3 specular;
	ifstream >> specular.x >> specular.y >> specular.z;

	m_lights.push_back(light(position, diffuse, specular));
}
