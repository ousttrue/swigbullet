#include "camera.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS
#include <iostream>



Camera::Camera()
    :
        m_ele(20.f),
        m_azi(0.f),
        m_cameraPosition(0.f,0.f,0.f),
        m_cameraTargetPosition(0.f,0.f,0.f),
        m_cameraUp(0,1,0),
        m_zoomStepSize(0.4), 
        m_stepSize(5.0),
        m_cameraDistance(50.0),
        m_frustumZNear(1.f),
        m_frustumZFar(10000.f)
{
    update();
}


void OrthogonalCamera::move(int dx, int dy, int w, int h)
{
    btVector3 hor = getRayTo(0,0, w, h)-getRayTo(1,0, w, h);
    btVector3 vert = getRayTo(0,0, w, h)-getRayTo(0,1, w, h);
    m_cameraTargetPosition += hor* dx;
    m_cameraTargetPosition += vert* dy;
}


void PerspectiveCamera::move(int dx, int dy, int w, int h)
{
    btVector3 hor = getRayTo(0,0, w, h)-getRayTo(1,0, w, h);
    btVector3 vert = getRayTo(0,0, w, h)-getRayTo(0,1, w, h);
    m_cameraTargetPosition += hor* dx * btScalar(0.001);
    m_cameraTargetPosition += vert* dy * btScalar(0.001);
}


btVector3 OrthogonalCamera::getRayTo(int x,int y, int w, int h)
{
    btVector3 extents(m_aspect * 1.0f, 1.0f,0);

    extents *= m_cameraDistance;
    btVector3 lower = m_cameraTargetPosition - extents;
    btVector3 upper = m_cameraTargetPosition + extents;

    btScalar u = x / btScalar(w);
    btScalar v = (h - y) / btScalar(h);

    btVector3 p(0,0,0);
    p.setValue(
            (1.0f - u) * lower.getX() + u * upper.getX(),
            (1.0f - v) * lower.getY() + v * upper.getY(),
            //m_cameraTargetPosition.getZ());
            -1000.0f);
    return p;
}


btVector3 PerspectiveCamera::getRayTo(int x,int y, int w, int h)
{
    float top = 1.f;
    float bottom = -1.f;
    float nearPlane = 1.f;
    float tanFov = (top-bottom)*0.5f / nearPlane;
    float fov = btScalar(2.0) * btAtan(tanFov);

    btVector3 rayFrom = getCameraPosition();
    btVector3 rayForward = (getCameraTargetPosition()-getCameraPosition());
    rayForward.normalize();
    float farPlane = 10000.f;
    rayForward*= farPlane;

    btVector3 rightOffset;
    btVector3 vertical = m_cameraUp;

    btVector3 hor;
    hor = rayForward.cross(vertical);
    hor.normalize();
    vertical = hor.cross(rayForward);
    vertical.normalize();

    float tanfov = tanf(0.5f*fov);

    hor *= 2.f * farPlane * tanfov;
    vertical *= 2.f * farPlane * tanfov;

    hor*=m_aspect;

    btVector3 rayToCenter = rayFrom + rayForward;
    btVector3 dHor = hor * 1.f/btScalar(w);
    btVector3 dVert = vertical * 1.f/btScalar(h);

    btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
    rayTo += btScalar(x) * dHor;
    rayTo -= btScalar(y) * dVert;
    return rayTo;
}


void OrthogonalCamera::draw()
{
    btVector3 extents=btVector3(m_aspect * 1.0f, 1.0f,0) * m_cameraDistance;
    btVector3 lower = m_cameraTargetPosition - extents;
    btVector3 upper = m_cameraTargetPosition + extents;
    glMatrixMode(GL_PROJECTION);
    //gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
    glOrtho(lower.getX(), upper.getX(), lower.getY(), upper.getY(),-1000,1000);
    glMatrixMode(GL_MODELVIEW);
    //glTranslatef(100,210,0);
} 


void PerspectiveCamera::draw()
{
    btVector3 extents(m_aspect * 1.0f, 1.0f,0);
    glMatrixMode(GL_PROJECTION);
    // glFrustum (-m_aspect, m_aspect, -1.0, 1.0, 1.0, 10000.0);
    glFrustum (
            -m_aspect * m_frustumZNear, 
            m_aspect * m_frustumZNear, 
            -m_frustumZNear, 
            m_frustumZNear, 
            m_frustumZNear, 
            m_frustumZFar);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(
            m_cameraPosition[0], 
            m_cameraPosition[1], 
            m_cameraPosition[2], 
            m_cameraTargetPosition[0], 
            m_cameraTargetPosition[1], 
            m_cameraTargetPosition[2], 
            m_cameraUp.getX(),
            m_cameraUp.getY(),
            m_cameraUp.getZ());
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

