#include "BoundingBox.hpp"
using namespace std;

BoundingBox::BoundingBox() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

BoundingBox::BoundingBox(glm::vec3 minCorner, glm::vec3 maxCorner) : min(minCorner), max(maxCorner) {}

BoundingBox::~BoundingBox() {}

// Bounding box algorhtm is inspired from https://tavianator.com/fast-branchless-raybounding-box-intersections/
// and https://gist.github.com/DomNomNom/46bb1ce47f68d255fd5d
bool BoundingBox::intersect(Ray& ray, glm::vec2 interval, Photon& photon)
{
    float t_min = interval[0];
    float t_max = interval[1];

    for (int i = 0; i < 3; ++i) {
        if (std::abs(ray.direction[i]) < 1e-8f) { // Parallel ray
            if (ray.origin[i] < min[i] || ray.origin[i] > max[i]) return false;
            continue;
        }

        float invD = 1.0f / ray.direction[i];
        float t0 = (min[i] - ray.origin[i]) * invD;
        float t1 = (max[i] - ray.origin[i]) * invD;

        if (invD < 0.0f) std::swap(t0, t1);

        t_min = std::max(t_min, t0);
        t_max = std::min(t_max, t1);

        if (t_max <= t_min) return false;
    }

    photon.t = t_min;
    photon.hitPoint = ray.at(t_min);
    photon.normal = glm::vec3(0.0f);
    photon.material = nullptr;

    return true;
}