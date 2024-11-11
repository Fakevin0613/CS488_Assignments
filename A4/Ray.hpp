#pragma once
// #ifndef Ray_hpp
// #define Ray_hpp

#include <glm/glm.hpp>

class Ray {
public:
    glm::vec3 origin;      // Starting point of the ray
    glm::vec3 direction;   // Direction of the ray (should be normalized)

    Ray();
    Ray(const glm::vec3& origin, const glm::vec3& direction);

    // Function to get a point along the ray at distance t from the origin
    glm::vec3 at(float t) const;
};

// #endif