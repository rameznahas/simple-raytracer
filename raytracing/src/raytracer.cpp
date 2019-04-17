#include "raytracer.h"
#include "ray.h"
#include <random>

/**
	Parameterized constructor.

	@param scene a reference to the scene to render.
	@param screen a reference to screen through which rays will be traced.
	@param image a reference to the window where the render should be displayed.
*/
raytracer::raytracer(scene& scene, screen& screen, cimg_library::CImg<float>& image) 
	: 
	m_scene(scene), 
	m_screen(screen), 
	m_image(image) 
{}

/**
	The starting point of the raytracer class.

	This method traces multiple rays (anti-aliasing) for every pixel of m_screen. It then
	computes compute the color at the point of intersection (if any) and saves that color
	in m_image. Once done, it renders the result.
*/
void raytracer::run() {
	glm::vec3 COP(m_scene.m_camera->m_position);
	unsigned int height = (unsigned int)m_screen.m_height;
	unsigned int width = (unsigned int)m_screen.m_width;
	unsigned int pixel_count = width * height;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.f, 1.f);
	for (unsigned int i = 0; i < pixel_count; i++) {
		glm::vec3 color(0.f);
		unsigned int u = i % width;
		unsigned int v = i / width;
		for (unsigned int j = 0; j < ANTI_ALIASING_SAMPLE; j++) {
			float rand = dist(gen);
			glm::vec3 target = m_screen.to_world(u + rand, v + rand);
			color += trace(ray(COP, target));
		}
		color /= ANTI_ALIASING_SAMPLE;
		write_pixel(u, v, color);
	}

	render();
}

/**
	This method traces rays in the scene.

	This is done by checking for an intersection between ray_ and every shape in m_scene.
	If there is a hit, sends a shadow ray to every light in the scene to determine if the
	hit is in shadows or not. Computes the color accordingly.

	@param ray_ the ray to trace.
	@return glm::vec3 the color of the pixel.
*/
glm::vec3 raytracer::trace(ray ray_) {
	glm::vec3 color(0.f);

	for (unsigned int i = 0; i < m_scene.m_shapes.size(); i++) {
		m_scene.m_shapes[i]->intersection(&ray_);
	}
	// if true, there is a hit, so cast shadow rays to determine if the intersection
	// point is obstructed by another shape or not.
	if (ray_.m_hit) {
		for (light light_ : m_scene.m_lights) {
			ray shadow_ray(ray_.m_hit.m_position + ray_.m_hit.m_normal * SHADOW_BIAS, light_.m_position);
			unsigned int i = 0;

			while (!shadow_ray.m_hit && i < m_scene.m_shapes.size()) {
				m_scene.m_shapes[i]->intersection(&shadow_ray);
				i++;
			}

			if (!shadow_ray.m_hit) {
				color += get_color(ray_.m_hit, light_);
			}
		}
		color += ray_.m_hit.m_material.m_ambient;
	}
	return color;
}

/**
	Computes the color at the intersection point hit_.

	Uses phong shading to determine the color.
	
	@param hit_ the information of the ray intersection point.
	@param light_ the light.
	@return glm::vec3 the color at hit_ point.
*/
glm::vec3 raytracer::get_color(hit hit_, light light_) {
	glm::vec3 light_dir = glm::normalize(light_.m_position - hit_.m_position);
	float dot_diff = glm::max(glm::dot(light_dir, hit_.m_normal), 0.f);
	glm::vec3 diffuse = hit_.m_material.m_diffuse * light_.m_diffuse * dot_diff;

	glm::vec3 view_dir = glm::normalize(m_scene.m_camera->m_position - hit_.m_position);
	glm::vec3 reflection = glm::reflect(-light_dir, hit_.m_normal);
	float dot_spec = glm::pow(glm::max(glm::dot(view_dir, reflection), 0.f), hit_.m_material.m_shi);
	glm::vec3 specular = hit_.m_material.m_specular * light_.m_specular * dot_spec;

	return diffuse + specular;
}

/**
	Writes the color at pixel (u, v).
*/
void raytracer::write_pixel(unsigned int u, unsigned int v, glm::vec3 color) {
	m_image(u, v, 0) = glm::clamp(color.x * 255.f, 0.f, 255.f);
	m_image(u, v, 1) = glm::clamp(color.y * 255.f, 0.f, 255.f);
	m_image(u, v, 2) = glm::clamp(color.z * 255.f, 0.f, 255.f);
}

/**
	Renders the scene.
*/
void raytracer::render() {
	m_image.save("test.bmp");
	cimg_library::CImgDisplay main_disp(m_image, "Raytracer");
	while (!main_disp.is_closed()) {
		main_disp.wait();
	}
}