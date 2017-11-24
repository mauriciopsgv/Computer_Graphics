#ifndef CAMERA_H
#define CAMERA_H

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

struct Camera {
   glm::vec3 eye;      /* posicao do olho ou centro de projecao conica */
   glm::vec3 at;       /* ponto para onde a camera aponta              */
   glm::vec3 up;       /* orientacao da direcao "para cima" da camera  */
   float fovy;         /* angulo de abertura da camera                 */
   float zNear,zFar;   /* distancia do plano proximo e distante        */
   float width,height; /* largura e altura da janela em pixels         */
};

#endif // CAMERA_H
