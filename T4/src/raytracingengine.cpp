#include "raytracingengine.h"

RayTracingEngine::RayTracingEngine()
{

}

bool RayTracingEngine::insertTriangles(std::vector<glm::vec3> vertexes)
{
    for (unsigned int i = 0; i < vertexes.size(); i+=3)
    {
        _triangles.push_back(Triangle(vertexes[i], vertexes[i+1], vertexes[i+2]));
    }
}

QImage RayTracingEngine::generateRayTracingImage()
{
    printf("Working YAY BITCHEEEEES");
}

std::vector<float> RayTracingEngine::solveQuadraticEquation (float a, float b, float c)
{
    std::vector<float> roots;
    float delta = 4 * a * c;
    if (delta == 0)
        roots.push_back( -b/(2*a) );
    else if (delta > 0)
    {
        roots.push_back( (-b - sqrt(delta))/(2*a) );
        roots.push_back( (-b + sqrt(delta))/(2*a) );
    }
    return roots;
}

std::vector<glm::vec3> RayTracingEngine::raySphereIntersection (Sphere s, Ray r)
{
    // Building quadratic equation that describes the intersection between ray and sphere
    float a, b, c;
    a = glm::dot(r.direction(), r.direction());
    b = glm::dot(2.0f*r.direction(), r.origin() - s.center());
    c = glm::dot(r.origin() - s.center(),r.origin() - s.center()) - pow(s.radius(),2);

    std::vector<float> intersectionT = solveQuadraticEquation(a,b,c);

    // Translating the parameters found into actual points
    std::vector<glm::vec3> intersectionPoints;
    intersectionPoints.push_back(r.evaluateRay(intersectionT[0]));
    intersectionPoints.push_back(r.evaluateRay(intersectionT[1]));
    return intersectionPoints;
}

std::vector<glm::vec3> rayTriangleIntersection (Triangle t, Ray r)
{
    std::vector<glm::vec3> intersectionPoints;

    // Making sure the triangle and ray are not parallel
    if (glm::dot(r.direction(),t.normal()))
    {
        // Finding parameter where r intercepts t plane
        float intersectionT = glm::dot(t.p1() - r.origin(), t.normal())/glm::dot(r.direction(), t.normal());
        glm::vec3 intersectionPoint = r.evaluateRay(intersectionT);

        if (t.isInsideTriangle(intersectionPoint))
            intersectionPoints.push_back(intersectionPoint);
    }

    return intersectionPoints;
}
