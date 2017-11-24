#include "ray.h"

Ray::Ray()
{

}


Ray::Ray(double fov, int width, int height, int near, glm::vec3 eye, glm::vec3 center, glm::vec3 up, int x, int y)
{
    float f, a, b;
    f = near;
    a = 2*f*tan(fov/2);
    b = a*width/height;

    glm::vec3 xe, ye, ze;
    ze = glm::normalize(eye - center);
    xe = glm::normalize(glm::cross(up,ze));
    ye = glm::cross(ze,xe);

    _origin = eye;
    _direction = -f*ze + a*( (float) ( y/height - 0.5 ))*ye + b*( (float) (x/width - 0.5))*xe;

}

Ray::Ray(int near, int width, int height, int x, int y, float a, float b, glm::vec3 eye, glm::vec3 xe, glm::vec3 ye, glm::vec3 ze)
{
    float f = near;
    _origin = eye;
    _direction = -f*ze + a*( (float) ( y/height - 0.5 ))*ye + b*( (float) (x/width - 0.5))*xe;
}

glm::vec3 Ray::evaluateRay(float t)
{
    return _origin + t*_direction;
}

glm::vec3 Ray::origin()
{
    return _origin;
}

glm::vec3 Ray::direction()
{
    return _direction;
}
