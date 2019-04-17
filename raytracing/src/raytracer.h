/**
	The raytracer class that traces rays in a scene to creater a render of the scene.
*/
#pragma once
#include "scene.h"
#include "screen.h"
#include "ray.h"
#include "CImg-2.5.5/CImg.h"
#define ANTI_ALIASING_SAMPLE 32
#define SHADOW_BIAS 0.01f

class raytracer {
public:
	raytracer(scene& scene, screen& screen, cimg_library::CImg<float>& image);
	void run();

private:
	void render();
	void write_pixel(unsigned int u, unsigned int v, glm::vec3 color);
	glm::vec3 trace(ray ray_);
	glm::vec3 get_color(hit hit_, light light_);

	scene& m_scene;
	screen& m_screen;
	cimg_library::CImg<float>& m_image;
};
