from OpenGL.GL import *
from OpenGL.GLU import *
import math
from .vector3 import Vector3


class View(object):
    def __init__(self):
        self.position=Vector3(0.0, 0.0, 0.0)
        self.target=Vector3(0.0, 0.0, 0.0)
        self.up=Vector3(0.0, 1.0, 0.0)
        self.w=1
        self.h=1

    def draw(self):
        glMatrixMode(GL_MODELVIEW)
        gluLookAt(
                self.position[0], self.position[1], self.position[2], 
                self.target[0], self.target[1], self.target[2], 
                self.up[0], self.up[1], self.up[2])

    def getRayTo(self, x, y):
        top = 1.0
        bottom = -1.0
        nearPlane = 1.0
        tanFov = (top-bottom)*0.5 / nearPlane
        fov = 2.0 * math.atan(tanFov)

        rayFrom = self.position[:]
        rayForward = vector3.normalize(vector3.sub(target, position))
        farPlane = 10000.0
        rayForward=vector3.mul(rayForward, farPlane)

        vertical = up[:]

        hor = vector3.normalize(vector3.cross(rayForward, vertical))

        vertical = vector3.normalize(vector3.cross(hor, rayForward))

        tanfov = math.tan(0.5*fov)

        hor = vector3.mul(hor, 2.0 * farPlane * tanfov)
        vertical = vector3.mul(vertical, 2.0 * farPlane * tanfov)

        hor*=vector3.mul(hor, w / float(h))

        rayToCenter = rayFrom+rayForward
        dHor = hor*1.0/float(w)
        dVert = vertical*1.0/float(h)

        rayTo = rayToCenter - 0.5 * vector3.mul(hor + 0.5 * vertical)
        rayTo += float(x) * dHor
        rayTo -= float(y) * dVert
        return rayTo

