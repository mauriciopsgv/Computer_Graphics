#include "raytracingengine.h"

#include <iostream>

RayTracingEngine::RayTracingEngine()
{

}

bool RayTracingEngine::setCamera(Camera camera)
{
    _camera = camera;
    return true;
}

bool RayTracingEngine::insertTriangles(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoordinates, int textureId, int materialId)
{
    for (unsigned int i = 0; i < vertices.size(); i+=3)
    {
        _triangles.push_back(Triangle(vertices[i], vertices[i+1], vertices[i+2], textureCoordinates[i], textureCoordinates[i+1], textureCoordinates[i+2], textureId, materialId));
    }
    return true;
}

int RayTracingEngine::insertTexture(std::string texturePath)
{
    QImage texImage = QImage(texturePath.c_str());
    _textures.push_back(texImage);
    return _textures.size() - 1;
}

int RayTracingEngine::insertMaterial(Material material)
{
    _materials.push_back(material);
    return _materials.size() -1;
}

QImage RayTracingEngine::generateRayTracingImage()
{
    // TODO: lighting
    // TODO: texture color interpolation
    QImage outputImage = QImage(_camera.width, _camera.height, QImage::Format_RGB32);
    float nearestIntersection, intersectionT;
    Triangle nearestTriangle = Triangle();
    Ray currentRay = Ray(_camera.fovy, _camera.width, _camera.height, _camera.zNear, _camera.eye, _camera.at, _camera.up, 0, 0);
    for (int y = 0; y < outputImage.height(); y++)
    {
        for (int x = 0; x < outputImage.width(); x++)
        {
            nearestIntersection = FLT_MAX;
            currentRay.changeDirection(x,y);
            for (unsigned int i = 0; i < _triangles.size(); i++)
            {
                intersectionT = rayTriangleIntersectionT(_triangles[i], currentRay);
                if (intersectionT != -1 && intersectionT < nearestIntersection)
                {
                    nearestIntersection = intersectionT;
                    nearestTriangle = _triangles[i];
                }
            }
            if (nearestIntersection > -1 && nearestIntersection < FLT_MAX)
            {
                glm::vec2 texCoord = nearestTriangle.textureCoordinatesAnyPoint(currentRay.evaluateRay(nearestIntersection));
                texCoord[0] = texCoord[0]*_textures[nearestTriangle.textureId()].width();
                texCoord[1] = _textures[nearestTriangle.textureId()].height() - texCoord[1]*_textures[nearestTriangle.textureId()].height();
                QColor pixColor = _textures[nearestTriangle.textureId()].pixelColor(texCoord[0], texCoord[1]);
                outputImage.setPixelColor(x, outputImage.height() - y - 1, pixColor);
            }
            else
                outputImage.setPixelColor(x, outputImage.height() - y - 1, QColor(0,0,0));
        }
    }
    return outputImage;
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

std::vector<glm::vec3> RayTracingEngine::rayTriangleIntersectionPoint (Triangle t, Ray r)
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

float RayTracingEngine::rayTriangleIntersectionT (Triangle t, Ray r)
{
    float intersectionT;
    // Making sure the triangle and ray are not parallel
    if (glm::dot(r.direction(),t.normal()))
    {
        // Finding parameter where r intercepts t plane
        intersectionT = glm::dot(t.p1() - r.origin(), t.normal())/glm::dot(r.direction(), t.normal());
        glm::vec3 intersectionPoint = r.evaluateRay(intersectionT);
        if (!t.isInsideTriangle(intersectionPoint))
            return -1;
    }
    return intersectionT;
}
