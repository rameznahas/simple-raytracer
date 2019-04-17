#include "shapes.h"
#include "ray.h"
////using tiny obj loader for obj loading////
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
/////////////////////////////////////////////

/**
	Parameterized constructor.
	
	@param ambient the ambient property of the shape.
	@param diffuse the diffuse property of the shape.
	@param specular the specular property of the shape.
	@param shi the shininess coefficient of the shape.
*/
shape::material::material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shi)
	:
	m_ambient(ambient),
	m_diffuse(diffuse),
	m_specular(specular),
	m_shi(shi)
{}

/**
	Parameterized constructor.

	Also initializes m_surface_normal using the positions of the triangle vertices.

	@param pos0 the position of the first vertex.
	@param pos1 the position of the second vertex.
	@param pos2 the position of the third vertex.
	@param mat the material of the triangle.
*/
triangle::triangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2, const material& mat) {
	m_vertices[0].m_pos = pos0;
	m_vertices[1].m_pos = pos1;
	m_vertices[2].m_pos = pos2;
	m_material = mat;

	m_surface_normal = glm::cross(pos1 - pos0, pos2 - pos0);
	m_vertices[0].m_norm = m_surface_normal;
	m_vertices[1].m_norm = m_surface_normal;
	m_vertices[2].m_norm = m_surface_normal;
}

/**
	Computes the ray-triangle intersection of front-facing triangles.

	Uses MOLLER-TRUMBORE algorithm to compute the triangle intersection, because
	it is faster than finding the intersection with the plane of the triangle
	first, and then checking if the intersection is within the triangle.

	@param ray a pointer to the current ray.
*/
void triangle::intersection(ray* ray) {
	if (glm::dot(ray->m_direction, m_surface_normal) > 0.f) return; // is a back face

	const float EPSILON = 0.0000001f;
	glm::vec3 e1, e2, s;
	e1 = (m_vertices[1].m_pos - m_vertices[0].m_pos);
	e2 = (m_vertices[2].m_pos - m_vertices[0].m_pos);

	glm::vec3 p = glm::cross(ray->m_direction, e2);
	float d = glm::dot(p, e1);

	//ray is parallel
	if (abs(d) < EPSILON) return;

	s = (ray->m_origin - m_vertices[0].m_pos);

	float alpha = glm::dot(p, s) / d;

	if (alpha < 0.f || alpha > 1.f) return;

	glm::vec3 q = glm::cross(s, e1);
	float beta = glm::dot(ray->m_direction, q) / d;

	if ((beta < 0.f) || (alpha + beta > 1.0f)) return;

	float t = glm::dot(e2, q) / d;
	if (t <= EPSILON) return;

	glm::vec3 intersection = ray->point_at(t);
	glm::vec3 norm = glm::normalize((1.f - alpha - beta) * m_vertices[0].m_norm + alpha * m_vertices[1].m_norm + beta * m_vertices[2].m_norm);

	ray->set_hit(t, intersection, norm, m_material);
}

/**
	Parameterized constructor.

	Loads the mesh located in the .obj file_name, and processes all the
	vertex positions to create triangles and store them in m_triangles.
	Disregards normals of the file. See get_smooth_normals() for more info.

	@param file_name the .obj file to load.
	@param mat the material of the mesh.
*/
mesh::mesh(const char* file_name, const material& mat) {
	m_material = mat;

	// obj loading and processing is mix of my original work  
	// and sample code found on https://github.com/syoyo/tinyobjloader
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file_name, NULL, false);
	
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!loaded) exit(EXIT_FAILURE);

	for (size_t s = 0; s < shapes.size(); s++) {
		m_triangle_count += shapes[s].mesh.num_face_vertices.size();
	}
	m_triangles = new triangle*[m_triangle_count];
	
	unsigned int triangle_index = 0;
	glm::vec3 positions[triangle::VERTEX_COUNT];
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			
			// Loop over vertices in the face.
			for (int v = 0; v < fv; v++) {
				// access to vertex positions
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				positions[v] = glm::vec3(
					attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]);
			}
			m_triangles[triangle_index++] = new triangle(positions[0], positions[1], positions[2], m_material);
			index_offset += fv;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////

	get_smooth_normals();
}

mesh::~mesh() {
	if (!m_triangles) return;

	for (unsigned int i = 0; i < m_triangle_count; i++) {
		delete m_triangles[i];
		m_triangles[i] = nullptr;
	}
	delete m_triangles;
	m_triangles = nullptr;
}

/**
	Computes the ray-mesh intersection.

	Trivial  for mesh shapes. Just get the intersection with each triangle
	in m_triangles.

	@param ray a pointer to the current ray.
*/
void mesh::intersection(ray* ray) {
	for (unsigned int i = 0; i < m_triangle_count; i++) {
		m_triangles[i]->intersection(ray);
	}
}

