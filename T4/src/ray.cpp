#include "ray.h"

Ray::Ray()
{

}


Ray::Ray(double fov, int width, int height, float near, glm::vec3 eye, glm::vec3 center, glm::vec3 up, int x, int y)
{
    _width = width;
    _height = height;

    _f = near;
    _a = 2*_f*tan(glm::radians(fov/2));
    _b = _a*width/height;

    _ze = glm::normalize(eye - center);
    _xe = glm::normalize(glm::cross(up,_ze));
    _ye = glm::cross(_ze,_xe);

    _origin = eye;
    _direction = -_f*_ze + _a*( (float) ( y/_height - 0.5 ))*_ye + _b*( (float) (x/_width - 0.5))*_xe;
}

bool Ray::changeDirection(int x, int y)
{
    _direction = -_f*_ze + _a*( (float) ( (y/ (float) _height) - 0.5 ))*_ye + _b*( (float) ((x/ (float) _width) - 0.5))*_xe;
    return true;
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
