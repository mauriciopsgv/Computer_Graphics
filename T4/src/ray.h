#ifndef RAY_H
#define RAY_H

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

class Ray
{
public:
    Ray();
    Ray(double fov, int width, int height, int near, glm::vec3 eye, glm::vec3 center, glm::vec3 up, int x, int y);
    Ray(int near, int width, int height, int x, int y, float a, float b, glm::vec3 eye, glm::vec3 xe, glm::vec3 ye, glm::vec3 ze);

    glm::vec3 evaluateRay(float t);

    glm::vec3 origin();
    glm::vec3 direction();

private:
    glm::vec3 _origin;
    glm::vec3 _direction;

};

#endif // RAY_H
