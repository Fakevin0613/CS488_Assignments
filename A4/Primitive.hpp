// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "HitRecord.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord);
};

class Sphere : public Primitive {
  Primitive *m_nonhierSphere;
public:
  Sphere();
  virtual ~Sphere();
  virtual bool intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord) override;
};

class Cube : public Primitive {
  Primitive *m_nonhierBox;
public:
  Cube();
  virtual ~Cube();
  virtual bool intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord) override;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius) 
  {
  }
  virtual ~NonhierSphere();
  virtual bool intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord) override;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual bool intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord) override;

private:
  glm::vec3 m_pos;
  double m_size;
};
