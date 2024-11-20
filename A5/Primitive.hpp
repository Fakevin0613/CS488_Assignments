// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "Photon.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon);
};

class Sphere : public Primitive {
  Primitive *m_nonhierSphere;
public:
  Sphere();
  virtual ~Sphere();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;
};

class Cube : public Primitive {
  Primitive *m_nonhierBox;
public:
  Cube();
  virtual ~Cube();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : center(pos), radius(radius) 
  {
  }
  virtual ~NonhierSphere();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;

private:
  glm::vec3 center;
  double radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size) 
  {
  }
  virtual ~NonhierBox();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;

private:
  glm::vec3 m_pos;
  double m_size;
};
