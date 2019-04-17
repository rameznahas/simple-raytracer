#include "ray.h"

/**
	Parameterized constructor.

	At instantiation, there is no hit, so time of intersection m_t is set to FLT_MAX.

	@param origin the origin of the ray.
	@param target where the ray should go.
*/
ray::ray(glm::vec3 origin, glm::vec3 target) : m_origin(origin) {
	m_direction = glm::normalize(target - origin);
	m_hit.m_t = FLT_MAX;
	m_hit.m_hit = false;
}

/**
	Saves the relevant information of the intersection of the ray and a shape.

	@param t time of intersection.
	@param position the intersection position in world space.
	@param normal the normal at the intersection position.
	@param mat the material of the shape at that hit point.
*/
void ray::set_hit(float t, glm::vec3 position, glm::vec3 normal, shape::material mat) {
	// if true, intersection is either behind the camera or behind a previous intersection.
	// disregard this intersection.
	if (t <= 0 || t > m_hit.m_t) return; 

	m_hit.m_t = t;
	m_hit.m_position = position;
	m_hit.m_normal = glm::normalize(normal);
	m_hit.m_material = mat;
	m_hit.m_hit = true;
}

/**
	Computes the position of the ray at time t.

	@param t time parameter.
	@return glm::vec3 the position at time t.
*/
glm::vec3 ray::point_at(float t) {
	return glm::vec3(m_origin + m_direction * t);
}

hit::operator bool() {
	return m_hit;
}