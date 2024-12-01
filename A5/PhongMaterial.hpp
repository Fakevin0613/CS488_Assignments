// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Image.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double transparency, double reflection, double refraction);
  virtual ~PhongMaterial();
  glm::vec3 diffuse();
  glm::vec3 diffuse(const glm::vec2& uv);
  glm::vec3 specular();
  double shininess();
  double transparency();
  double reflection();
  double refraction();

  void setTexture(const Image& texture);

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  double m_shininess;
  double m_transparency;
  double m_reflection;
  double m_refraction;

  bool has_texture = false;
  Image m_texture;
};
