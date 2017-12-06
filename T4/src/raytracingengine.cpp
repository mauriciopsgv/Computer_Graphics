#include "raytracingengine.h"

#include <glm/gtx/rotate_vector.hpp>

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

int RayTracingEngine::insertLight(glm::vec3 lightPosition)
{
    _lights.push_back(lightPosition);
    return _lights.size() - 1;
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
    #pragma omp parallel for
    for (int y = 0; y < outputImage.height(); y++)
    {
        for (int x = 0; x < outputImage.width(); x++)
        {
            float nearestIntersection, intersectionT;
            Triangle nearestTriangle = Triangle();
            Ray currentRay = Ray(_camera.fovy, _camera.width, _camera.height, _camera.zNear, _camera.eye, _camera.at, _camera.up, 0,0);
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
                glm::vec3 intersectionPoint = currentRay.evaluateRay(nearestIntersection);
                glm::vec2 texCoord = nearestTriangle.textureCoordinatesAnyPoint(intersectionPoint);
                texCoord[0] = texCoord[0]*_textures[nearestTriangle.textureId()].width();
                texCoord[1] = _textures[nearestTriangle.textureId()].height() - texCoord[1]*_textures[nearestTriangle.textureId()].height();
                QColor texColor = _textures[nearestTriangle.textureId()].pixelColor(texCoord[0], texCoord[1]);
                glm::vec3 texColorV = glm::vec3(texColor.red()/255.f, texColor.green()/255.f, texColor.blue()/255.f);

                //ambient lighting
                Material currentMaterial = _materials[nearestTriangle.materialId()];
                glm::vec3 ambientComponent = currentMaterial.ambient * texColorV;

                //diffuse and specullar lighting
                glm::vec3 L;
                glm::vec3 diffuseComponent = glm::vec3(0,0,0);
                glm::vec3 specularComponent = glm::vec3(0,0,0);
                for (unsigned int i = 0; i < _lights.size(); i++)
                {
                    L = glm::normalize(_lights[i] - intersectionPoint);
                    float angleDiff = glm::dot(L,nearestTriangle.normal());
                    if (angleDiff > 0)
                    {
                        //diffuse lighting

                        diffuseComponent[0] += angleDiff * currentMaterial.diffuse[0] * texColorV[0];
                        diffuseComponent[1] += angleDiff * currentMaterial.diffuse[1] * texColorV[1];
                        diffuseComponent[2] += angleDiff * currentMaterial.diffuse[2] * texColorV[2];

                        //specular lighting

                        glm::vec3 V = glm::normalize(_camera.eye - intersectionPoint);
                        glm::vec3 H = glm::normalize(L + V);
                        float ispec = glm::dot(nearestTriangle.normal(), H);
                        float ispecForRealBitches = 0;
                        if (ispec > 0)
                        {
                             ispecForRealBitches = pow(ispec, currentMaterial.shininess);
                        }

//                        glm::vec3 r = glm::normalize(glm::rotate(L, glm::radians(180.f), nearestTriangle.normal()));
//                        glm::vec3 r = 2*(glm::dot(nearestTriangle.normal(), L))*nearestTriangle.normal() - L;
//                        glm::vec3 r =  2*glm::dot(L, nearestTriangle.normal())*nearestTriangle.normal() - L;
//                        float reflectionAngleDiff = pow( glm::max(glm::dot(r,V), 0.f), currentMaterial.shininess);
//                        float reflectionAngleDiff = pow( glm::max(glm::dot(nearestTriangle.normal(), (L + V) ), 0.f), currentMaterial.shininess);
                        specularComponent[0] += ispecForRealBitches * currentMaterial.specular[0];
                        specularComponent[1] += ispecForRealBitches * currentMaterial.specular[1];
                        specularComponent[2] += ispecForRealBitches * currentMaterial.specular[2];
                    }
                }

                //specular lighting

                glm::vec3 pixColor = ambientComponent + diffuseComponent + specularComponent;
                pixColor[0] = pixColor[0] > 1 ? 1.f : pixColor[0];
                pixColor[1] = pixColor[1] > 1 ? 1.f : pixColor[1];
                pixColor[2] = pixColor[2] > 1 ? 1.f : pixColor[2];

                outputImage.setPixelColor(x, outputImage.height() - y - 1, QColor(255*pixColor[0], 255*pixColor[1], 255*pixColor[2]));
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
