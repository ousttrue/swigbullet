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

