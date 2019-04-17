#pragma once
#include "glm/glm/glm.hpp"

struct camera {
	camera(glm::vec3 position, float fov, float focal_length, float aspect_ratio)
		:
		m_position(position),
		m_fov(fov),
		m_focal_length(focal_length),
		m_aspect_ratio(aspect_ratio)
	{}

	glm::vec3 m_position;
	float m_fov;
	float m_focal_length;
	float m_aspect_ratio;
};
