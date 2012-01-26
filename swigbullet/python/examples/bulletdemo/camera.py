from OpenGL.GL import *
from OpenGL.GLU import *
import math
import swigbullet as bullet
from . import vector3


class Camera(object):

    def __init__(self, distance=15.0):
        self.m_ele=20.0
        self.m_azi=0.0
        self.m_cameraPosition=(0.0,0.0,0.0)
        self.m_cameraTargetPosition=(0.0,0.0,0.0)
        self.m_cameraUp=(0.0,1.0,0.0)
        self.m_forwardAxis=2
        self.m_zoomStepSize=0.4
        self.m_cameraDistance=distance
        self.m_glutScreenWidth=0
        self.m_glutScreenHeight=0
        self.m_frustumZNear=1.0
        self.m_frustumZFar=10000.0
        self.m_ortho=False
        self.stepsize=5.0;

    def stepLeft(self):
        self.m_azi -= self.stepsize;
        if (m_azi < 0.0):
            m_azi += 360.0

    def stepRight(self):
        m_azi += self.stepsize; 
        if (m_azi >= 360.0):
            m_azi -= 360.0; 

    def stepFront(self): 
        m_ele += self.stepsize; 
        if (m_ele >= 360.0):
            m_ele -= 360.0;

    def stepBack(self):
        m_ele -= self.stepsize; 
        if (m_ele < 0.0):
            m_ele += 360.0;

    def zoomIn(self):
        m_cameraDistance -= btScalar(self.m_zoomStepSize); 
        if (m_cameraDistance < 0.1):
            m_cameraDistance = 0.1;

    def zoomOut(self):
        m_cameraDistance += m_zoomStepSize;

    def resize(self, w, h):
        self.m_glutScreenWidth = w;
        self.m_glutScreenHeight = h;

    def getAspectRatio(self):
        return self.m_glutScreenWidth / float(self.m_glutScreenHeight);

    def getRayTo(self, x, y):
        if (m_ortho):
            aspect = self.getAspectRatio()
            extents=vector3.mul((aspect * 1.0, 1.0, 0.0), self.m_cameraDistance)
            lower = vector3.sub(m_cameraTargetPosition, extents);
            upper = vector3.add(m_cameraTargetPosition, extents);
            u = x / float(m_glutScreenWidth);
            v = (self.m_glutScreenHeight - y) / float(self.m_glutScreenHeight);
            return (
                    (1.0 - u) * lower[0] + u * upper[0],
                    (1.0 - v) * lower[1] + v * upper[1],
                    self.m_cameraTargetPosition[2]
                    );

        top = 1.0;
        bottom = -1.0;
        nearPlane = 1.0;
        tanFov = (top-bottom)*0.5 / nearPlane;
        fov = (2.0) * math.atan(tanFov);

        rayFrom = self.getCameraPosition();
        rayForward = vector3.normalize(
                vector3.sub(self.getCameraTargetPosition(), self.getCameraPosition()));
        farPlane = 10000.0;
        rayForward= vector3.mul(rayForward, farPlane);

        vertical = self.m_cameraUp[:];
        hor = vector3.normalize(vector3.cross(rayForward, vertical));
        vertical = vector3.normalize(vector3.cross(hor, rayForward));

        tanfov = math.tan(0.5*fov);

        hor *= 2.0 * farPlane * tanfov;
        vertical *= 2.0 * farPlane * tanfov;

        hor*=self.getAspectRatio();

        rayToCenter = vector3.add(rayFrom, rayForward);
        dHor = vector3.mul(hor, 1.0/float(self.m_glutScreenWidth));
        dVert = vector3.mul(vertical, 1.0/float(self.m_glutScreenHeight));
        rayTo = vector3.add(
                vector3.sub(rayToCenter, vector3.mul(hor, 0.5)), 
                vector3.mul(vertical, 0.5));
        rayTo = vector3.add(rayTo, vector3.mul(dHor * x));
        rayTo = vector3.sub(rayTo, vector3.mul(dVert * y));
        return rayTo;

    def draw(self):
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        rele = self.m_ele * (0.01745329251994329547); # rads per deg
        razi = self.m_azi * (0.01745329251994329547); # rads per deg

        rot=bullet.btQuaternion(self.m_cameraUp, razi);

        eyePos=[0.0, 0.0, 0.0];
        eyePos[self.m_forwardAxis] = -self.m_cameraDistance;
        eyePos=tuple(eyePos)

        forward=eyePos[:]
        if (vector3.length2(forward) < bullet.SIMD_EPSILON):
            forward=(1.0, 0.0, 0.0);
        right = vector3.cross(self.m_cameraUp, forward);
        roll=bullet.btQuaternion(right, -rele);

        m=(bullet.btMatrix3x3(rot) * bullet.btMatrix3x3(roll))
        eyePos = m.apply(eyePos);

        self.m_cameraPosition=[e + t for e, t in zip(eyePos, self.m_cameraTargetPosition)]

        if (self.m_glutScreenWidth == 0 and self.m_glutScreenHeight == 0):
            return;

        aspect = self.getAspectRatio()
        extents=(aspect, 1.0, 0.0);
        
        if (self.m_ortho):
            glLoadIdentity();
            extents = vector3.mul(extents, self.m_cameraDistance);
            lower = vector3.sub(self.m_cameraTargetPosition, extents);
            upper = vector3.add(self.m_cameraTargetPosition, extents);
            glOrtho(lower[0], upper[0], lower[1], upper[1], -1000, 1000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        else:
            glFrustum (
                    -aspect * self.m_frustumZNear, aspect * self.m_frustumZNear, 
                    -self.m_frustumZNear, self.m_frustumZNear, 
                    self.m_frustumZNear, self.m_frustumZFar);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(
                    self.m_cameraPosition[0], 
                    self.m_cameraPosition[1], 
                    self.m_cameraPosition[2], 
                    self.m_cameraTargetPosition[0], 
                    self.m_cameraTargetPosition[1], 
                    self.m_cameraTargetPosition[2], 
                    self.m_cameraUp[0],
                    self.m_cameraUp[1],
                    self.m_cameraUp[2]);

    """
    See http:#www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
    """
    def setOrthographicProjection(self):
        # switch to projection mode
        glMatrixMode(GL_PROJECTION);
        # save previous matrix which contains the 
        #settings for the perspective projection
        glPushMatrix();
        # reset matrix
        glLoadIdentity();
        # set a 2D orthographic projection
        gluOrtho2D(0, self.m_glutScreenWidth, 0, self.m_glutScreenHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        # invert the y axis, down is positive
        glScalef(1, -1, 1);
        # mover the origin from the bottom left corner
        # to the upper left corner
        glTranslatef(0.0, float(-self.m_glutScreenHeight), 0.0);

    def resetPerspectiveProjection(self):
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

    def move(self, dx, dy):
        hor = vector3.sub(self.getRayTo(0,0), self.getRayTo(1,0));
        vert = vector3.sub(self.getRayTo(0,0), self.getRayTo(0,1));
        multiplierX = (0.001);
        multiplierY = (0.001);
        if (self.m_ortho):
            multiplierX = 1;
            multiplierY = 1;
        self.m_cameraTargetPosition += vector3.mul(hor, dx * multiplierX);
        self.m_cameraTargetPosition += vector3.mul(vert, dy * multiplierY);

    def rot(self, dx, dy):
        self.m_azi += dx * (0.2);
        self.m_azi = math.mod(self.m_azi, (360.0));
        self.m_ele += dy * (0.2);
        self.m_ele = math.mod(self.m_ele, (180.0));

    def dolly(self, dy):
        self.m_cameraDistance -= dy * (0.02);
        if (m_cameraDistance<(0.1)):
            m_cameraDistance = (0.1);

