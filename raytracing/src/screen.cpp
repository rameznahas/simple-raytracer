#include "screen.h"

/**
	Parameterized constructor.

	Initializes the screen according to the camera properties.

	@param camera the camera of the scene.
	@param height [optional] specifies the desired screen height.
*/
screen::screen(camera camera, float height) : m_height(height) {
	float tan_fov = glm::tan(glm::radians(camera.m_fov * .5f));
	float z = camera.m_position.z - camera.m_focal_length;

	m_upper_left = glm::vec3(-camera.m_aspect_ratio * camera.m_focal_length * tan_fov, camera.m_focal_length * tan_fov, z);
	m_upper_right = glm::vec3(camera.m_aspect_ratio * camera.m_focal_length * tan_fov, camera.m_focal_length * tan_fov, z);
	m_lower_left = glm::vec3(-camera.m_aspect_ratio * camera.m_focal_length * tan_fov, camera.m_focal_length * -tan_fov, z);
	m_lower_right = glm::vec3(camera.m_aspect_ratio * camera.m_focal_length * tan_fov, camera.m_focal_length * -tan_fov, z);
	m_center = glm::vec3(camera.m_position.x, camera.m_position.y, z);
	
	if (m_height < 0) m_height = glm::distance(m_upper_left, m_lower_left);
	m_width = m_height * camera.m_aspect_ratio;

	m_slope_x = 2.f * camera.m_aspect_ratio * camera.m_focal_length * tan_fov / m_width;
	m_intersection_x = -camera.m_aspect_ratio * camera.m_focal_length * tan_fov;
	m_slope_y = 2.f * camera.m_focal_length * tan_fov / m_height;
	m_intersection_y = -camera.m_focal_length * tan_fov;
}

screen& screen::operator=(const screen& rhs) {
	if (&rhs != this) {
		m_upper_left = rhs.m_upper_left;
		m_upper_right = rhs.m_upper_right;
		m_lower_left = rhs.m_lower_left;
		m_lower_right = rhs.m_lower_right;
		m_center = rhs.m_center;
		m_width = rhs.m_width;
		m_height = rhs.m_height;
		m_slope_x = rhs.m_slope_x;
		m_intersection_x = rhs.m_intersection_x;
		m_slope_y = rhs.m_slope_y;
		m_intersection_y = rhs.m_intersection_y;
	}
	return *this;
}

/**
	Transforms the pixel (u, v) to world coordinates (x, y, z).

	@param u u-coordinate of the pixel.
	@param v v-coordinates of the pixel.
	@return glm::vec3 the equivalent 3D world space point.
*/
glm::vec3 screen::to_world(float u, float v) {
	float x = m_slope_x * u + m_intersection_x;
	float y = -(m_slope_y * v + m_intersection_y);
	return glm::vec3(x, y, m_center.z);
}