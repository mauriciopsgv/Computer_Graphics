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
    Ray(double fov, int width, int height, float near, glm::vec3 eye, glm::vec3 center, glm::vec3 up, int x, int y);

    bool changeDirection(int x, int y);

    glm::vec3 evaluateRay(float t);

    glm::vec3 origin();
    glm::vec3 direction();

private:
    int _width;
    int _height;

    float _f;
    float _a;
    float _b;

    glm::vec3 _xe;
    glm::vec3 _ye;
    glm::vec3 _ze;

    glm::vec3 _origin;
    glm::vec3 _direction;


};

#endif // RAY_H
