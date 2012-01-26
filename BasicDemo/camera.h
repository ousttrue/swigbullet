#ifndef CAMERA_H
#define CAMERA_H


#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btMatrix3x3.h"


class Camera
{
	float m_ele;
	float m_azi;
	btVector3 m_cameraPosition;
	btVector3 m_cameraTargetPosition;//look at
	btVector3 m_cameraUp;
	int	m_forwardAxis;
	float m_zoomStepSize;
	float	m_cameraDistance;
	int m_glutScreenWidth;
	int m_glutScreenHeight;
	float	m_frustumZNear;
	float	m_frustumZFar;
	int	m_ortho;

public:
    Camera();
    virtual ~Camera();

	void stepLeft();
	void stepRight();
	void stepFront();
	void stepBack();
	void zoomIn();
	void zoomOut();
	void	setAzi(float azi)
	{
		m_azi = azi;
	}
	void	setCameraUp(const btVector3& camUp)
	{
		m_cameraUp = camUp;
	}
	void	setCameraForwardAxis(int axis)
	{
		m_forwardAxis = axis;
	}
	void updateCamera();
	btVector3	getCameraPosition()
	{
		return m_cameraPosition;
	}
	btVector3	getCameraTargetPosition()
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
			m_ortho = !m_ortho;//m_stepping = !m_stepping;
    }
    int getOrtho(){ return m_ortho; }
	btVector3	getRayTo(int x,int y);
	float	getCameraDistance();
	void	setCameraDistance(float dist);	
    void reshape(int w, int h);
	void setOrthographicProjection();
	void resetPerspectiveProjection();
    void move(int dx, int dy);
    void rot(int dx, int dy);
    void dolly(int dy);
};


#endif // CAMERA_H
