/**
	The screen class that holds the screen information.
*/
#pragma once
#include "glm/glm/glm.hpp"
#include "camera.h"

class screen {
public:
	screen(camera camera, float height = -1.f);
	screen& operator=(const screen& rhs);
	glm::vec3 to_world(float u, float v);

	float m_width;
	float m_height;

private:
	glm::vec3 m_upper_left;
	glm::vec3 m_upper_right;
	glm::vec3 m_lower_left;
	glm::vec3 m_lower_right;
	glm::vec3 m_center;

	float m_slope_x;
	float m_intersection_x;
	float m_slope_y;
	float m_intersection_y;
};