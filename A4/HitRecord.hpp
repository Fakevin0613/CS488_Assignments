#ifndef HitRecord_hpp
#define HitRecord_hpp
// #pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Material.hpp"

class HitRecord {  
public:
    glm::vec3 hitPoint;
    glm::vec3 normal;
    double t;
    bool hit;
    Material *material;
};

#endif