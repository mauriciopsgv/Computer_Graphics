#ifndef MATERIAL_H
#define MATERIAL_H

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

struct Material {
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   float shininess;
};

#endif // MATERIAL_H