/**
	This method computes vertex normals for smooth shading.

	It loops over every vertex in m_triangles, and for every vertex,
	computes its normal as the sum of the m_surface_normal of the triangle
	that the vertex belongs to. It doesn't not average the result since
	m_surface_normal is not a unit vector. So implicitely, the weight of a triangle
	is determined by the length of its m_surface_normal. Hence the vertex normal needs
	to only be normalized once its normal has been computed.
*/
void mesh::get_smooth_normals() {
	// to keep track of duplicate normals since if two triangles are coplanar
	// and share a vertex, we do not want to add the same normal twice.
	std::vector<glm::vec3> vertex_normals; 
	const float EPSILON = 0.000001f; // for float equality testing

	// for each triangle
	for (unsigned int i = 0; i < m_triangle_count; i++) {
		// for each vertex in the triangle
		for (unsigned int j = 0; j < triangle::VERTEX_COUNT; j++) {
			glm::vec3 pos = m_triangles[i]->m_vertices[j].m_pos;
			glm::vec3& norm = m_triangles[i]->m_vertices[j].m_norm;
			vertex_normals.push_back(norm);
			// for every other triangle
			for (unsigned int k = i + 1; k < m_triangle_count; k++) {
				// for each vertex in the other triangle
				for (unsigned int l = 0; l < triangle::VERTEX_COUNT; l++) {
					glm::vec3 pos_ = m_triangles[k]->m_vertices[l].m_pos;
					if (pos_ == pos) { // same vertex
						glm::vec3 norm_ = m_triangles[k]->m_vertices[l].m_norm;
						bool dup_norm = false;
						for (unsigned int z = 0; z < vertex_normals.size(); z++) {
							glm::vec3 test_norm = vertex_normals[z];
							// if true, we have a coplanar triangle with shared vertex.
							// Disregard the normal since it was already added from previous triangle.
							if (glm::abs(norm_.x - test_norm.x) <= EPSILON &&
								glm::abs(norm_.y - test_norm.y) <= EPSILON &&
								glm::abs(norm_.z - test_norm.z) <= EPSILON) {
								dup_norm = true;
								break;
							}
						}
						if (!dup_norm) {
							norm += norm_;
							vertex_normals.push_back(norm_);
						}
						break; // no need to check other vertices of this triangle
					}
				}
			}
			vertex_normals.clear();
			norm = glm::normalize(norm);
		}
	}
}

/**
	Parameterized constructor.

	@param center the sphere center point.
	@param radius the sphere radius.
	@param mat the sphere material.
*/
sphere::sphere(glm::vec3 center, float radius, const material& mat) 
	:
	m_center(center), 
	m_radius(radius) 
{
	m_material = mat;
}

/**
	Computes the ray-sphere intersection.

	@param ray a pointer to the current ray.
*/
void sphere::intersection(ray* ray) {
	float t0, t1;

	glm::vec3 ray_to_center = ray->m_origin - m_center;

	float a = 1;
	float b = 2 * glm::dot(ray_to_center, ray->m_direction);
	float c = glm::pow(ray_to_center.x, 2) + glm::pow(ray_to_center.y, 2) + glm::pow(ray_to_center.z, 2) - glm::pow(m_radius, 2);
	
	float delta = glm::pow(b, 2) - 4 * a * c;

	if (delta < 0) return; // no intersection

	t0 = (-b - glm::sqrt(delta)) / (2 * a);
	t1 = (-b + glm::sqrt(delta)) / (2 * a);

	float t = glm::min(t0, t1);

	glm::vec3 intersection = ray->point_at(t);
	glm::vec3 normal(intersection - m_center);
	ray->set_hit(t, intersection, normal, m_material);
}

/**
	Parameterized constructor.

	@param normal the plane normal.
	@param point a point on the plane.
	@param mat the plane material.
*/
plane::plane(glm::vec3 normal, glm::vec3 point, const material& mat) 
	:
	m_normal(normal), 
	m_point(point) 
{
	m_material = mat;
}

/**
	Computes the ray-plane intersection.

	@param ray a pointer to the current ray.
*/
void plane::intersection(ray* ray) {
	float d = glm::dot(-m_normal, m_point);
	float numerator = -(glm::dot(m_normal, ray->m_origin) + d);
	float denominator = glm::dot(m_normal, ray->m_direction);

	if (denominator != 0.f) {
		float t = numerator / denominator;

		glm::vec3 intersection = ray->point_at(t);
		ray->set_hit(t, intersection, m_normal, m_material);
	} // else ray and plane are parallel
}