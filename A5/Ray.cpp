#include "Ray.hpp"
using namespace std;

Ray::Ray(){
    origin = glm::vec3(0);
    direction = glm::vec3(0, 0, 1);
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction){
    this->origin = origin;
    this->direction = direction;
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, double time){
    this->origin = origin;
    this->direction = direction;
    this->time = time;
}

glm::vec3 Ray::at(float t) const {
    return origin + t * direction;
}

