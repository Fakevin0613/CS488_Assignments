#ifndef Photon_hpp
#define Photon_hpp
// #pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Material.hpp"

class Photon {  
public:
    glm::vec3 hitPoint;
    glm::vec3 normal;
    double t;
    bool hit;
    Material *material;
    glm::vec2 uv;
};

#endif