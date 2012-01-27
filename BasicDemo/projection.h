#ifndef PROJECTION_H
#define PROJECTION_H

#include "LinearMath/btVector3.h"


struct Projection
{
    float aspect;
    float zNear;
    float zFar;
    Projection();
    void draw();
};


#endif
