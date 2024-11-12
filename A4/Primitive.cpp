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

bool Primitive::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
    return false;
}

Sphere::Sphere()
{
    m_nonhierSphere = new NonhierSphere(glm::vec3(0), 1);
}

bool Sphere::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
    return m_nonhierSphere->intersect(ray, interval, hitRecord);
}

Sphere::~Sphere()
{
    delete m_nonhierSphere;
}

Cube::Cube()
{
    m_nonhierBox = new NonhierBox(glm::vec3(0), 1);
}

bool Cube::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
    return m_nonhierBox->intersect(ray, interval, hitRecord);
}

Cube::~Cube()
{
    delete m_nonhierBox;
}

bool NonhierSphere::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    double roots[2];
    size_t n_roots = quadraticRoots(a, b, c, roots);

    float t = 0;
    if (n_roots == 0) {
        hitRecord.hit = false;
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
        hitRecord.hit = false;
        return false;
    }

    hitRecord.t = t;
    hitRecord.hitPoint = ray.at(t);
    hitRecord.normal = hitRecord.hitPoint - center;
    hitRecord.hit = true;
    return true;
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierBox::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
    glm::vec3 minCornor = m_pos;
    glm::vec3 maxCornor = m_pos + glm::vec3(m_size);

    glm::vec3 invDir = 1.0f / ray.direction;

    glm::vec3 t0 = (minCornor - ray.origin) * invDir;
    glm::vec3 t1 = (maxCornor - ray.origin) * invDir;
    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);
    float tNear = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    float tFar = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    if (tNear > tFar || tFar < interval.x || tNear > interval.y) {
        hitRecord.hit = false;
        return false;  // No valid intersection
    }

    hitRecord.hit = true;
    hitRecord.t = tNear >= interval.x ? tNear : tFar;
    hitRecord.hitPoint = ray.at(hitRecord.t);

    if (hitRecord.t == t0.x || hitRecord.t == t1.x) hitRecord.normal = glm::vec3((hitRecord.t == t0.x ? -1 : 1), 0, 0);
    else if (hitRecord.t == t0.y || hitRecord.t == t1.y) hitRecord.normal = glm::vec3(0, (hitRecord.t == t0.y ? -1 : 1), 0);
    else if (hitRecord.t == t0.z || hitRecord.t == t1.z) hitRecord.normal = glm::vec3(0, 0, (hitRecord.t == t0.z ? -1 : 1));

    return true;

    // return m_mesh->intersect(ray, interval, hitRecord);
}

NonhierBox::~NonhierBox()
{
}
