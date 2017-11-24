#include "sphere.h"

Sphere::Sphere()
{

}

Sphere::Sphere(glm::vec3 center, float radius)
{
    _center = center;
    _radius = radius;
}

glm::vec3 Sphere::center()
{
    return _center;
}

float Sphere::radius()
{
    return _radius;
}
