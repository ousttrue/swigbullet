#include "camera.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


Camera::Camera()
    :
m_ele(20.f),
m_azi(0.f),
m_cameraPosition(0.f,0.f,0.f),
m_cameraTargetPosition(0.f,0.f,0.f),
m_cameraUp(0,1,0),
m_forwardAxis(2),
m_zoomStepSize(0.4),	
m_cameraDistance(15.0),
m_glutScreenWidth(0),
m_glutScreenHeight(0),
m_frustumZNear(1.f),
m_frustumZFar(10000.f),
m_ortho(0)
{
}

Camera::~Camera()
{
}

const float STEPSIZE = 5;

void Camera::stepLeft() 
{ 
	m_azi -= STEPSIZE; if (m_azi < 0) m_azi += 360; updateCamera(); 
}
void Camera::stepRight() 
{ 
	m_azi += STEPSIZE; if (m_azi >= 360) m_azi -= 360; updateCamera(); 
}
void Camera::stepFront() 
{ 
	m_ele += STEPSIZE; if (m_ele >= 360) m_ele -= 360; updateCamera(); 
}
void Camera::stepBack() 
{ 
	m_ele -= STEPSIZE; if (m_ele < 0) m_ele += 360; updateCamera(); 
}
void Camera::zoomIn() 
{ 
	m_cameraDistance -= btScalar(m_zoomStepSize); updateCamera(); 
	if (m_cameraDistance < btScalar(0.1))
		m_cameraDistance = btScalar(0.1);

}
void Camera::zoomOut() 
{ 
	m_cameraDistance += btScalar(m_zoomStepSize); updateCamera(); 

}

void	Camera::setCameraDistance(float dist)
{
	m_cameraDistance  = dist;
}

float	Camera::getCameraDistance()
{
	return m_cameraDistance;
}

void Camera::reshape(int w, int h)
{
	m_glutScreenWidth = w;
	m_glutScreenHeight = h;
	updateCamera();
}

btVector3 Camera::getRayTo(int x,int y)
{
	if (getOrtho())
	{

		btScalar aspect;
		btVector3 extents;
		aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
		extents.setValue(aspect * 1.0f, 1.0f,0);
		
		extents *= m_cameraDistance;
		btVector3 lower = m_cameraTargetPosition - extents;
		btVector3 upper = m_cameraTargetPosition + extents;

		btScalar u = x / btScalar(m_glutScreenWidth);
		btScalar v = (m_glutScreenHeight - y) / btScalar(m_glutScreenHeight);
		
		btVector3	p(0,0,0);
		p.setValue((1.0f - u) * lower.getX() + u * upper.getX(),(1.0f - v) * lower.getY() + v * upper.getY(),m_cameraTargetPosition.getZ());
		return p;
	}

	float top = 1.f;
	float bottom = -1.f;
	float nearPlane = 1.f;
	float tanFov = (top-bottom)*0.5f / nearPlane;
	float fov = btScalar(2.0) * btAtan(tanFov);

	btVector3	rayFrom = getCameraPosition();
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

	btScalar aspect;
	
	aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
	
	hor*=aspect;


	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.f/float(m_glutScreenWidth);
	btVector3 dVert = vertical * 1.f/float(m_glutScreenHeight);


	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
	rayTo += btScalar(x) * dHor;
	rayTo -= btScalar(y) * dVert;
	return rayTo;
}

void Camera::updateCamera() {


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	btScalar rele = m_ele * btScalar(0.01745329251994329547);// rads per deg
	btScalar razi = m_azi * btScalar(0.01745329251994329547);// rads per deg


	btQuaternion rot(m_cameraUp,razi);


	btVector3 eyePos(0,0,0);
	eyePos[m_forwardAxis] = -m_cameraDistance;

	btVector3 forward(eyePos[0],eyePos[1],eyePos[2]);
	if (forward.length2() < SIMD_EPSILON)
	{
		forward.setValue(1.f,0.f,0.f);
	}
	btVector3 right = m_cameraUp.cross(forward);
	btQuaternion roll(right,-rele);

	eyePos = btMatrix3x3(rot) * btMatrix3x3(roll) * eyePos;

	m_cameraPosition[0] = eyePos.getX();
	m_cameraPosition[1] = eyePos.getY();
	m_cameraPosition[2] = eyePos.getZ();
	m_cameraPosition += m_cameraTargetPosition;

	if (m_glutScreenWidth == 0 && m_glutScreenHeight == 0)
		return;

	btScalar aspect;
	btVector3 extents;

	aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
	extents.setValue(aspect * 1.0f, 1.0f,0);
	
	
	if (m_ortho)
	{
		// reset matrix
		glLoadIdentity();
		
		
		extents *= m_cameraDistance;
		btVector3 lower = m_cameraTargetPosition - extents;
		btVector3 upper = m_cameraTargetPosition + extents;
		//gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
		glOrtho(lower.getX(), upper.getX(), lower.getY(), upper.getY(),-1000,1000);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glTranslatef(100,210,0);
	} else
	{
//		glFrustum (-aspect, aspect, -1.0, 1.0, 1.0, 10000.0);
		glFrustum (-aspect * m_frustumZNear, aspect * m_frustumZNear, -m_frustumZNear, m_frustumZNear, m_frustumZNear, m_frustumZFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(m_cameraPosition[0], m_cameraPosition[1], m_cameraPosition[2], 
			m_cameraTargetPosition[0], m_cameraTargetPosition[1], m_cameraTargetPosition[2], 
			m_cameraUp.getX(),m_cameraUp.getY(),m_cameraUp.getZ());
	}

}

//See http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
void Camera::setOrthographicProjection() 
{

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, m_glutScreenWidth, 0, m_glutScreenHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(btScalar(0), btScalar(-m_glutScreenHeight), btScalar(0));

}

void Camera::resetPerspectiveProjection() 
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	updateCamera();
}

void Camera::move(int dx, int dy)
{
    btVector3 hor = getRayTo(0,0)-getRayTo(1,0);
    btVector3 vert = getRayTo(0,0)-getRayTo(0,1);
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
}

void Camera::dolly(int dy)
{
    m_cameraDistance -= dy * btScalar(0.02f);
    if (m_cameraDistance<btScalar(0.1))
        m_cameraDistance = btScalar(0.1);
}

