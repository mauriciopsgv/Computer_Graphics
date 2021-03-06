#ifndef RAYTRACINGENGINE_H
#define RAYTRACINGENGINE_H

#include <vector>
#include <cmath>
#include <QImage>

#include "camera.h"
#include "material.h"
#include "sphere.h"
#include "ray.h"
#include "triangle.h"

class RayTracingEngine
{
public:
    RayTracingEngine();

    bool setCamera(Camera camera);

    bool insertTriangles(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoordinates, int textureId, int materialId);

    int insertTexture(std::string texturePath);

    int insertLight(glm::vec3 lightPosition);

    int insertMaterial(Material material);

    QImage generateRayTracingImage();

private:
    // Equations of the form ax^2 + bx + c = 0
    static std::vector<float> solveQuadraticEquation (float a, float b, float c);

    // Intersection functions return empty vector if there is no interception
    std::vector<glm::vec3> raySphereIntersection (Sphere s, Ray r);

    std::vector<glm::vec3> rayTriangleIntersectionPoint (Triangle t, Ray r);
    float rayTriangleIntersectionT (Triangle t, Ray r);

    // Scene objects
    std::vector<Triangle> _triangles;
    std::vector<Sphere> _spheres;
    std::vector<QImage> _textures;
    std::vector<Material> _materials;
    Camera _camera;
    std::vector<glm::vec3> _lights;

    // Texture Stuff
    int LoadTexture(std::string texturePath);

};

#endif // RAYTRACINGENGINE_H
