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
        m_cameraDistance(15.0),
        m_frustumZNear(1.f),
        m_frustumZFar(10000.f),
        m_ortho(0)
{
}

Camera::~Camera()
{
}


void Camera::stepLeft() 
{ 
    m_azi -= m_stepSize;
    update();
}

void Camera::stepRight() 
{ 
    m_azi += m_stepSize;
    update();
}

void Camera::stepFront() 
{ 
    m_ele += m_stepSize;
    update();
}

void Camera::stepBack() 
{ 
    m_ele -= m_stepSize;
    update();
}

void Camera::zoomIn() 
{ 
    m_cameraDistance -= btScalar(m_zoomStepSize);
    update();
}

void Camera::zoomOut() 
{ 
    m_cameraDistance += btScalar(m_zoomStepSize);
    update();
}

void Camera::move(int dx, int dy, int w, int h)
{
    btVector3 hor = getRayTo(0,0, w, h)-getRayTo(1,0, w, h);
    btVector3 vert = getRayTo(0,0, w, h)-getRayTo(0,1, w, h);
    btScalar multiplierX = btScalar(0.001);
    btScalar multiplierY = btScalar(0.001);
    if (getOrtho())
    {
        multiplierX = 1;
        multiplierY = 1;
    }
    m_cameraTargetPosition += hor* dx * multiplierX;
    m_cameraTargetPosition += vert* dy * multiplierY;
}

void Camera::rot(int dx, int dy)
{
    m_azi += dx * btScalar(0.2);
    m_azi = fmodf(m_azi, btScalar(360.f));
    m_ele += dy * btScalar(0.2);
    m_ele = fmodf(m_ele, btScalar(180.f));
    update();
}

void Camera::dolly(int dy)
{
    m_cameraDistance -= dy * btScalar(0.02f);
    update();
}


void Camera::reshape(int w, int h)
{
    m_aspect= w / (btScalar)h;
    update();
}

btVector3 Camera::getRayTo(int x,int y, int w, int h)
{
    if (getOrtho()) {

        btVector3 extents;
        extents.setValue(m_aspect * 1.0f, 1.0f,0);

        extents *= m_cameraDistance;
        btVector3 lower = m_cameraTargetPosition - extents;
        btVector3 upper = m_cameraTargetPosition + extents;

        btScalar u = x / btScalar(w);
        btScalar v = (h - y) / btScalar(h);

        btVector3 p(0,0,0);
        p.setValue((1.0f - u) * lower.getX() + u * upper.getX(),(1.0f - v) * lower.getY() + v * upper.getY(),m_cameraTargetPosition.getZ());
        return p;
    }
    else{
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
}


void Camera::draw()
{
    if (m_ortho)
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
    else {
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
}


void Camera::update()
{
    // clamp
    if (m_azi < 0){
        m_azi += 360;
    }
    if (m_azi > 360){
        m_azi -= 360;
    }
    if (m_ele < 0){
        m_ele += 360;
    }
    if (m_ele > 360){
        m_ele -= 360;
    }
    if (m_cameraDistance < btScalar(0.1)){
        m_cameraDistance = btScalar(0.1);
    }
    /*
    std::cout 
        << "m_azi: " << m_azi << ','
        << "m_ele: " << m_ele << ','
        << "m_cameraDistance: " << m_cameraDistance << std::endl
        ;
        */

    // calc camera position
    btScalar razi = m_azi * btScalar(0.01745329251994329547);// rads per deg
    btQuaternion head(m_cameraUp, razi);

    btVector3 eyePos(0.0f, 0.0f, -m_cameraDistance);
    btVector3 forward=eyePos;
    if (forward.length2() < SIMD_EPSILON) {
        forward.setValue(1.0f, 0.0f, 0.0f);
    }
    btVector3 right=m_cameraUp.cross(forward);
    btScalar rele = m_ele * btScalar(0.01745329251994329547);// rads per deg
    btQuaternion pitch(right, -rele);

    m_cameraPosition=btMatrix3x3(head)*btMatrix3x3(pitch)*eyePos + m_cameraTargetPosition;

    /*
    std::cout 
        << m_cameraPosition[0] << ','
        << m_cameraPosition[1] << ','
        << m_cameraPosition[2] << std::endl
        ;
        */
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

