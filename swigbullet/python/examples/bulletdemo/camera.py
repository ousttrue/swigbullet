from OpenGL.GL import *
from OpenGL.GLU import *
import math
import swigbullet as bullet
from .vector3 import Vector3
from .view import View
from .projection import Projection


#See http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
def setOrthographicProjection(w, h):
    # switch to projection mode
    # set a 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    # invert the y axis, down is positive
    glScalef(1, -1, 1);
    # mover the origin from the bottom left corner
    # to the upper left corner
    glTranslatef(0.0, float(-h), 0.0);


class Camera(object):
    def __init__(self):
        self.m_view=View()
        self.m_projection=Projection()
        self.m_ele=20.0
        self.m_azi=0.0
        self.m_cameraDistance=50.0
        self.m_zoomStepSize=0.4 
        self.m_stepSize=5.0
        self.update()

    def move(self, dx, dy):
        hor = self.m_view.getRayTo(0, 0)-self.m_view.getRayTo(1, 0)
        vert = self.m_view.getRayTo(0, 0)-self.m_view.getRayTo(0, 1)
        self.m_view.target += (hor*dx + vert*dy)* 0.001

    def draw(self):
        self.m_projection.draw()
        self.m_view.draw()

    def stepLeft(self):
        self.m_azi -= self.m_stepSize
        update()

    def stepRight(self):
        self.m_azi += self.m_stepSize
        update()

    def stepFront(self):
        self.m_ele += self.m_stepSize
        update()

    def stepBack(self):
        self.m_ele -= self.m_stepSize
        update()

    def zoomIn(self):
        self.m_cameraDistance -= self.m_zoomStepSize
        update()

    def zoomOut(self):
        m_cameraDistance += self.m_zoomStepSize
        update()

    def rot(self, dx, dy):
        self.m_azi += dx * 0.2
        self.m_azi = math.fmod(self.m_azi, 360.0)
        self.m_ele += dy * 0.2
        self.m_ele = math.fmod(self.m_ele, 180.0)
        self.update()

    def dolly(self, dy):
        self.m_cameraDistance -= dy * 0.02
        self.update()

    def resize(self, w, h):
        self.m_view.w=w
        self.m_view.h=h
        self.m_projection.aspect= w / float(h)
        self.update()

    def update(self):
        # clamp
        while (self.m_azi < 0):
            self.m_azi += 360
        while (self.m_azi > 360):
            self.m_azi -= 360
        while (self.m_ele < 0):
            self.m_ele += 360
        while (self.m_ele > 360):
            self.m_ele -= 360
        if (self.m_cameraDistance < 0.1):
            self.m_cameraDistance = 0.1

        # calc camera position
        razi = self.m_azi * 0.01745329251994329547 # rads per deg
        head=bullet.btQuaternion(self.m_view.up, razi)

        eyePos=Vector3(0.0, 0.0, -self.m_cameraDistance)
        forward=Vector3(*eyePos[:])
        if (forward.length2() < bullet.SIMD_EPSILON):
            forward=Vector3(1.0, 0.0, 0.0)
        right=self.m_view.up.cross(forward)
        rele = self.m_ele * 0.01745329251994329547 # rads per deg
        pitch=bullet.btQuaternion(right, -rele)

        self.m_view.position=self.m_view.target+(
                bullet.btMatrix3x3(head)*bullet.btMatrix3x3(pitch)).apply(eyePos);

