#include "triangle.h"

Triangle::Triangle()
{

}

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    _p1 = p1;
    _p2 = p2;
    _p3 = p3;

    _e12 = _p2 - _p1;
    _e23 = _p3 - _p2;
    _e31 = _p1 - _p3;
    _e13 = - _e31;

    _n = glm::normalize(glm::cross(_p2 - _p1, _p3 - _p2));
}

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
    _p1 = p1;
    _p2 = p2;
    _p3 = p3;

    _e12 = _p2 - _p1;
    _e23 = _p3 - _p2;
    _e31 = _p1 - _p3;
    _e13 = - _e31;

    _t1 = t1;
    _t2 = t2;
    _t3 = t3;

    _n = glm::normalize(glm::cross(_p2 - _p1, _p3 - _p2));
}

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n)
{
    _p1 = p1;
    _p2 = p2;
    _p3 = p3;

    _e12 = _p2 - _p1;
    _e23 = _p3 - _p2;
    _e31 = _p1 - _p3;
    _e13 = - _e31;

    n = glm::normalize(n);
}

bool Triangle::isInsideTriangle(glm::vec3 point)
{
    //Test if every point is on the left side of the edges
//    if (glm::dot( glm::cross(_e12,_n), point - _p1) > 0 ||
//        glm::dot( glm::cross(_e23,_n), point - _p2) > 0 ||
//        glm::dot( glm::cross(_e31,_n), point - _p3) > 0 )
//        return false;
//    return true;
    if (glm::dot( _n, glm::cross(_e12, point - _p1)) < 0 ||
        glm::dot( _n, glm::cross(_e23, point - _p2)) < 0 ||
        glm::dot( _n, glm::cross(_e31, point - _p3)) < 0 )
        return false;
    return true;
}

glm::vec3 Triangle::p1()
{
    return _p1;
}

glm::vec3 Triangle::p2()
{
    return _p2;
}

glm::vec3 Triangle::p3()
{
    return _p3;
}

glm::vec3 Triangle::t1()
{
    return _t1;
}

glm::vec3 Triangle::t2()
{
    return _t2;
}

glm::vec3 Triangle::t3()
{
    return _t3;
}

glm::vec3 Triangle::normal()
{
    return _n;
}
