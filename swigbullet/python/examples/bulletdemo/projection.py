from OpenGL.GL import *


class Projection(object):
    def __init__(self):
        self.zNear=1.0
        self.zFar=10000.0
        self.aspect=1.0

    def draw(self):
        glMatrixMode(GL_PROJECTION)
        glFrustum (
                -self.aspect * self.zNear, 
                self.aspect * self.zNear, 
                -self.zNear, 
                self.zNear, 
                self.zNear, 
                self.zFar);

