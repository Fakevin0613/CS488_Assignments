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

bool Cylinder::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    glm::vec3 oc = ray.origin - center;

    glm::vec3 d_perp = ray.direction - glm::dot(ray.direction, axis) * axis;
    glm::vec3 oc_perp = oc - glm::dot(oc, axis) * axis;

    double a = glm::dot(d_perp, d_perp);
    double b = 2.0 * glm::dot(d_perp, oc_perp);
    double c = glm::dot(oc_perp, oc_perp) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;

    double sqrt_discriminant = sqrt(discriminant);
    double t0 = (-b - sqrt_discriminant) / (2.0 * a);
    double t1 = (-b + sqrt_discriminant) / (2.0 * a);

    // Check for valid intersections within the interval
    for (double t : {t0, t1}) {
        if (t < interval.x || t > interval.y) continue;
        glm::vec3 hit_point = ray.origin + t * ray.direction;
        double projection = glm::dot(hit_point - center, axis);

        if (projection >= 0 && projection <= height) {
            photon.t = t;
            photon.hit = true;
            photon.hitPoint = hit_point;
            photon.normal = glm::normalize(hit_point - center - projection * axis);
            return true;
        }
    }

    return false;
}

Cylinder::~Cylinder()
{
}

bool Cone::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    // Cosine squared of the angle for simplification
    double cos_theta = cos(angle);
    double cos2 = cos_theta * cos_theta;

    glm::vec3 v_apex_ray = ray.origin - center;
    double axis_dot_dir = glm::dot(ray.direction, axis);
    double axis_dot_v = glm::dot(v_apex_ray, axis);

    // Coefficients for the quadratic equation
    glm::vec3 d_perp = ray.direction - axis_dot_dir * axis;
    glm::vec3 v_perp = v_apex_ray - axis_dot_v * axis;

    double a = glm::dot(d_perp, d_perp) - cos2 * (axis_dot_dir * axis_dot_dir);
    double b = 2.0 * (glm::dot(d_perp, v_perp) - cos2 * axis_dot_dir * axis_dot_v);
    double c = glm::dot(v_perp, v_perp) - cos2 * (axis_dot_v * axis_dot_v);

    // Solve the quadratic equation
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;  // No intersection

    double sqrt_discriminant = sqrt(discriminant);
    double t0 = (-b - sqrt_discriminant) / (2.0 * a);
    double t1 = (-b + sqrt_discriminant) / (2.0 * a);

    // Check valid intersections within the interval
    for (double t : {t0, t1}) {
        if (t < interval.x || t > interval.y) continue;

        glm::vec3 hit_point = ray.origin + t * ray.direction;
        double projection = glm::dot(hit_point - center, axis);

        // Ensure the intersection is within the finite cone height
        if (projection >= 0 && projection <= height) {
            photon.t = t;
            photon.hitPoint = hit_point;

            // Compute the normal at the hit point
            glm::vec3 normal = glm::normalize(hit_point - center - projection * tan(angle) * axis);
            photon.normal = glm::normalize(normal);

            return true;
        }
    }

    return false;
}

Cone::~Cone()
{
}
