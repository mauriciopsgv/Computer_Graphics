#ifndef TRIANGLE_H
#define TRIANGLE_H

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

class Triangle
{
public:
    Triangle();
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n);

    bool isInsideTriangle(glm::vec3 point);

    glm::vec3 p1();
    glm::vec3 p2();
    glm::vec3 p3();
    glm::vec3 normal();

private:
    // Points
    glm::vec3 _p1;
    glm::vec3 _p2;
    glm::vec3 _p3;

    // Edges
    glm::vec3 _e12;
    glm::vec3 _e23;
    glm::vec3 _e31;
    glm::vec3 _e13;

    glm::vec3 _n;
};

#endif // TRIANGLE_H
