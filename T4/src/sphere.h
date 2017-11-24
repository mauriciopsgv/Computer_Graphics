#ifndef SPHERE_H
#define SPHERE_H

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

class Sphere
{
public:
    Sphere();
    Sphere(glm::vec3 center, float radius);

    glm::vec3 center();
    float radius();

private:
    glm::vec3 _center;
    float _radius;
};

#endif // SPHERE_H
