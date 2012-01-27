#ifndef CAMERA_H
#define CAMERA_H


#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"


class Camera
{
protected:
    // euler angle
    float m_ele;
    float m_azi;

    btVector3 m_cameraPosition;
    //look at
    btVector3 m_cameraTargetPosition;
    btVector3 m_cameraUp;

    float m_stepSize;
    float m_zoomStepSize;
    float m_cameraDistance;

    // projection
    float m_aspect;
    float m_frustumZNear;
    float m_frustumZFar;

public:
    Camera();
    virtual ~Camera(){}
    void setAzi(float azi)
    {
        m_azi = azi;
    }
    void setCameraUp(const btVector3& camUp)
    {
        m_cameraUp = camUp;
    }
    btVector3 getCameraPosition()
    {
        return m_cameraPosition;
    }
    btVector3 getCameraTargetPosition()
    {
        return m_cameraTargetPosition;
    }
    void setFrustumZPlanes(float zNear, float zFar)
    {
        m_frustumZNear = zNear;
        m_frustumZFar = zFar;
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
    void reshape(int w, int h)
    {
        m_aspect= w / (btScalar)h;
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
    }

    virtual void move(int dx, int dy, int w, int h)=0;
    virtual btVector3 getRayTo(int x,int y, int w, int h)=0;
    virtual void draw()=0;
};


class PerspectiveCamera : public Camera
{
public:
    virtual void move(int dx, int dy, int w, int h);
    virtual btVector3 getRayTo(int x,int y, int w, int h);
    virtual void draw();
};


void setOrthographicProjection(int w, int h);


#endif // CAMERA_H
