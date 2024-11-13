#ifndef BoundingBox_hpp
#define BoundingBox_hpp
#include <glm/glm.hpp>
#include "Ray.hpp"
#include "Photon.hpp"
class BoundingBox {
public:
    glm::vec3 min; // Minimum corner
    glm::vec3 max; // Maximum corner

    BoundingBox();
    ~BoundingBox();
    BoundingBox(glm::vec3 minCorner, glm::vec3 maxCorner);

    // Ray-AABB intersection test
    bool intersect(Ray& ray, glm::vec2 interval, Photon& photon);
};

#endif