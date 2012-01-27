#ifndef CAMERA_H
#define CAMERA_H


#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"


struct Projection
{
    float aspect;
    float zNear;
    float zFar;
    Projection();
    void draw();
};

struct View
{
    btVector3 position;
    btVector3 target;
    btVector3 up;
    int w;
    int h;
    View();
    btVector3 getRayTo(int x,int y);
    void draw();
};

class Camera
{
    float m_ele;
    float m_azi;
    float m_cameraDistance;

    float m_stepSize;
    float m_zoomStepSize;

public:
    View m_view;
    Projection m_projection;

    Camera();
    void setAzi(float azi)
    {
        m_azi = azi;
    }
    void setCameraDistance(float dist)
    {
        m_cameraDistance = dist;
    }
    float getCameraDistance()
    {
        return m_cameraDistance;
    }
    void stepLeft() 
    { 
        m_azi -= m_stepSize;
        update();
    }
    void stepRight() 
    { 
        m_azi += m_stepSize;
        update();
    }
    void stepFront() 
    { 
        m_ele += m_stepSize;
        update();
    }
    void stepBack() 
    { 
        m_ele -= m_stepSize;
        update();
    }
    void zoomIn() 
    { 
        m_cameraDistance -= btScalar(m_zoomStepSize);
        update();
    }
    void zoomOut() 
    { 
        m_cameraDistance += btScalar(m_zoomStepSize);
        update();
    }
    void rot(int dx, int dy)
    {
        m_azi += dx * btScalar(0.2);
        m_azi = fmodf(m_azi, btScalar(360.f));
        m_ele += dy * btScalar(0.2);
        m_ele = fmodf(m_ele, btScalar(180.f));
        update();
    }
    void dolly(int dy)
    {
        m_cameraDistance -= dy * btScalar(0.02f);
        update();
    }
    void resize(int w, int h)
    {
        m_view.w=w;
        m_view.h=h;
        m_projection.aspect= w / (btScalar)h;
        update();
    }
    void update()
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

        // calc camera position
        btScalar razi = m_azi * btScalar(0.01745329251994329547);// rads per deg
        btQuaternion head(m_view.up, razi);

        btVector3 eyePos(0.0f, 0.0f, -m_cameraDistance);
        btVector3 forward=eyePos;
        if (forward.length2() < SIMD_EPSILON) {
            forward.setValue(1.0f, 0.0f, 0.0f);
        }
        btVector3 right=m_view.up.cross(forward);
        btScalar rele = m_ele * btScalar(0.01745329251994329547);// rads per deg
        btQuaternion pitch(right, -rele);

        m_view.position=m_view.target+btMatrix3x3(head)*btMatrix3x3(pitch)*eyePos;
    }

    void move(int dx, int dy);
    void draw();
};


void setOrthographicProjection(int w, int h);


#endif // CAMERA_H
