// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "Photon.hpp"
#include <cmath>
#include <vector>

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

class MovingSphere : public Primitive {
public:
  MovingSphere(const glm::vec3& pos, double radius, const glm::vec3& velocity)
    : center(pos), radius(radius), velocity(velocity) 
  {
  }
  virtual ~MovingSphere();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;

private:
  glm::vec3 center;
  double radius;
  glm::vec3 velocity;
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

class Cylinder : public Primitive {
public:
  Cylinder(glm::vec3& center, glm::vec3& axis, double radius, double height)
    : center(center), axis(glm::normalize(axis)), radius(radius), height(height) 
  {
  }
  virtual ~Cylinder();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;
private:
  glm::vec3 center;
  glm::vec3 axis;
  double radius;
  double height;
};

class Cone : public Primitive {
public:
  Cone(glm::vec3& center, glm::vec3& axis, double angle, double height)
    : center(center), axis(glm::normalize(axis)), angle(angle), height(height) 
  {
  }
  virtual ~Cone();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;
private:
  glm::vec3 center;
  glm::vec3 axis;
  double angle;
  double height;
};

class Torus : public Primitive {
public:
  Torus(glm::vec3& center, double minorRadius, double majorRadius)
    : center(center), minorRadius(minorRadius), majorRadius(majorRadius) 
  {
  }
  virtual ~Torus();
  virtual bool intersect(Ray& ray, glm::vec2 interval, Photon& photon) override;
private:
  glm::vec3 center;
  double minorRadius;
  double majorRadius;
};