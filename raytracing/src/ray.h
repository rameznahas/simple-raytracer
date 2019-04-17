#pragma once
#include "glm/glm/glm.hpp"
#include "shapes.h"
#include <iostream>

/**
	The hit struct holds the intersection information of a ray and a shape.
*/
struct hit {
	operator bool();
	float m_t;
	glm::vec3 m_position;
	glm::vec3 m_normal;
	shape::material m_material;

private:
	bool m_hit;
	friend ray;
};

/**
	The ray class.
*/
class ray {
public:
	ray(glm::vec3 origin, glm::vec3 target);
	void set_hit(float t, glm::vec3 position, glm::vec3 normal, shape::material mat);
	glm::vec3 point_at(float t);

	glm::vec3 m_origin;
	glm::vec3 m_direction;
	hit m_hit;
};
