#ifndef CAMERA_H
#define CAMERA_H


#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"


class Camera
{
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
    int m_ortho;

public:
    Camera();
    virtual ~Camera();

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
    void toggleOrtho()
    {
        m_ortho = !m_ortho;
    }
    void setCameraDistance(float dist)
    {
        m_cameraDistance = dist;
    }
    float getCameraDistance()
    {
        return m_cameraDistance;
    }
    int getOrtho()
    { 
        return m_ortho; 
    }

    void stepLeft();
    void stepRight();
    void stepFront();
    void stepBack();
    void zoomIn();
    void zoomOut();
    void move(int dx, int dy, int w, int h);
    void rot(int dx, int dy);
    void dolly(int dy);

    void reshape(int w, int h);
    void update();
    void draw();
    btVector3 getRayTo(int x,int y, int w, int h);
};

void setOrthographicProjection(int w, int h);


#endif // CAMERA_H
