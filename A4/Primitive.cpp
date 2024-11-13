// Termm--Fall 2024
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include "Ray.hpp"
#include "Primitive.hpp"
#include "polyroots.hpp"

Primitive::~Primitive()
{
}

bool Primitive::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    return false;
}

Sphere::Sphere()
{
    m_nonhierSphere = new NonhierSphere(glm::vec3(0), 1);
}

bool Sphere::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    return m_nonhierSphere->intersect(ray, interval, photon);
}

Sphere::~Sphere()
{
    delete m_nonhierSphere;
}

Cube::Cube()
{
    m_nonhierBox = new NonhierBox(glm::vec3(0), 1);
}

bool Cube::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    return m_nonhierBox->intersect(ray, interval, photon);
}

Cube::~Cube()
{
    delete m_nonhierBox;
}

bool NonhierSphere::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    double roots[2];
    size_t n_roots = quadraticRoots(a, b, c, roots);

    float t = 0;
    if (n_roots == 0) {
        photon.hit = false;
        return false;
    } else if (n_roots == 1){
        t = roots[0];
    } else {
        if (roots[0] < roots[1]) {
            t = roots[0];
        } else {
            t = roots[1];
        }
    }

    if (t <= interval[0] || t >= interval[1]) {
        photon.hit = false;
        return false;
    }

    photon.t = t;
    photon.hitPoint = ray.at(t);
    photon.normal = photon.hitPoint - center;
    photon.hit = true;
    return true;
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierBox::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    glm::vec3 minCornor = m_pos;
    glm::vec3 maxCornor = m_pos + glm::vec3(m_size);

    glm::vec3 invDir = 1.0f / ray.direction;
    // similar to bounding volume intersection
    glm::vec3 t0 = (minCornor - ray.origin) * invDir;
    glm::vec3 t1 = (maxCornor - ray.origin) * invDir;
    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);
    float tNear = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    float tFar = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    if (tNear > tFar || tFar < interval.x || tNear > interval.y) {
        photon.hit = false;
        return false;  // No valid intersection
    }

    photon.hit = true;
    photon.t = tNear >= interval.x ? tNear : tFar;
    photon.hitPoint = ray.at(photon.t);

    if (photon.t == t0.x || photon.t == t1.x) photon.normal = glm::vec3((photon.t == t0.x ? -1 : 1), 0, 0);
    else if (photon.t == t0.y || photon.t == t1.y) photon.normal = glm::vec3(0, (photon.t == t0.y ? -1 : 1), 0);
    else if (photon.t == t0.z || photon.t == t1.z) photon.normal = glm::vec3(0, 0, (photon.t == t0.z ? -1 : 1));

    return true;
}

NonhierBox::~NonhierBox()
{
}
