#include "view.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


View::View()
    :
        position(0.0, 0.0, 0.0),
        target(0.0, 0.0, 0.0),
        up(0.0, 1.0, 0.0),
        w(1), h(1)
{
}


void View::draw()
{
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(
            position[0], position[1], position[2], 
            target[0], target[1], target[2], 
            up.getX(), up.getY(), up.getZ());
}


btVector3 View::getRayTo(int x,int y)
{
    float top = 1.f;
    float bottom = -1.f;
    float nearPlane = 1.f;
    float tanFov = (top-bottom)*0.5f / nearPlane;
    float fov = 2.0f * btAtan(tanFov);

    btVector3 rayFrom = position;
    btVector3 rayForward = target-position;
    rayForward.normalize();
    float farPlane = 10000.f;
    rayForward*= farPlane;

    btVector3 vertical = up;

    btVector3 hor = rayForward.cross(vertical);
    hor.normalize();

    vertical = hor.cross(rayForward);
    vertical.normalize();

    float tanfov = tanf(0.5f*fov);

    hor *= 2.f * farPlane * tanfov;
    vertical *= 2.f * farPlane * tanfov;

    hor*=(w / btScalar(h));

    btVector3 rayToCenter = rayFrom + rayForward;
    btVector3 dHor = hor * 1.f/btScalar(w);
    btVector3 dVert = vertical * 1.f/btScalar(h);

    btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
    rayTo += btScalar(x) * dHor;
    rayTo -= btScalar(y) * dVert;
    return rayTo;
}

