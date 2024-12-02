// Termm--Fall 2024
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include "Ray.hpp"
#include "Primitive.hpp"
#include "polyroots.hpp"
using namespace std;

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
    glm::vec3 localPoint = glm::normalize(photon.hitPoint - center);

    // Calculate spherical coordinates
    // We want the texture to wrap around horizontally (U coordinate)
    // and from top to bottom vertically (V coordinate)
    // float u = 0.5f + (atan2(localPoint.z, localPoint.x) / (2.0f * M_PI));
    // float v = 0.5f - (asin(localPoint.y) / M_PI);

    float u = atan2(-localPoint.z, localPoint.x) / (2.0f * M_PI);  // Negative sign to fix mirroring
    float v = acos(-localPoint.y) / M_PI;

    // Ensure UV coordinates are in [0,1] range
    photon.uv = glm::vec2(u, v);
    return true;
}

NonhierSphere::~NonhierSphere()
{
}

bool MovingSphere::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    glm::vec3 center = this->center + velocity * ray.time;
    // cout << "time: " << ray.time << endl;
    // cout << "velocity: " << glm::to_string(velocity) << endl;
    // cout << "center: " << glm::to_string(center) << endl;
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

MovingSphere::~MovingSphere()
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

    // Calculate normal and UV coordinates based on which face was hit
    glm::vec3 localPoint = (photon.hitPoint - m_pos) / m_size; // Normalize to [0,1] range
    
    if (photon.t == t0.x || photon.t == t1.x) {
        photon.normal = glm::vec3((photon.t == t0.x ? -1 : 1), 0, 0);
        photon.uv = glm::vec2(localPoint.z, localPoint.y); // Map Z to U, Y to V for X faces
    }
    else if (photon.t == t0.y || photon.t == t1.y) {
        photon.normal = glm::vec3(0, (photon.t == t0.y ? -1 : 1), 0);
        photon.uv = glm::vec2(localPoint.x, localPoint.z); // Map X to U, Z to V for Y faces
    }
    else if (photon.t == t0.z || photon.t == t1.z) {
        photon.normal = glm::vec3(0, 0, (photon.t == t0.z ? -1 : 1));
        photon.uv = glm::vec2(localPoint.x, localPoint.y); // Map X to U, Y to V for Z faces
    }

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
            
            // Calculate UV coordinates
            // U coordinate: angle around cylinder (0 to 1)
            // V coordinate: height along cylinder (0 to 1)
            glm::vec3 localPoint = hit_point - center;
            float phi = atan2(glm::dot(localPoint, glm::cross(axis, glm::vec3(0, 0, 1))),
                            glm::dot(localPoint, glm::cross(axis, glm::cross(axis, glm::vec3(0, 0, 1)))));
            float u = (phi + M_PI) / (2.0f * M_PI);
            float v = projection / height;
            
            photon.uv = glm::vec2(u, v);
            return true;
        }
    }

    // Check caps
    double t_cap = -1;
    glm::vec3 normal_cap;
    
    // Bottom cap (at center)
    double t_bottom = -glm::dot(oc, axis) / glm::dot(ray.direction, axis);
    if (t_bottom >= interval.x && t_bottom <= interval.y) {
        glm::vec3 hit_point = ray.origin + t_bottom * ray.direction;
        glm::vec3 radial_vec = hit_point - center;
        if (glm::length(radial_vec - glm::dot(radial_vec, axis) * axis) <= radius) {
            t_cap = t_bottom;
            normal_cap = -axis;
        }
    }
    
    // Top cap (at center + height * axis)
    glm::vec3 oc_top = ray.origin - (center + height * axis);
    double t_top = -glm::dot(oc_top, axis) / glm::dot(ray.direction, axis);
    if (t_top >= interval.x && t_top <= interval.y) {
        glm::vec3 hit_point = ray.origin + t_top * ray.direction;
        glm::vec3 radial_vec = hit_point - (center + height * axis);
        if (glm::length(radial_vec - glm::dot(radial_vec, axis) * axis) <= radius) {
            if (t_cap < 0 || t_top < t_cap) {
                t_cap = t_top;
                normal_cap = axis;
            }
        }
    }
    
    if (t_cap >= 0) {
        photon.t = t_cap;
        photon.hit = true;
        photon.hitPoint = ray.origin + t_cap * ray.direction;
        photon.normal = normal_cap;
        
        // UV coordinates for caps (polar coordinates)
        glm::vec3 localPoint = photon.hitPoint - (center + (normal_cap.y > 0 ? height : 0.0f) * axis);
        float r = glm::length(localPoint - glm::dot(localPoint, axis) * axis) / radius;
        float theta = atan2(glm::dot(localPoint, glm::cross(axis, glm::vec3(0, 0, 1))),
                          glm::dot(localPoint, glm::cross(axis, glm::cross(axis, glm::vec3(0, 0, 1)))));
        photon.uv = glm::vec2((theta + M_PI) / (2.0f * M_PI), r);
        return true;
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

// Torus intersection
// The quartic equation provided in polyroot.cpp is not reliable
// Therefore, we use the idea from https://www.shadertoy.com/view/4sBGDy
bool Torus::intersect(Ray& ray, glm::vec2 interval, Photon& photon) {
    glm::dvec3 ro = ray.origin - center;
    glm::dvec3 rd = ray.direction;

    double majorRadius2 = majorRadius * majorRadius;
    double minorRadius2 = minorRadius * minorRadius;

    // Calculate coefficients for the quartic equation
    double m = glm::dot(ro, ro);  // |ro|^2
    double n = glm::dot(ro, rd);  // ro · rd

    double k = (m - minorRadius2 - majorRadius2) * 0.5;
    double k3 = n;
    double k2 = n * n + majorRadius2 * rd.z * rd.z + k;
    double k1 = k * n + majorRadius2 * ro.z * rd.z;
    double k0 = k * k + majorRadius2 * ro.z * ro.z - majorRadius2 * minorRadius2;

    // Prevent |c1| from being too close to zero
    if (std::abs(k3 * (k3 * k3 - k2) + k1) < 1e-4) {
        std::swap(k1, k3);
        k0 = 1.0 / k0;
        k1 *= k0;
        k2 *= k0;
        k3 *= k0;
    }

    // Compute quartic coefficients
    double c2 = 2.0 * k2 - 3.0 * k3 * k3;
    double c1 = k3 * (k3 * k3 - k2) + k1;
    double c0 = k3 * (k3 * (-3.0 * k3 * k3 + 4.0 * k2) - 8.0 * k1) + 4.0 * k0;

    c2 /= 3.0;
    c1 *= 2.0;
    c0 /= 3.0;

    double Q = c2 * c2 + c0;
    double R = 3.0 * c0 * c2 - c2 * c2 * c2 - c1 * c1;

    double h = R * R - Q * Q * Q;
    double z = 0.0;

    if (h < 0.0) {
        // Four roots
        double sQ = std::sqrt(Q);
        z = 2.0 * sQ * std::cos(std::acos(R / (sQ * Q)) / 3.0);
    } else {
        // Two roots
        double sQ = std::pow(std::sqrt(h) + std::abs(R), 1.0 / 3.0);
        z = std::copysign(sQ + Q / sQ, R);
    }
    z = c2 - z;

    double d1 = z - 3.0 * c2;
    double d2 = z * z - 3.0 * c0;

    if (std::abs(d1) < 1e-4) {
        if (d2 < 0.0) return false;
        d2 = std::sqrt(d2);
    } else {
        if (d1 < 0.0) return false;
        d1 = std::sqrt(d1 * 0.5);
        d2 = c1 / d1;
    }

    double result = std::numeric_limits<double>::infinity();

    // Compute potential roots
    h = d1 * d1 - z + d2;
    if (h > 0.0) {
        h = std::sqrt(h);
        double t1 = -d1 - h - k3;
        double t2 = -d1 + h - k3;
        if (t1 > 0.0) result = t1;
        if (t2 > 0.0) result = std::min(result, t2);
    }

    h = d1 * d1 - z - d2;
    if (h > 0.0) {
        h = std::sqrt(h);
        double t1 = d1 - h - k3;
        double t2 = d1 + h - k3;
        if (t1 > 0.0) result = std::min(result, t1);
        if (t2 > 0.0) result = std::min(result, t2);
    }

    // Check if no valid intersections
    if (result == std::numeric_limits<double>::infinity()) return false;
    if (result < interval.x || result > interval.y) return false;
    glm::dvec3 hitPoint = ray.origin + result * ray.direction;

    // Compute normal
    glm::dvec3 grad = hitPoint * (glm::dot(hitPoint, hitPoint) - minorRadius2 - majorRadius2);
    grad.z += 2.0 * majorRadius2 * hitPoint.z;
    glm::dvec3 normal = glm::normalize(grad);

    // Update photon information
    photon.t = result;
    photon.hitPoint = hitPoint;
    photon.normal = normal;

    return true;
}

Torus::~Torus()
{
    
}
bool Plane::intersect(Ray& ray, glm::vec2 interval, Photon& photon) {
    // Compute the denominator to check if the ray is parallel to the plane
    float denom = glm::dot(normal, ray.direction);
    if (fabs(denom) < 1e-6) { // Parallel case: No intersection
        photon.hit = false;
        return false;
    }

    // Compute the intersection parameter t
    float t = glm::dot(center - ray.origin, normal) / denom;

    // Check if t is within the interval
    if (t <= interval[0] || t >= interval[1]) {
        photon.hit = false;
        return false;
    }

    // Update the photon properties
    photon.t = t;
    photon.hitPoint = ray.at(t); // Compute the intersection point

    // Check if the intersection point is within the plane's bounds
    glm::vec3 localPoint = photon.hitPoint - center;
    float u_proj = glm::dot(localPoint, u);
    float v_proj = glm::dot(localPoint, v);

    if (fabs(u_proj) > width / 2 || fabs(v_proj) > height / 2) {
        photon.hit = false;
        return false;
    }

    photon.normal = normal;
    photon.hit = true;

    // Map local coordinates directly to texture space with 90-degree rotation
    // We swap u and v coordinates and invert one of them for 90-degree rotation
    photon.uv = glm::vec2(
        1.0f - (v_proj + height/2) / height,  // Inverted v coordinate becomes u
        (u_proj + width/2) / width            // u coordinate becomes v
    );

    return true;
}

Plane::~Plane()
{

}