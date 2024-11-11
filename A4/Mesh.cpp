// Termm--Fall 2024

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

bool Mesh::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord) {
	bool hit = false;
    float closestT = interval.y;

    for (const auto& tri : m_faces) {
        // Retrieve vertices of the triangle
        const glm::vec3& v0 = m_vertices[tri.v1];
        const glm::vec3& v1 = m_vertices[tri.v2];
        const glm::vec3& v2 = m_vertices[tri.v3];

        // Möller-Trumbore triangle intersection
        const float EPSILON = 1e-8;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 h = glm::cross(ray.direction, edge2);
        float a = glm::dot(edge1, h);
        if (a > -EPSILON && a < EPSILON) continue; // Ray is parallel to the triangle

        float f = 1.0f / a;
        glm::vec3 s = ray.origin - v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) continue;

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(ray.direction, q);
        if (v < 0.0f || u + v > 1.0f) continue;

        float t = f * glm::dot(edge2, q);
        if (t > EPSILON && t < closestT) { // Valid intersection
            closestT = t;
            hit = true;

            // Update HitRecord
            hitRecord.t = t;
            hitRecord.hitPoint = ray.at(t);
            hitRecord.normal = glm::cross(edge1, edge2); // Triangle normal
			hitRecord.material = nullptr;
            // Ensure the normal points against the ray direction
            if (glm::dot(hitRecord.normal, ray.direction) > 0) {
                hitRecord.normal = -hitRecord.normal;
            }
        }
    }

    return hit;
}