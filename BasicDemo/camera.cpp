#include "camera.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS
#include <iostream>


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


Camera::Camera()
    :
        m_ele(20.f),
        m_azi(0.f),
        m_cameraDistance(50.0),
        m_zoomStepSize(0.4), 
        m_stepSize(5.0)
{
    update();
}


void Camera::move(int dx, int dy)
{
    btVector3 hor = m_view.getRayTo(0, 0)-m_view.getRayTo(1, 0);
    btVector3 vert = m_view.getRayTo(0, 0)-m_view.getRayTo(0, 1);
    m_view.target += hor* dx * 0.001f;
    m_view.target += vert* dy * 0.001f;
}


void Camera::draw()
{
    m_projection.draw();
    m_view.draw();
}


//See http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
void setOrthographicProjection(int w, int h)
{
    // switch to projection mode
    // set a 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // mover the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(btScalar(0), btScalar(-h), btScalar(0));
}

