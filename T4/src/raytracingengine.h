#ifndef RAYTRACINGENGINE_H
#define RAYTRACINGENGINE_H

#include <vector>
#include <cmath>
#include <QImage>

#include "camera.h"
#include "sphere.h"
#include "ray.h"
#include "triangle.h"

class RayTracingEngine
{
public:
    RayTracingEngine();

    bool insertTriangles(std::vector<glm::vec3> vertex);

    QImage generateRayTracingImage();

private:
    // Equations of the form ax^2 + bx + c = 0
    static std::vector<float> solveQuadraticEquation (float a, float b, float c);

    // Intersection functions return empty vector if there is no interception
    std::vector<glm::vec3> raySphereIntersection (Sphere s, Ray r);

    std::vector<glm::vec3> rayTriangleIntersection (Triangle t, Ray r);

    // Scene objects
    std::vector<Triangle> _triangles;
    std::vector<Sphere> _spheres;
    Camera camera;

};

#endif // RAYTRACINGENGINE_H
