#pragma once
#include "glm/glm/glm.hpp"
#define XY_NORM glm::vec3(0.f, 0.f, 1.f)
#define XZ_NORM glm::vec3(0.f, 1.f, 0.f)
#define YZ_NORM glm::vec3(1.f, 0.f, 0.f)

class ray;

/**
	The vertex struct holds the most basic vertex information.
*/
struct vertex {
	glm::vec3 m_pos;
	glm::vec3 m_norm;
};

/**
	The shape abstract class is the base class of all shapes in the scene.

	Every shape in the scene must derive from this class.
*/
class shape {
public:
	virtual ~shape() {}
	virtual void intersection(ray* ray) = 0;

	/**
		The material struct hold the material information of a shape.
	*/
	struct material {
		material() = default;
		material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shi);

		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;
		float m_shi;
	} m_material;
};

/**
	The triangle class which is the basis of all meshes.
*/
class triangle : public shape {
public:
	triangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2, const material& mat);
	virtual void intersection(ray* ray);

	static const unsigned int VERTEX_COUNT = 3;

	vertex m_vertices[VERTEX_COUNT];

private:
	glm::vec3 m_surface_normal;
};

/**
	The mesh class.
*/
class mesh : public shape {
public:
	mesh(const char* file_name, const material& mat);
	virtual ~mesh();
	virtual void intersection(ray* ray);

private:
	void get_smooth_normals();

	triangle** m_triangles;
	unsigned int m_triangle_count = 0;
};

/**
	The sphere class.
*/
class sphere : public shape {
public:
	sphere(glm::vec3 center, float radius, const material& mat);
	virtual void intersection(ray* ray);

private:
	glm::vec3 m_center;
	float m_radius;
};

/**
	The plane class.
*/
class plane : public shape {
public:
	plane(glm::vec3 normal, glm::vec3 point, const material& mat);
	virtual void intersection(ray* ray);

private:
	glm::vec3 m_normal;
	glm::vec3 m_point;
};
