#pragma once
#include "glm/glm/glm.hpp"

struct light {
	light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular)
		:
		m_position(position),
		m_diffuse(diffuse),
		m_specular(specular)
	{}

	glm::vec3 m_position;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
};
