// Termm--Fall 2024

#include "PhongMaterial.hpp"
#include <iostream>
using namespace std;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{}

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double transparency, double reflection, double refraction )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_transparency(transparency)
	, m_reflection(reflection)
	, m_refraction(refraction)
{}

glm::vec3 PhongMaterial::diffuse() {
	return m_kd;
}

int clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

glm::vec3 PhongMaterial::diffuse(const glm::vec2& uv) {
    if (!has_texture) {
        return m_kd; // Return default diffuse color if no texture
    }

    // Get texture dimensions
    int tex_width = m_texture.width();
    int tex_height = m_texture.height();

    // Get UV coordinates
    float u = uv[0];
    float v = uv[1];

    // Convert to texture coordinates with wrapping/repeating behavior
    int i = static_cast<int>(std::fmod(u * tex_width, tex_width));
    if (i < 0) i += tex_width;  // Handle negative values

    int j = static_cast<int>(std::fmod((1.0f - v) * tex_height, tex_height));
    if (j < 0) j += tex_height;  // Handle negative values

    // std::cout << "Texture dims: " << tex_width << "x" << tex_height << std::endl;
    // std::cout << "Sampled coords: i=" << i << ", j=" << j << std::endl; 



    return glm::vec3(
        m_texture(i, j, 0),
        m_texture(i, j, 1),
        m_texture(i, j, 2)
    );

    // // Get texture dimensions
    // int tex_width = m_texture.width();
    // int tex_height = m_texture.height();

    // // Get UV coordinates and handle wrapping
    // float u = std::fmod(uv[0], 1.0f);
    // float v = std::fmod(1.0f - uv[1], 1.0f);  // Flip v coordinate
    // if (u < 0.0f) u += 1.0f;
    // if (v < 0.0f) v += 1.0f;

    // // Convert to texture space with interpolation factors
    // float di = u * (tex_width - 1);
    // float dj = v * (tex_height - 1);

    // // Get integer pixel coordinates
    // int i = static_cast<int>(di);
    // int j = static_cast<int>(dj);

    // // Get the next pixel coordinates with wrapping
    // int i1 = (i + 1 >= tex_width) ? 0 : i + 1;
    // int j1 = (j + 1 >= tex_height) ? 0 : j + 1;

    // // Get interpolation factors
    // float alpha = di - i;
    // float beta = dj - j;

    // // Bilinear interpolation
    // return (1.0f - alpha) * (1.0f - beta) * glm::vec3(m_texture(i, j, 0), m_texture(i, j, 1), m_texture(i, j, 2)) +
    //     (1.0f - alpha) * beta * glm::vec3(m_texture(i, j1, 0), m_texture(i, j1, 1), m_texture(i, j1, 2)) +
    //     alpha * (1.0f - beta) * glm::vec3(m_texture(i1, j, 0), m_texture(i1, j, 1), m_texture(i1, j, 2)) +
    //     alpha * beta * glm::vec3(m_texture(i1, j1, 0), m_texture(i1, j1, 1), m_texture(i1, j1, 2));
}

glm::vec3 PhongMaterial::specular() {
	return m_ks;
}

double PhongMaterial::shininess() {
	return m_shininess;
}

double PhongMaterial::transparency() {
	return m_transparency;
}

double PhongMaterial::reflection() {
	return m_reflection;
}

double PhongMaterial::refraction() {
	return m_refraction;
}

void PhongMaterial::setTexture(const Image& texture) {
	m_texture = texture;
	has_texture = true;
}

PhongMaterial::~PhongMaterial()
{}
