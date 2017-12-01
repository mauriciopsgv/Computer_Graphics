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
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 t1, glm::vec2 t2, glm::vec2 t3, int textureId);
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n);

    bool isInsideTriangle(glm::vec3 point);

    glm::vec3 p1();
    glm::vec3 p2();
    glm::vec3 p3();
    glm::vec2 t1();
    glm::vec2 t2();
    glm::vec2 t3();
    int textureId();
    glm::vec3 normal();

    glm::vec2 textureCoordinatesAnyPoint(glm::vec3 point);

private:

    void baricentricInterpolation(glm::vec3 point, float &lambda1, float &lambda2, float &lambda3);

    float triangleArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

    // Points
    glm::vec3 _p1;
    glm::vec3 _p2;
    glm::vec3 _p3;

    // Edges
    glm::vec3 _e12;
    glm::vec3 _e23;
    glm::vec3 _e31;
    glm::vec3 _e13;

    // Texture Coordinates
    glm::vec2 _t1;
    glm::vec2 _t2;
    glm::vec2 _t3;

    // Normal
    glm::vec3 _n;

    int _textureId;
};

#endif // TRIANGLE_H
