#include "projection.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


Projection::Projection()
    :
        zNear(1.0f),
        zFar(10000.0f),
        aspect(1.0f)
{
}


void Projection::draw()
{
    btVector3 extents(aspect, 1.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glFrustum (
            -aspect * zNear, 
            aspect * zNear, 
            -zNear, 
            zNear, 
            zNear, 
            zFar);
}

