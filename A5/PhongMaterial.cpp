// Termm--Fall 2024

#include "PhongMaterial.hpp"

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

PhongMaterial::~PhongMaterial()
{}
